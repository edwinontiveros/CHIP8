#pragma once

#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H 1

#include <span>

#include "constants.h"
#include "types.h"

namespace chip8::memory
{
    class Memory final
    {
     public:
        static constexpr std::size_t MEM_SIZE = chip8::constants::memory::MEMORY_SIZE;
        using Storage                         = chip8::types::Storage<MEM_SIZE>;
        using Byte                            = chip8::types::Byte;
        using Address                         = chip8::types::Address;
        using Word                            = chip8::types::Word;
        using Region                          = std::span<Byte>;
        using ConstRegion                     = std::span<const Byte>;
        using DumpRegion                      = std::span<const Byte, MEM_SIZE>;

    private:
        /// <summary>
        /// Default CHIP8 fonts sprite table.
        /// </summary>
        static constexpr chip8::types::Storage<chip8::constants::memory::BUILTIN_FONTSET_SIZE>
            FONTSET = {
                0x0F, 0x09, 0x09, 0x09, 0x0F,  // 0
                0x02, 0x06, 0x02, 0x02, 0x07,  // 1
                0x0F, 0x01, 0x0F, 0x08, 0x0F,  // 2
                0x0F, 0x01, 0x0F, 0x01, 0x0F,  // 3
                0x09, 0x09, 0x0F, 0x01, 0x01,  // 4
                0x0F, 0x08, 0x0F, 0x01, 0x0F,  // 5
                0x0F, 0x08, 0x0F, 0x09, 0x0F,  // 6
                0x0F, 0x01, 0x02, 0x04, 0x04,  // 7
                0x0F, 0x09, 0x0F, 0x09, 0x0F,  // 8
                0x0F, 0x09, 0x0F, 0x01, 0x0F,  // 9
                0x0F, 0x09, 0x0F, 0x09, 0x09,  // A
                0x0E, 0x09, 0x0E, 0x09, 0x0E,  // B
                0x0F, 0x08, 0x08, 0x08, 0x0F,  // C
                0x0E, 0x09, 0x09, 0x09, 0x0E,  // D
                0x0F, 0x08, 0x0F, 0x08, 0x0F,  // E
                0x0F, 0x08, 0x0F, 0x08, 0x08,  // F
        };

        /// @brief Initialize fontset memory region with builtin font sprites
        void initialize_fontset() noexcept;

        /**
         * @brief Validate memory address lies within Chip8 valid regions.
         * @param address Memory address to check.
         * @param alignment Validate memory address is aligned to this size
         */
        static void validate_address(Address address, std::size_t alignment);

     public:
        /**
         * @brief Get a memory dump from 0x0000 to 0x0FFF
         * @return Span<> with current memory state.
         */
        CHIP8_NODISCARD DumpRegion dump() const noexcept;

        /**
         * @brief
         */
        void clear() noexcept;

        /**
         * @brief Read from memory address.
         * @tparam T The size to read: Byte or Word
         * @param address  Address to read from
         * @return The value stored at the address
         */
        template<typename T> CHIP8_NODISCARD T read(Address address) const;

        /**
         * @brief Write a value to the provide memory address
         * @tparam T The size of the value to write: Byte or Word
         * @param address The address to write to
         * @param value The value to write
         */
        template<typename T> void write(Address address, T value);

        /*
         * ==========================
         *
         *   Memory region accessors
         *
         * ==========================
         */

     private:
        /**
         * @brief Memory container, 4KB in size.
         */
        Storage m_data {};
    };

    /**
     * @brief Template specialization to read a Byte from memory
     * @param address The memory address to read from
     * @return A Byte read from the address
     */
    template<>
    CHIP8_NODISCARD Memory::Byte Memory::read<Memory::Byte>(Memory::Address address) const;

    /**
     * @brief Template specialization to read a Word from memory
     * @param address The memory address to read from
     * @return A Word read from the address
     */
    template<>
    CHIP8_NODISCARD Memory::Word Memory::read<Memory::Word>(Memory::Address address) const;

    /**
     * @brief Template specialization to write a Byte value to memory
     * @param address The memory address to write on
     * @param value The Byte value to write
     */
    template<>
    void Memory::write<Memory::Byte>(Memory::Address address, Memory::Byte value);

    /**
     * @brief Template specialization to write a Word value to memory
     * @param address The memory address to write on
     * @param value The Word value to write
     */
    template<>
    void Memory::write<Memory::Word>(Memory::Address address, Memory::Word value);

}  // namespace chip8::memory

#endif  // !CHIP8_MEMORY_H
