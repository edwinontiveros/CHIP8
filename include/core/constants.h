#pragma once

#ifndef CHIP8_CONSTANTS_H
#define CHIP8_CONSTANTS_H 1

#include <cstdint>

#include "types.h"

namespace chip8::constants
{
    namespace memory
    {
        using Address = chip8::types::Address;

        // special byte sequences
        inline constexpr chip8::types::Byte CLEAR_BYTE = 0x00;

        // major regions sizes
        inline constexpr std::size_t MEMORY_SIZE             = 1024 * 4;
        inline constexpr std::size_t INTERPRETER_REGION_SIZE = 512;
        inline constexpr std::size_t RAM_REGION_SIZE = (MEMORY_SIZE - INTERPRETER_REGION_SIZE);
        inline constexpr std::size_t REGIONS_TOTAL   = INTERPRETER_REGION_SIZE + RAM_REGION_SIZE;

        // subregion sizes
        inline constexpr std::size_t BUILTIN_FONTSET_SIZE  = 80;
        inline constexpr std::size_t INTERPRETER_DATA_SIZE = 294;
        inline constexpr std::size_t UNUSED_REGION_SIZE    = 138;
        inline constexpr std::size_t SUBREGIONS_TOTAL =
            (BUILTIN_FONTSET_SIZE + INTERPRETER_DATA_SIZE + UNUSED_REGION_SIZE);

        // fixed address ranges
        inline constexpr Address MEMORY_BEGIN      = 0x0000;
        inline constexpr Address MEMORY_END        = 0x0FFF;
        inline constexpr Address FONTSET_BEGIN     = 0x0000;
        inline constexpr Address FONTSET_END       = 0x004F;
        inline constexpr Address INTERPRETER_BEGIN = 0x0050;
        inline constexpr Address INTERPRETER_END   = 0x0175;
        inline constexpr Address UNUSED_BEGIN      = 0x0176;
        inline constexpr Address UNUSED_END        = 0x01FF;
        inline constexpr Address ROM_BEGIN         = 0x0200;
        inline constexpr Address ROM_END           = 0x0FFF;

        // chip8 spec configuration assertions
        static_assert(MEMORY_SIZE == 4096,
                      "Chip8 specifies the total memory to be 4Kb (4096 bytes)");

        static_assert(BUILTIN_FONTSET_SIZE == 80,
                      "Chip8 specifies the fonstet region to be 80 bytes");
        static_assert(INTERPRETER_REGION_SIZE == 512,
                      "Chip8 specifies reserved interpreter region to be 512 bytes");
        static_assert(RAM_REGION_SIZE == 3584,
                      "Chip8 specifies Program Data region to be 3584 bytes");

        static_assert(SUBREGIONS_TOTAL == INTERPRETER_REGION_SIZE,
                      "Interpreter sub-regions must be equal to total interpreter "
                      "region size 512 bytes.");
        static_assert(REGIONS_TOTAL == MEMORY_SIZE,
                      "Interpreter and Program RAM, should be equal to total memory "
                      "size 4kb(4096)");

        // memory fixed address ranges assertions
        static_assert(MEMORY_BEGIN == 0x0000, "Chip8 specifies memory to begin at address 0x0000");
        static_assert(MEMORY_END == 0x0FFF, "Chip8 specifies memory to end at address 0x0FFF");

        static_assert(FONTSET_BEGIN == 0x0000, "Chip8 specifies fontset region to begin at 0x0000");
        static_assert(FONTSET_END == 0x004F, "Chip8 specifies fontset region to end at 0x004F");

        static_assert(INTERPRETER_BEGIN == 0x0050,
                      "Chip8 specifies interpreter region to begin at 0x0050");
        static_assert(INTERPRETER_END == 0x0175,
                      "Chip8 specifies interpreter region to end at 0x0175");

        static_assert(UNUSED_BEGIN == 0x0176,
                      "Chip8 specifies unused interpreter region to begin at 0x0176");
        static_assert(UNUSED_END == 0x01FF,
                      "Chip8 specifies unused interpreter region to end at 0x01FF");

        static_assert(ROM_BEGIN == 0x0200, "Chip8 specifies RAM region to begin at 0x0200");
        static_assert(ROM_END == 0x0FFF, "Chip8 specifies RAM region to end at 0x0FFF");

    }  // namespace memory

    namespace display
    {
        inline constexpr std::size_t WIDTH              = 64;
        inline constexpr std::size_t HEIGHT             = 32;
        inline constexpr std::size_t BUFFER_SIZE        = (WIDTH * HEIGHT) / 8;
        inline constexpr std::size_t NUM_BUILTIN_FONTS  = 16;
        inline constexpr std::size_t FONT_PIXELS_WIDTH  = 8;
        inline constexpr std::size_t FONT_PIXELS_HEIGHT = 5;

        // chip8 spec display config assertions
        static_assert(WIDTH == 64, "Chip8 specifices a screen width of 64");
        static_assert(HEIGHT == 32, "Chip8 specifices a screen height of 32");
        static_assert(BUFFER_SIZE == 256,
                      "Chip8 specifies the total screen buffer to be packed as 256 bytes");
        static_assert(FONT_PIXELS_WIDTH == 8,
                      "Chip8 specifies the builtin font width to be 8 pixels");
        static_assert(FONT_PIXELS_HEIGHT == 5,
                      "Chip8 specifies the builtin font height to be 5 pixels");
        static_assert(NUM_BUILTIN_FONTS == 16,
                      "Chip8 specifies the builtin fonts to be 16: 0-9 A-F");

        static_assert((NUM_BUILTIN_FONTS * FONT_PIXELS_HEIGHT * FONT_PIXELS_WIDTH) / 8
                          == memory::BUILTIN_FONTSET_SIZE,
                      "Chip8 defines the builtin fontset region to be 80 bytes");
    }  // namespace display

    namespace input
    {
        inline constexpr std::size_t NUM_KEYS = 16;
        static_assert(NUM_KEYS == 16,
                      "Chip8 specifies the number of keyboard keys to be 16: 0-9 A-F.");
    }  // namespace input

    namespace registers
    {
        inline constexpr std::size_t V_SIZE            = 16;
        inline constexpr std::size_t TIMERS_SIZE       = 2;
        inline constexpr std::size_t STACK_SIZE        = 16;
        inline constexpr std::size_t SPECIAL_REGISTERS = 3;

        // chip8 spec registers config assertions
        static_assert(V_SIZE == 16, "Chip8 specifies 16 general purpose V registers: 0-9 A-F");
        static_assert(TIMERS_SIZE == 2, "Chip8 specifies 2 timer registers: DT and ST");
        static_assert(STACK_SIZE == 16, "Chip8 specifies a max stack size of 16 items");
        static_assert(SPECIAL_REGISTERS == 3,
                      "Chip8 specifies only 3 special registers: I, PC and SP");
    }  // namespace registers

    namespace cpu
    {
        inline constexpr std::size_t CLOCK_FREQUENCY = 60;

        // clock spec assertions
        static_assert(CLOCK_FREQUENCY == 60, "Chip8 defines the clock frequency to be 60hz");
    }  // namespace cpu

}  // namespace chip8::constants

#endif  // !CHIP8_CONSTANTS_H
