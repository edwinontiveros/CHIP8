#pragma once

#ifndef CHIP8_INPUT_CONSTANTS_H
#define CHIP8_INPUT_CONSTANTS_H 1

#include <cstddef> // VCIC-Excluded 

namespace chip8::constants::input
{
    inline constexpr std::size_t NUM_KEYS = 16;
    static_assert(NUM_KEYS == 16, "Chip8 specifies the number of keyboard keys to be 16: 0-9 A-F.");
}  // namespace chip8::constants::input

#endif  // !CHIP8_INPUT_CONSTANTS_H

