#pragma once

#ifndef CHIP8_CPU_CONSTANTS_H
#define CHIP8_CPU_CONSTANTS_H 1

#include <cstddef>  // VCIC-Excluded

namespace chip8::constants::cpu
{
    namespace registers
    {
        inline constexpr std::size_t V_REGISTERS           = 16;
        inline constexpr std::size_t NUM_TIMERS            = 2;
        inline constexpr std::size_t STACK_SIZE            = 16;
        inline constexpr std::size_t NUM_SPECIAL_REGISTERS = 3;
        inline constexpr std::size_t INSTRUCTION_SIZE      = 2;

        // chip8 spec registers config assertions
        static_assert(V_REGISTERS == 16, "Chip8 specifies 16 general purpose V registers: 0-9 A-F");
        static_assert(NUM_TIMERS == 2, "Chip8 specifies 2 timer registers: DT and ST");
        static_assert(STACK_SIZE == 16, "Chip8 specifies a max stack size of 16 items");
        static_assert(NUM_SPECIAL_REGISTERS == 3,
                      "Chip8 specifies only 3 special registers: I, PC and SP");
    }  // namespace registers

    inline constexpr std::size_t CLOCK_FREQUENCY_HZ = 60;

    // clock spec assertions
    static_assert(CLOCK_FREQUENCY_HZ == 60, "Chip8 defines the clock frequency to be 60hz");

}  // namespace chip8::constants::cpu

#endif  // !CHIP8_CPU_CONSTANTS_H

