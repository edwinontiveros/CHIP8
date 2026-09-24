#pragma once

#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H 1

#include <cstdint>
#include <array>

namespace chip8::types
{
    using Byte = std::uint8_t;
    using Word = std::uint16_t;
    using Address = std::uint16_t;

    template <class T>
    using C8Constant = inline constexpr T;

    namespace array_types
    {
        template <std::size_t size>
        using ByteArray = std::array<Byte, size>;

        template <std::size_t size>
        using WordArray = std::array<Word, size>;
    }

} // chip8::types

#endif // !CHIP8_TYPES_H
