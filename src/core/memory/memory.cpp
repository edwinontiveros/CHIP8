#include "pch.h"

#include "core/memory/memory.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "types/types.h"

namespace chip8::memory
{
    Memory::ConstRegion Memory::dump() const noexcept { return m_data; }

    void Memory::clear() noexcept {
        m_data.fill(CLEAR_BYTE);
        initialize_fontset();
    }

    void Memory::initialize_fontset() noexcept { std::ranges::copy(FONTSET, m_data.begin()); }

    void Memory::validate_address(Address address, std::size_t size) {
        const auto start          = static_cast<std::size_t>(address);
        const auto alignment_size = static_cast<std::size_t>(MEM_SIZE - start);

        if (start >= MEM_SIZE || size > alignment_size)
            throw std::out_of_range("CHIP-8 memory access out of range.");
    }

    /*
     *==========================
     * Template specializations
     *==========================
     */

    template<>
    Memory::Byte Memory::read<Memory::Byte>(Address address) const {
        validate_address(address, alignof(Byte));
        return m_data[address];
    }

    template<>
    void Memory::write<Memory::Byte>(Address address, Memory::Byte value) {
        validate_address(address, alignof(Byte));
        m_data[address] = value;
    }

    template<>
    Memory::Word Memory::read<Memory::Word>(Address address) const {
        validate_address(address, alignof(Word));

        const auto high = static_cast<Word>(read<Byte>(address));
        const auto low  = static_cast<Word>(read<Byte>(address + 1));

        return static_cast<Word>((high << 8) | low);
    }

    template<>
    void Memory::write<Memory::Word>(Address address, Word value) {
        validate_address(address, alignof(Word));

        const auto high = static_cast<Byte>(value >> 8);
        const auto low  = static_cast<Byte>(value & 0x00FF);

        write<Byte>(address, high);
        write<Byte>(address + 1, low);
    }

    /*
     *===========================================
     * Memory accessor templates
     *===========================================
     */

    template<Memory::RegionType Region_>
    CHIP8_NODISCARD constexpr Memory::Region Memory::view() noexcept {
        return make_view<Region_>(m_data);
    }

    template<Memory::RegionType Region_>
    CHIP8_NODISCARD constexpr Memory::ConstRegion Memory::view() const noexcept {
        return make_view<Region_>(std::as_const(m_data));
    }

}  // namespace chip8::memory
