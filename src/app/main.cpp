#include "pch.h"

#include <iostream>

#include "core/cpu/cpu.h"
#include "core/memory/memory.h"
#include "types/types.h"

int main() {
    std::cout << "Hello world Chip8" << '\n';

    chip8::memory::Memory memory {};
    chip8::cpu::Cpu       cpu {memory};

    memory.clear();
    memory.write<chip8::types::Word>(0x0200, 0x6012);
    memory.write<chip8::types::Word>(0x0202, 0x70FF);

    cpu.decode_and_execute();
    cpu.decode_and_execute();

    return 0;
}

