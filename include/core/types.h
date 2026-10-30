#pragma once

#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H 1

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>

#define CHIP8_NODISCARD [[nodiscard]]

namespace chip8::types
{
    using Byte    = std::uint8_t;
    using Word    = std::uint16_t;
    using DWord   = std::uint32_t;
    using Address = std::uint16_t;
    using Stack   = std::stack<Word>;

    template<std::size_t size>
    using Storage = std::array<Byte, size>;

}  // namespace chip8::types

#endif  // !CHIP8_TYPES_H
