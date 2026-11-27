# CHIP-8 Emulator — Developer Documentation

Technical reference and implementation notes for the CHIP-8 interpreter and emulator.

**Reference:** [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8)

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Memory Layout](#memory-layout)
3. [CPU Architecture](#cpu-architecture)
4. [Keyboard Input](#keyboard-input)
5. [Display and Graphics](#display-and-graphics)
6. [Fontset and Sprites](#fontset-and-sprites)
7. [Timers](#timers)
8. [Instruction Set](#instruction-set)
9. [Opcode Decoding](#opcode-decoding)
10. [Implementation Considerations](#implementation-considerations)

---

# Architecture Overview

CHIP-8 is an interpreted programming environment originally developed in the 1970s. It provides a small virtual machine with memory, registers, timers, a display, and keyboard input.

The emulator consists of several logical components:

| Component | Responsibility |
|---|---|
| Memory | Provides the 4 KB address space and memory access operations. |
| CPU | Fetches, decodes, and executes instructions. |
| Display | Maintains the 64 × 32 monochrome framebuffer. |
| Input | Handles the 16-key hexadecimal keypad. |
| Frontend | Handles window management, rendering, and user input. |
| Emulator | Coordinates CPU execution, timers, and frontend updates. |

The CPU owns its execution state, including general-purpose registers, special registers, timers, and the call stack. These are not physically memory-mapped hardware registers.

---

# Memory Layout

## Address Space

CHIP-8 provides a 4 KB (4096-byte) address space.

- Address width: 12 bits.
- Address range: `0x0000–0x0FFF`.
- Program entry point: `0x0200`.

The first 512 bytes are reserved for interpreter data. Programs are conventionally loaded starting at address `0x0200`.

| Region | Start | End | Size |
|---|---:|---:|---:|
| Fontset | `0x0000` | `0x004F` | 80 bytes |
| Mappings | `0x0050` | `0x0175` | 294 bytes |
| Unused | `0x0176` | `0x01FF` | 138 bytes |
| Program RAM | `0x0200` | `0x0FFF` | 3584 bytes |
| **Total** | `0x0000` | `0x0FFF` | **4096 bytes** |

The interpreter region consists of the first three regions and occupies exactly 512 bytes.

## Memory Map

```text
Address Space: 4096 bytes

0x0000  +--------------------------------+
        |                                |
        |          FONTSET               |
        |          80 bytes              |
        |                                |
0x004F  +--------------------------------+

0x0050  +--------------------------------+ ---------------------+
        |                                |
        |          Interpreter           |      UNUSED
        |          432 bytes             |
        |                                |
0x01FF  +--------------------------------+ --------------------+

0x0200  +--------------------------------+
        |                                |
        |          PROGRAM RAM           |
        |          3584 bytes            |
        |                                |
0x0FFF  +--------------------------------+

        Total: 4096 bytes
```

### Interpreter Data Regions

The interpreter area is divided into three logical regions:

- **Fontset:** Stores the built-in hexadecimal character sprites.
- **Interpreter:** Interpreter source code address space. **Unused.**
- **Program RAM:** This is where the loaded ROM is stored, program execution begins at **0x0200.**

> CPU registers and the return-address stack are maintained by the CPU component rather than
stored in these memory regions.

### Program Loading

Programs are conventionally loaded at `0x0200`.

```cpp
static constexpr Address PROGRAM_START = 0x0200;
```

The available program storage is:

```cpp
static constexpr std::size_t PROGRAM_RAM_SIZE = 3584;
```

A program loader must ensure that the ROM does not exceed the available program memory.

---

# CPU Architecture

The CHIP-8 CPU maintains the execution state required to interpret instructions.

## General-Purpose Registers

CHIP-8 provides sixteen 8-bit general-purpose registers:

```text
V0, V1, V2, V3, V4, V5, V6, V7,
V8, V9, VA, VB, VC, VD, VE, VF
```

Each register stores an unsigned 8-bit value.

```cpp
std::array<Byte, 16> m_v {};
```

The `VF` register is also used as a flag register by several arithmetic and graphics instructions.

Programs should not assume that `VF` retains its previous value after instructions that use
it as a flag.

## Special Registers

| Register | Width | Responsibility |
|---|---:|---|
| I | 16 bits | Index register used for memory addressing. |
| PC | 16 bits | Program counter. |
| SP | 8 bits | Stack pointer. |

Although `I` and `PC` are 16-bit registers, the original CHIP-8 address space uses only
12 address bits.

### Index Register (I)

The index register stores a memory address, commonly used to access sprites and data.

Instructions involving `I` include:

- `ANNN`: Set I to address NNN.
- `FX1E`: Add Vx to I.
- `FX29`: Point I to the hexadecimal font sprite for Vx.
- `FX33`: Store the binary-coded decimal representation of Vx.
- `FX55`: Store V0 through Vx in memory.
- `FX65`: Load memory values into V0 through Vx.

### Program Counter (PC)

The program counter identifies the address of the next instruction to execute.

The conventional starting address is:

```cpp
Word m_pc { 0x0200 };
```

CHIP-8 instructions are two bytes long, so normal sequential execution advances the PC by two bytes.

Control-flow instructions can modify the PC directly.

### Stack Pointer (SP) and Stack

The stack stores return addresses when executing subroutines.

CHIP-8 implementations conventionally support at least 16 nested calls.

Each stack entry stores a 16-bit return address.

```cpp
std::array<Word, 16> m_stack {};
Byte m_sp {};
```

The stack is CPU execution state and is not part of the CHIP-8 memory address space.

### CPU Clock

The CPU execution rate and the timer update frequency are separate concepts.

The original CHIP-8 specification does not mandate a universal instruction execution frequency.
An emulator should make its instruction rate configurable.

The delay and sound timers, however, decrement at 60 Hz.

---

# Keyboard Input

CHIP-8 provides a hexadecimal keypad consisting of sixteen keys:

```text
+---+---+---+---+
| 1 | 2 | 3 | C |
+---+---+---+---+
| 4 | 5 | 6 | D |
+---+---+---+---+
| 7 | 8 | 9 | E |
+---+---+---+---+
| A | 0 | B | F |
+---+---+---+---+
```

Each key represents a hexadecimal value between `0x0` and `0xF`.

Modern emulators typically map these keys to a conventional keyboard layout.

A common mapping is:

```text
CHIP-8       Keyboard

1 2 3 C      1 2 3 4
4 5 6 D      Q W E R
7 8 9 E      A S D F
A 0 B F      Z X C V
```

The emulator should maintain the current state of all sixteen keys.

```cpp
std::array<bool, 16> m_keys {};
```

### Keyboard Instructions

- `EX9E`: Skip the next instruction if the key represented by Vx is pressed.
- `EXA1`: Skip the next instruction if the key represented by Vx is not pressed.
- `FX0A`: Wait for a key press and store its value in Vx.

The `FX0A` instruction must suspend instruction progression until a key is pressed.

---

# Display and Graphics

## Display Specifications

The original CHIP-8 display has the following characteristics:

| Property | Value |
|---|---:|
| Width | 64 pixels |
| Height | 32 pixels |
| Color depth | Monochrome |
| Pixel states | On / Off |
| Framebuffer size | 2048 bits |
| Framebuffer storage | 256 bytes |

The screen coordinate system uses the upper-left corner as its origin.

```text
(0,0)                         (63,0)
  +--------------------------------+
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  |                                |
  +--------------------------------+
(0,31)                       (63,31)
```

Each pixel represents a single bit:

- `0`: Pixel off.
- `1`: Pixel on.

## Framebuffer Representation

A straightforward representation uses one byte per pixel:

```cpp
std::array<Byte, WIDTH * HEIGHT> framebuffer {};
```

This is simple to manipulate but requires 2048 bytes.

A packed representation uses one bit per pixel:

```cpp
std::array<Byte, BUFFER_SIZE> framebuffer {};
```

Where:

```cpp
BUFFER_SIZE = (WIDTH * HEIGHT) / 8;
```

This requires exactly 256 bytes.

### Pixel Indexing

For a row-major packed framebuffer, the pixel at `(x, y)` is stored using:

```cpp
const std::size_t pixel_index = (y * WIDTH) + x;
const std::size_t byte_index  = pixel_index / 8;
const std::size_t bit_index   = 7 - (pixel_index % 8);
```

The most-significant bit represents the leftmost pixel in each group of eight pixels.

Reading a pixel:

```cpp
const bool pixel_on =
    (framebuffer[byte_index] & (1U << bit_index)) != 0;
```

Writing a pixel:

```cpp
framebuffer[byte_index] |= static_cast<Byte>(1U << bit_index);
```

Clearing a pixel:

```cpp
framebuffer[byte_index] &= static_cast<Byte>(~(1U << bit_index));
```

Toggling a pixel:

```cpp
framebuffer[byte_index] ^= static_cast<Byte>(1U << bit_index);
```

The framebuffer representation is an implementation detail. The renderer should consume the 
framebuffer without depending on the CPU's internal representation.

---

# Fontset and Sprites

## Built-in Fontset

CHIP-8 includes a built-in hexadecimal fontset containing sixteen sprites:

```text
0 1 2 3 4 5 6 7 8 9 A B C D E F
```

Each character sprite is:

- 4 pixels wide.
- 5 pixels tall.
- 5 bytes in memory.
- 40 bits of pixel information, of which 20 bits are used.

Each byte represents one horizontal row of the sprite. The upper four bits contain the active 
pixels, and the lower four bits are padding.

The complete fontset requires:

```cpp
16 * 5 = 80 bytes;
```

### Example: Character F

```text
1111 0000  -> 0xF0
1000 0000  -> 0x80
1111 0000  -> 0xF0
1000 0000  -> 0x80
1000 0000  -> 0x80
```

The corresponding byte sequence is:

```cpp
{ 0xF0, 0x80, 0xF0, 0x80, 0x80 }
```

The lower nibble is padding and does not represent active pixels.

### Font Sprite Addressing

The `FX29` instruction sets I to the location of the hexadecimal sprite corresponding to Vx.

For a five-byte font sprite:

```cpp
I = FONTSET_BEGIN + (Vx * 5);
```

The emulator must ensure that Vx represents a valid hexadecimal digit.

## Compressed Fontset Representation

The fontset can be stored in a compressed representation by removing the unused lower nibble of 
every row.

Each sprite contains five 4-bit values, requiring 20 bits.

For example:

```text
Character F:

F 8 F 8 8

Compressed representation:

0xF8F88
```

A 32-bit integer can hold the compressed representation.

```cpp
static constexpr std::array<std::uint32_t, 16> COMPRESSED_FONTS =
{
    0xF999F, 0x26227, 0xF1F8F, 0xF1F1F,
    0x99F11, 0xF8F1F, 0xF8F9F, 0xF1244,
    0xF9F9F, 0xF9F1F, 0xF9F99, 0xE9E9E,
    0xF888F, 0xE999E, 0xF8F8F, 0xF8F88
};
```

### Decompression

The compressed representation can be expanded into the conventional five-byte sprite format.

The most-significant nibble is decoded first.

```cpp
void decompress_fonts()
{
    auto* target = fontset.data();

    for (const auto sprite : COMPRESSED_FONTS)
    {
        for (int shift = 16; shift >= 0; shift -= 4)
        {
            const auto nibble = (sprite >> shift) & 0x0F;
            *target++ = static_cast<Byte>(nibble << 4);
        }
    }
}
```

Each 4-bit value is shifted into the upper nibble of its corresponding output byte.

This representation reduces the fontset definition size, although the decompressed runtime 
representation still occupies 80 bytes.

The fontset should be initialized in the reserved interpreter memory area, starting at `0x0000`.

## Custom Sprites

CHIP-8 supports sprites up to 8 pixels wide and 15 pixels tall.

Sprite data is stored in memory, with each byte representing one row.

The `DXYN` instruction draws a sprite containing N rows, starting at memory address I.

---

# Timers

CHIP-8 provides two 8-bit timers:

| Timer | Description |
|---|---|
| DT | Delay timer |
| ST | Sound timer |

Both timers decrement at a frequency of 60 Hz.

The timers stop decrementing when they reach zero.

## Delay Timer (DT)

The delay timer is commonly used to control timing and pauses in CHIP-8 programs.

- Set using `FX15`.
- Read using `FX07`.
- Decrements once every 1/60 second.
- Stops at zero.

## Sound Timer (ST)

The sound timer controls the duration of the sound signal.

- Set using `FX18`.
- Decrements once every 1/60 second.
- Produces sound while its value is greater than zero.
- Stops producing sound when it reaches zero.

The original CHIP-8 sound output is a simple tone. Its actual frequency is implementation-dependent.

## Timer Update

Timer updates should be driven by elapsed time rather than by the number of CPU instructions 
executed. This keeps timer behavior independent of the configurable CPU execution frequency.

---

# Instruction Set

The original CHIP-8 instruction set contains 35 commonly documented instructions, depending on 
how the `0NNN` SYS instruction is counted.

All standard CHIP-8 instructions are 16 bits (2 bytes) long.

Instructions are stored in big-endian order:

```text
Memory[address]     = Opcode high byte
Memory[address + 1] = Opcode low byte
```

For example:

```text
Opcode: 0xA123

Memory:
Address      Value
0x0200       0xA1
0x0201       0x23
```

Instructions are conventionally located at even addresses.

## Opcode Variables

The following symbols are used to describe instruction operands:

| Symbol | Width | Description |
|---|---:|---|
| `nnn` | 12 bits | Memory address. |
| `n` | 4 bits | Lowest nibble of the instruction. |
| `x` | 4 bits | Register index selected from V0–VF. |
| `y` | 4 bits | Register index selected from V0–VF. |
| `kk` | 8 bits | Immediate byte value. |

## Opcode Bit Layout

```text
15             12 11              8 7               4 3               0
+----------------+-----------------+-----------------+-----------------+
|      Type      |        x        |        y        |        n        |
+----------------+-----------------+-----------------+-----------------+

For instructions using kk:

+----------------+-----------------+-----------------------------------+
|      Type      |        x        |                kk                 |
+----------------+-----------------+-----------------------------------+
```

## Operand Extraction

The following masks extract the instruction operands:

| Operand | Mask | Extraction |
|---|---|---|
| `nnn` | `0x0FFF` | `opcode & 0x0FFF` |
| `n` | `0x000F` | `opcode & 0x000F` |
| `x` | `0x0F00` | `(opcode >> 8) & 0x0F` |
| `y` | `0x00F0` | `(opcode >> 4) & 0x0F` |
| `kk` | `0x00FF` | `opcode & 0x00FF` |

Example:

```cpp
const Byte x  = (opcode >> 8) & 0x0F;
const Byte y  = (opcode >> 4) & 0x0F;
const Byte n  = opcode & 0x0F;
const Byte kk = opcode & 0x00FF;

const Word nnn = opcode & 0x0FFF;
```

---

# Opcode Table

The following table describes the original CHIP-8 instruction set.

| Opcode | Mnemonic | Operation |
|---|---|---|
| `0NNN` | `SYS NNN` | Call RCA 1802 routine at NNN. Usually ignored by modern interpreters. |
| `00E0` | `CLS` | Clear the display. |
| `00EE` | `RET` | Return from subroutine. |
| `1NNN` | `JP NNN` | Set PC to NNN. |
| `2NNN` | `CALL NNN` | Push return address and jump to NNN. |
| `3XKK` | `SE VX, KK` | Skip next instruction if VX equals KK. |
| `4XKK` | `SNE VX, KK` | Skip next instruction if VX differs from KK. |
| `5XY0` | `SE VX, VY` | Skip next instruction if VX equals VY. |
| `6XKK` | `LD VX, KK` | Set VX to KK. |
| `7XKK` | `ADD VX, KK` | Add KK to VX, keeping the low 8 bits. |
| `8XY0` | `LD VX, VY` | Set VX to VY. |
| `8XY1` | `OR VX, VY` | Set VX to VX OR VY. |
| `8XY2` | `AND VX, VY` | Set VX to VX AND VY. |
| `8XY3` | `XOR VX, VY` | Set VX to VX XOR VY. |
| `8XY4` | `ADD VX, VY` | Add VY to VX; VF receives carry. |
| `8XY5` | `SUB VX, VY` | Subtract VY from VX; VF indicates no borrow. |
| `8XY6` | `SHR VX, VY` | Shift right; VF receives the least-significant bit. |
| `8XY7` | `SUBN VX, VY` | Set VX to VY minus VX; VF indicates no borrow. |
| `8XYE` | `SHL VX, VY` | Shift left; VF receives the most-significant bit. |
| `9XY0` | `SNE VX, VY` | Skip next instruction if VX differs from VY. |
| `ANNN` | `LD I, NNN` | Set I to NNN. |
| `BNNN` | `JP V0, NNN` | Jump to NNN plus V0. |
| `CXKK` | `RND VX, KK` | Set VX to random byte AND KK. |
| `DXYN` | `DRW VX, VY, N` | Draw N-byte sprite at coordinates VX, VY. VF indicates collision. |
| `EX9E` | `SKP VX` | Skip next instruction if key VX is pressed. |
| `EXA1` | `SKNP VX` | Skip next instruction if key VX is not pressed. |
| `FX07` | `LD VX, DT` | Load delay timer into VX. |
| `FX0A` | `LD VX, K` | Wait for a key press and store its value in VX. |
| `FX15` | `LD DT, VX` | Set delay timer to VX. |
| `FX18` | `LD ST, VX` | Set sound timer to VX. |
| `FX1E` | `ADD I, VX` | Add VX to I. |
| `FX29` | `LD F, VX` | Set I to the font sprite address for VX. |
| `FX33` | `LD B, VX` | Store VX as three BCD digits at I, I+1, I+2. |
| `FX55` | `LD [I], VX` | Store V0 through VX in memory. |
| `FX65` | `LD VX, [I]` | Load memory values into V0 through VX. |

## Arithmetic and Flag Behavior

The `VF` register is used as a flag by arithmetic and graphics instructions.

| Instruction | VF behavior |
|---|---|
| `8XY4` | Set to 1 if addition produces a carry; otherwise 0. |
| `8XY5` | Set to 1 if VX is greater than or equal to VY; otherwise 0. |
| `8XY6` | Receives the least-significant bit of the value before shifting. |
| `8XY7` | Set to 1 if VY is greater than or equal to VX; otherwise 0. |
| `8XYE` | Receives the most-significant bit of the value before shifting. |
| `DXYN` | Set to 1 if drawing causes a pixel collision; otherwise 0. |

The exact behavior of some arithmetic and shift instructions varies between CHIP-8 interpreter 
implementations and historical compatibility profiles.

For example, shift instructions may operate on VX or VY depending on the selected compatibility
behavior.

The emulator should document its chosen behavior.

---

# Implementation Considerations

## Instruction Fetch

A CHIP-8 instruction is fetched as two consecutive bytes in big-endian order.

Conceptually:

```cpp
opcode = (memory[pc] << 8) | memory[pc + 1];
pc += 2;
```

The CPU should validate that both bytes are accessible before fetching.

## Instruction Execution

A typical CPU execution cycle consists of:

1. Fetch the next instruction.
2. Decode its opcode and operands.
3. Execute the instruction.
4. Update CPU state.
5. Update timers independently at 60 Hz.

The CPU should keep instruction execution separate from display rendering and frontend 
input processing.

## Memory Access

Memory access operations should validate address boundaries.

For a multi-byte operation, the entire requested range must fit within the memory 
address space.

Alignment validation, if enforced by the emulator, should be handled separately from 
bounds validation.

CHIP-8 is byte-addressable; an even-address requirement for word accesses is an
implementation policy rather than a universal requirement of the instruction set.

## Stack Safety

The CPU must prevent stack overflow and underflow.

- A `CALL` must not exceed the maximum stack depth.
- A `RET` must not execute when the stack is empty.

## Graphics Behavior

The `DXYN` instruction draws sprites using XOR semantics.

- A sprite bit set to 1 toggles the corresponding screen pixel.
- A sprite bit set to 0 leaves the pixel unchanged.
- VF is set when an enabled pixel is turned off due to collision.

Sprite drawing must also implement the selected screen-wrapping behavior.

## Compatibility

Some CHIP-8 behaviors differ between interpreters and historical implementations.

Examples include:

- Shift instruction source register.
- Whether `FX55` and `FX65` increment I.
- Whether `BNNN` uses V0 or another register in extended variants.
- Sprite wrapping at display boundaries.
- `FX0A` key-release behavior.

These should be explicit emulator configuration decisions rather than accidental side effects of 
implementation.

---

# References

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8)
- [CHIP-8 Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite)
- [Octo CHIP-8 Documentation](https://github.com/JohnEarnest/Octo)

