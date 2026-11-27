#include "pch.h"

#include <iostream>

#include "core/cpu/cpu.h"
#include "core/memory/memory.h"
#include "types/types.h"

int main() {
    using namespace chip8::cpu;
    using namespace chip8::memory;

    std::cout << "Hello world Chip8" << '\n';

    Memory memory {};
    Cpu    cpu {memory};

    /// Load into register V[0] the value FF
    memory.write<types::Word>(0x0200, 0x60FF);
    cpu.tick();

    /// Load into register V[1] the value A1
    memory.write<types::Word>(0x0202, 0x61A1);
    cpu.tick();

    /// V[0] XOR V[1]
    memory.write<types::Word>(0x0204, 0x8013);
    cpu.tick();

    // CALL subroutine at 0x0220: Generate random number
    memory.write<types::Word>(0x0206, 0x2220);

    // After return from subroutine, this is the return address. Shift left the random number
    memory.write<types::Word>(0x0208, 0x820E);

    // generate random number between 0x00 and 0x80, store in V[2]
    memory.write<types::Word>(0x0220, 0xC280);
    // add 3 to V[2]
    memory.write<types::Word>(0x0222, 0x7203);
    // return from subroutine
    memory.write<types::Word>(0x0224, 0x00EE);

    for (std::size_t i = 0; i < 5; i++) cpu.tick();
    

    cpu.reset();

    return 0;
}

