#include "core/memory.h"

#include <iostream>
#include <stdexcept>

#include "core/constants.h"

namespace chip8::memory
{
    Memory::DumpRegion Memory::dump() const noexcept { return m_data; }

    void Memory::clear() noexcept {
        m_data.fill(chip8::constants::memory::CLEAR_BYTE);
        initialize_fontset();
    }

    void Memory::initialize_fontset() noexcept {
        const auto& mem = m_data.data();
        for (std::size_t i = 0; i < chip8::constants::memory::BUILTIN_FONTSET_SIZE; i++) {
            mem[i] = FONTSET.at(i);
        }
    }

    void Memory::validate_address(Address address, std::size_t alignment) {
        const auto start = static_cast<std::size_t>(address);

        if (start > MEM_SIZE || alignment > (MEM_SIZE - start)) {
            throw std::out_of_range("CHIP-8 memory access out of range");
        }
    }

    /*
     *===========================================
     *Template specializations for read and write
     *===========================================
     */

    template<>
    Memory::Byte Memory::read<Memory::Byte>(Memory::Address address) const {
        validate_address(address, sizeof Memory::Byte);
        return m_data[address];
    }

    template<>
    void Memory::write<Memory::Byte>(Memory::Address address, Memory::Byte value) {
        validate_address(address, sizeof Memory::Byte);
        m_data[address] = value;
    }

    template<>
    Memory::Word Memory::read<Memory::Word>(Memory::Address address) const {
        validate_address(address, sizeof Memory::Word);

        if (address % 2 != 0) {
            throw std::out_of_range("Partial byte read not allowed. Address must be aligned.");
        }

        const auto high = static_cast<Memory::Word>(Memory::read<Memory::Byte>(address));
        const auto low  = static_cast<Memory::Word>(Memory::read<Memory::Byte>(address + 1));
        return static_cast<Memory::Word>((high << 8) | low);
    }

    template<>
    void Memory::write<Memory::Word>(Memory::Address address, Memory::Word value) {
        validate_address(address, sizeof Memory::Word);

        if (address % 2 != 0) {
            throw std::out_of_range("Partial byte write not allowed. Address must be aligned.");
        }

        const auto high = static_cast<Memory::Byte>(value >> 8);
        Memory::write<Memory::Byte>(address, high);

        const auto low = static_cast<Memory::Byte>(value & 0x00FF);
        Memory::write<Memory::Byte>(address + 1, low);
    }

}  // namespace chip8::memory
