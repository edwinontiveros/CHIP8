#pragma once

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H 1

#include "chip8_macros.h"

#include <random>

#include "constants/cpu_constants.h"
#include "constants/memory_constants.h"
#include "core/memory/memory.h"
#include "types/types.h"

namespace chip8::cpu
{
    namespace types            = chip8::types;
    namespace cpu_constants    = chip8::constants::cpu;
    namespace memory_constants = chip8::constants::memory;

    using Stack     = types::Stack;
    using Registers = types::Storage<types::Byte, cpu_constants::registers::V_REGISTERS>;
    using Memory    = chip8::memory::Memory;

    enum class Operand : types::Word
    {
        NNN = 0x0FFF,
        X   = 0x0F00,
        Y   = 0x00F0,
        N   = 0x000F,
        KK  = 0x00FF,
    };

    class Cpu final
    {
        Memory&   m_memory;
        Stack     m_stack {};
        Registers m_v {0};

        types::Byte    m_stkp {memory_constants::CLEAR_BYTE};
        types::Address m_i {memory_constants::CLEAR_BYTE};
        types::Address m_pc {memory_constants::ROM_BEGIN};
        types::Byte    m_dt {memory_constants::CLEAR_BYTE};
        types::Byte    m_st {memory_constants::CLEAR_BYTE};

        types::Word m_opcode {0x0000};

        types::Word get_operand(Operand op) const noexcept;
        types::Byte get_register(std::size_t index) const;
        void        set_register(std::size_t index, types::Byte value);

        std::mt19937                               rng {std::random_device {}()};
        std::uniform_int_distribution<types::Word> rand_byte {0, 0xFF};

        void fetch();
        void decode_and_execute();

        /// Unknown or malformed opcode handler.
        CHIP8_NORETURN void invalid_opcode();

    public:
        explicit Cpu(Memory& m) : m_memory(m) {}

        void tick();

        void clock();
        void tick_sound_timer();
        void tick_delay_timer();
        void reset();

        /// 0NNN (SYS NNN): call RCA 1802 routine. Unused.
        void SYS();
        /// 00E0 (CLS): clear the display.
        void CLS();

        /// 00EE (RET) return from subroutine.
        void RET();
        /// 1NNN (JP NNN): Jump unconditionally to address NNN.
        void JP();
        /// 2NNN (CALL NNN): Call subroutine at NNN.
        void CALL();

        /// 3XKK (SE VX, KK): Skip next instruction if V[X] == KK.
        void SE();
        /// 4XKK (SNE VX, KK): Skip next instructio if V[X] != KK.
        void SNE();
        /// 5XY0 (SE VX, VY): Skip next instruction if V[X] == V[Y].
        void SEV();

        /// 6XKK (LD VX, KK): Load KK into register V[X].
        void LD();
        /// 7XKK (ADD VX, KK): Add KK to the value of V[X]. Keep the low 8-bits of the result.
        void ADD();
        /// 8XY0 (LD VX, VY): Load value of register V[Y] into V[X].
        void LDV();

        /// 8XY1 (OR VX, VY): Set V[X] to the result of V[X] OR V[Y].
        void OR();
        /// 8XY2 (AND VX, VY): Set V[X] to the result of V[X] AND V[Y].
        void AND();
        /// 8XY3 (XOR VX, VY): Set V[X] to the result of V[X] XOR V[Y].
        void XOR();

        /// 8XY4 (ADD VX, VY): Set V[X] to the result of V[X] + V[Y]. V[F] recieves carry.
        void ADDC();
        /// 8XY5 (SUB VX, VY): Set V[X] to the result of V[X] - V[Y]. V[F] indicates no-borrow.
        void SUB();

        /// 8XY6 (SHR VX {, VY}): Bit-shift V[X] right. V[F] gets the MSB. V[Y] is optional,
        /// It indicates how many bits to shift.
        void SHR();
        /// 8XY7 (SUBN VX, VY): Set V[X] to the result of V[Y] - V[X]. V[F] indicates no-borrow.
        void SUBN();
        /// 8XYE (SHL VX {, VY}): Bit-shift V[X] left. V[F] gets the LSB. V[Y] is optional,
        /// It indicates how many bits to shift.
        void SHL();

        /// 9XY0 (SNE VX, VY): Skip next instruction if V[X] != V[Y].
        void SNEV();
        /// ANNN (LD I, NNN): Load the address NNN into the register I.
        void LDI();
        /// BNNN (JP V0, NNN): Jump unconditionally to address NNN + offset V[0].
        void JPO();

        /// CXKK (RND VX, KK): Set V[X] to the result of a random byte AND KK.
        void RND();
        /// DXYN (DRW VY, VY, N): Draw N-byte sprite at coords (V[X], V[Y]). V[F] indicates
        /// collision.
        void DRW();

        /// EX9E (SKP VX): Skip next instruction if key code V[X] is pressed.
        void SKP();
        /// EXA1 (SKP VX): Skip next instruction if key code V[X] is not pressed.
        void SKNP();

        /// FX07 (LD VX, DT): Load the value of the timer DT into V[X].
        void LDT();
        /// FX0A (LD VX {, K}): Wait for key press and store in V[X]. K is unused.
        void LDK();
        /// FX15 (LD DT, VX): Set delay timer to V[X].
        void LDTV();
        /// FX18 (LD ST, VX): Set sound timer to V[X].
        void LDS();

        /// FX1E (ADD I, VX): Add offset V[X] to address stored in I.
        void ADDI();

        /// FX29 (LD F, VX): Set I to the font sprite offset at V[X].
        void LDF();
        /// FX33 (LD B, VX): Store V[X] as three BCD digits at address I, I + 1, I + 2.
        void LDBCD();
        /// FX55 (LD [I], VX): Store V[0] ... V[X] begining at address in I.
        void LDIV();
        /// FX65 (LD VX, [I]): Load into V[0] ... V[X] memory values begining at address in I.
        void LDVI();
    };

}  // namespace chip8::cpu

#endif  // !CHIP8_CPU_H

