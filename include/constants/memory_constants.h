#pragma once

#ifndef CHIP8_MEMORY_CONSTANTS_H
#define CHIP8_MEMORY_CONSTANTS_H 1

#include <cstddef>  // VCIC-Excluded

#include "types/types.h"

namespace chip8::constants::memory
{
    using Address = chip8::types::Address;

    // special byte sequences
    inline constexpr chip8::types::Byte CLEAR_BYTE  = 0x00;
    inline constexpr chip8::types::Word UNUSED_BYTE = 0xDEAD;

    // major regions sizes
    inline constexpr std::size_t MEMORY_SIZE        = 1024 * 4;
    inline constexpr std::size_t INTERPRETER_SIZE   = 512;
    inline constexpr std::size_t PROGRAM_RAM_SIZE   = (MEMORY_SIZE - INTERPRETER_SIZE);
    inline constexpr std::size_t MAIN_REGIONS_TOTAL = INTERPRETER_SIZE + PROGRAM_RAM_SIZE;

    // subregion sizes
    inline constexpr std::size_t BUILTIN_FONTSET_REGION_SIZE = 80;
    inline constexpr std::size_t UNUSED_REGION_SIZE          = 432;
    inline constexpr std::size_t SUBREGIONS_TOTAL =
        (BUILTIN_FONTSET_REGION_SIZE + UNUSED_REGION_SIZE);

    // fixed address ranges
    inline constexpr Address MEMORY_BEGIN  = 0x0000;
    inline constexpr Address MEMORY_END    = 0x0FFF;
    inline constexpr Address FONTSET_BEGIN = 0x0000;
    inline constexpr Address FONTSET_END   = 0x004F;
    inline constexpr Address UNUSED_BEGIN  = 0x0050;
    inline constexpr Address UNUSED_END    = 0x01FF;
    inline constexpr Address ROM_BEGIN     = 0x0200;
    inline constexpr Address ROM_END       = 0x0FFF;

    // chip8 spec configuration assertions
    static_assert(MEMORY_SIZE == 4096, "Chip8 specifies the total memory to be 4Kb (4096 bytes)");

    static_assert(BUILTIN_FONTSET_REGION_SIZE == 80,
                  "Chip8 specifies the fonstet region to be 80 bytes");
    static_assert(INTERPRETER_SIZE == 512,
                  "Chip8 specifies reserved interpreter region to be 512 bytes");
    static_assert(PROGRAM_RAM_SIZE == 3584, "Chip8 specifies Program Data region to be 3584 bytes");

    static_assert(SUBREGIONS_TOTAL == INTERPRETER_SIZE,
                  "Interpreter sub-regions must be equal to total interpreter "
                  "region size 512 bytes.");

    static_assert(MAIN_REGIONS_TOTAL == MEMORY_SIZE,
                  "Interpreter and Program RAM, should be equal to total memory "
                  "size 4kb(4096)");

    // memory fixed address ranges assertions
    static_assert(MEMORY_BEGIN == 0x0000, "Chip8 specifies memory to begin at address 0x0000");
    static_assert(MEMORY_END == 0x0FFF, "Chip8 specifies memory to end at address 0x0FFF");

    static_assert(FONTSET_BEGIN == 0x0000, "Chip8 specifies fontset region to begin at 0x0000");
    static_assert(FONTSET_END == 0x004F, "Chip8 specifies fontset region to end at 0x004F");

    static_assert(UNUSED_BEGIN == 0x0050,
                  "Chip8 specifies unused interpreter region to begin at 0x0176");
    static_assert(UNUSED_END == 0x01FF,
                  "Chip8 specifies unused interpreter region to end at 0x01FF");

    static_assert(ROM_BEGIN == 0x0200, "Chip8 specifies RAM region to begin at 0x0200");
    static_assert(ROM_END == 0x0FFF, "Chip8 specifies RAM region to end at 0x0FFF");

}  // namespace chip8::constants::memory

#endif  // !CHIP8_MEMORY_CONSTANTS_H

