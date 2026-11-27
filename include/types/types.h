#pragma once

#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H 1

#include <array>
#include <cstdint>
#include <span>
#include <stack>

namespace chip8::types
{
    using Byte        = std::uint8_t;
    using Word        = std::uint16_t;
    using DWord       = std::uint32_t;
    using Address     = std::uint16_t;
    using Stack       = std::stack<Word>;
    using Region      = std::span<Byte>;
    using ConstRegion = std::span<const Byte>;

    template<class T, std::size_t size>
    using Storage = std::array<T, size>;

}  // namespace chip8::types

#endif  // !CHIP8_TYPES_H
