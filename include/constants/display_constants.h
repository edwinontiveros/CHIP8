#pragma once

#ifndef CHIP8_DISPLAY_CONSTANTS_H
#define CHIP8_DISPLAY_CONSTANTS_H 1

#include <cstddef>  // VCIC-Excluded
#include "memory_constants.h"

namespace chip8::constants::display
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
    static_assert(FONT_PIXELS_WIDTH == 8, "Chip8 specifies the builtin font width to be 8 pixels");
    static_assert(FONT_PIXELS_HEIGHT == 5,
                  "Chip8 specifies the builtin font height to be 5 pixels");
    static_assert(NUM_BUILTIN_FONTS == 16, "Chip8 specifies the builtin fonts to be 16: 0-9 A-F");

    static_assert((NUM_BUILTIN_FONTS * FONT_PIXELS_HEIGHT * FONT_PIXELS_WIDTH) / 8
                      == memory::BUILTIN_FONTSET_REGION_SIZE,
                  "Chip8 defines the builtin fontset region to be 80 bytes");
}  // namespace chip8::constants::display

#endif  // !CHIP8_DISPLAY_CONSTANTS_H

