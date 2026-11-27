#pragma once

#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H 1

#include "chip8_macros.h"

#include <type_traits>

#include "constants/memory_constants.h"
#include "types/types.h"

namespace chip8::memory
{

    template<typename T>
    concept MemoryAccessType =
        std::is_same_v<T, chip8::types::Byte> || std::is_same_v<T, chip8::types::Word>;

    class Memory final
    {
    public:
        static constexpr std::size_t MEM_SIZE = chip8::constants::memory::MEMORY_SIZE;
        static constexpr std::size_t FONTSET_SIZE =
            chip8::constants::memory::BUILTIN_FONTSET_REGION_SIZE;
        static constexpr std::size_t CLEAR_BYTE = chip8::constants::memory::CLEAR_BYTE;

        using Byte        = chip8::types::Byte;
        using Address     = chip8::types::Address;
        using Word        = chip8::types::Word;
        using Region      = chip8::types::Region;
        using ConstRegion = chip8::types::ConstRegion;

        template<std::size_t size>
        using MemoryStorage = chip8::types::Storage<Byte, size>;

        enum class RegionType
        {
            Fontset,
            Unused,
            ProgramRam,
        };

    private:
        /// @brief Default fontset sprite table
        static constexpr MemoryStorage<FONTSET_SIZE> FONTSET = {
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
         * @brief Validates that a memory access falls within valid CHIP-8 address space.
         * @param address Starting of memory address.
         * @param size Number of bytes to access.
         */
        static void validate_address(Address address, std::size_t size);

        /**
         * @brief Helper function: Memory region factory function.
         * @tparam Storage_ Memory Storage type.
         * @tparam Region_ Memory Region selector.
         * @return ConstRegion or Region, depending on the caller constness.
         */
        template<RegionType Region_, class Storage_>
        CHIP8_NODISCARD static constexpr auto make_view(Storage_& storage) noexcept;

    public:
        /**
         * @brief Get a memory dump from 0x0000 to 0x0FFF
         * @return Span<> with current memory state.
         */
        CHIP8_NODISCARD ConstRegion dump() const noexcept;

        /// @brief Clear all memory bytes to 0x0000 (CLEAR_BYTE) and fontset region to defaults.
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

        /**
         * @brief Get a mutable view of a memory region.
         * @tparam Region_ Memory region to view.
         * @return A mutable Region type
         */
        template<RegionType Region_>
        CHIP8_NODISCARD constexpr Region view() noexcept;

        /**
         * @brief Get a read-only view of a memory region.
         * @tparam Region_ Memory region to view.
         * @return A mutable Region type
         */
        template<RegionType Region_>
        CHIP8_NODISCARD constexpr ConstRegion view() const noexcept;

    private:
        /// @brief Memory container, 4KB in size.
        MemoryStorage<MEM_SIZE> m_data {};
    };

    /**
     * @brief Template specialization to read a Byte from memory
     * @param address The memory address to read from
     * @return A Byte read from the address
     */
    template<>
    CHIP8_NODISCARD Memory::Byte Memory::read<Memory::Byte>(Address address) const;

    /**
     * @brief Template specialization to read a Word from memory
     * @param address The memory address to read from
     * @return A Word read from the address
     */
    template<>
    CHIP8_NODISCARD Memory::Word Memory::read<Memory::Word>(Address address) const;

    /**
     * @brief Template specialization to write a Byte value to memory
     * @param address The memory address to write on
     * @param value The Byte value to write
     */
    template<>
    void Memory::write<Memory::Byte>(Address address, Byte value);

    /**
     * @brief Template specialization to write a Word value to memory
     * @param address The memory address to write on
     * @param value The Word value to write
     */
    template<>
    void Memory::write<Memory::Word>(Address address, Word value);

}  // namespace chip8::memory

#endif  // !CHIP8_MEMORY_H
