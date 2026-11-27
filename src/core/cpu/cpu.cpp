#include "pch.h"

#include "core/cpu/cpu.h"

#include <chrono>
#include <format>
#include <random>
#include <stdexcept>

#include "constants/cpu_constants.h"
#include "constants/memory_constants.h"
#include "core/memory/memory.h"
#include "types/types.h"

using namespace chip8::cpu;
using namespace chip8::memory;

void Cpu::fetch() {
    m_opcode = m_memory.read<types::Word>(m_pc);
    m_pc += cpu_constants::registers::INSTRUCTION_SIZE;
}

void Cpu::decode_and_execute() {
    switch (m_opcode & 0xF000) {
        case 0x0000: {
            const auto variant = m_opcode & 0x00FF;
            switch (variant) {
                case 0x00E0: {
                    CLS();
                    return;
                }
                case 0x00EE: {
                    RET();
                    return;
                }
                default: {
                    // SYS() falls here but it's not defined
                    XXXX();
                    return;
                }
            }
        }
        case 0x1000: {
            JP();
            return;
        }
        case 0x2000: {
            CALL();
            return;
        }
        case 0x3000: {
            SE();
            return;
        }
        case 0x4000: {
            SNE();
            return;
        }
        case 0x5000: {
            SEV();
            return;
        }
        case 0x6000: {
            LD();
            return;
        }
        case 0x7000: {
            ADD();
            return;
        }
        case 0x8000: {
            const auto variant = m_opcode & 0x000F;
            switch (variant) {
                case 0x0000: {
                    LDV();
                    return;
                }
                case 0x0001: {
                    OR();
                    return;
                }
                case 0x0002: {
                    AND();
                    return;
                }
                case 0x0003: {
                    XOR();
                    return;
                }
                case 0x0004: {
                    ADDC();
                    return;
                }
                case 0x0005: {
                    SUB();
                    return;
                }
                case 0x0006: {
                    SHR();
                    return;
                }
                case 0x0007: {
                    SUBN();
                    return;
                }
                case 0x000E: {
                    SHL();
                    return;
                }
                default: {
                    XXXX();
                    return;
                }
            }
        }
        case 0x9000: {
            SNEV();
            return;
        }
        case 0xA000: {
            LDI();
            return;
        }
        case 0xB000: {
            JPO();
            return;
        }
        case 0xC000: {
            RND();
            return;
        }
        case 0xD000: {
            DRW();
            return;
        }
        case 0xE000: {
            const auto variant = m_opcode & 0x00FF;
            switch (variant) {
                case 0x009E: {
                    SKP();
                    return;
                }
                case 0x00A1: {
                    SKNP();
                    return;
                }
                default: {
                    XXXX();
                    return;
                }
            }
        }
        case 0xF000: {
            const auto variant = m_opcode & 0x00FF;
            switch (variant) {
                case 0x0007: {
                    LDT();
                    return;
                }
                case 0x000A: {
                    LDK();
                    return;
                }
                case 0x0015: {
                    LDTV();
                    return;
                }
                case 0x0018: {
                    LDS();
                    return;
                }
                case 0x001E: {
                    ADDI();
                    return;
                }
                case 0x0029: {
                    LDF();
                    return;
                }
                case 0x0033: {
                    LDBCD();
                    return;
                }
                case 0x0055: {
                    LDIV();
                    return;
                }
                case 0x0065: {
                    LDVI();
                    return;
                }
                default: {
                    XXXX();
                    return;
                }
            }
        }
        default: {
            XXXX();
            return;
        }
    }
}

void Cpu::tick() {
    fetch();
    decode_and_execute();
    // tick_delay_timer();
    // tick_sound_timer();
    // clock();
    //  update clock
}

void Cpu::reset() {
    m_memory.clear();

    m_pc     = memory_constants::ROM_BEGIN;
    m_dt     = memory_constants::CLEAR_BYTE;
    m_st     = memory_constants::CLEAR_BYTE;
    m_stkp   = memory_constants::CLEAR_BYTE;
    m_i      = 0x0000;
    m_opcode = 0x0000;

    m_v.fill(memory_constants::CLEAR_BYTE);
    while (!m_stack.empty()) m_stack.pop();
}

void Cpu::set_register(std::size_t index, types::Byte value) {
    if (index >= cpu_constants::registers::V_REGISTERS)
        throw std::out_of_range(std::format("Register {:#x} out of bounds", index));

    m_v[index] = value;
}

types::Byte Cpu::get_register(std::size_t index) const {
    if (index >= cpu_constants::registers::V_REGISTERS)
        throw std::out_of_range(std::format("Register {:#x} out of bounds", index));

    return m_v[index];
}

types::Word Cpu::get_operand(Operand op) const noexcept {
    switch (op) {
        using enum Operand;
        case X : return (m_opcode & static_cast<types::Word>(op)) >> 8;
        case Y : return (m_opcode & static_cast<types::Word>(op)) >> 4;
        default: return (m_opcode & static_cast<types::Word>(op));
    }
}

void Cpu::SYS() { throw std::exception("SYS function not implemented"); }

void Cpu::CLS() { throw std::exception("CLS Function not implemented"); }

void Cpu::RET() {
    if (m_stkp < 1 || m_stack.size() < 1)
        throw std::underflow_error("STACK UNDERFLOW: Stack is empty.");

    const types::Address return_address = m_stack.top();
    m_stack.pop();
    m_stkp = static_cast<types::Byte>(m_stack.size());
    m_pc   = return_address;
}

void Cpu::JP() {
    const auto return_address = get_operand(Operand::NNN);

    if (return_address >= memory_constants::ROM_END)
        throw std::out_of_range("JP return address out of bounds.");

    m_pc = return_address;
}

void Cpu::CALL() {
    const types::Address jump_address   = get_operand(Operand::NNN);
    const types::Address return_address = m_pc;

    if (return_address >= memory_constants::ROM_END)
        throw std::out_of_range("CALL return address out of bounds.");

    if (jump_address >= memory_constants::ROM_END)
        throw std::out_of_range("CALL jump address out of bounds.");

    if (m_stack.size() >= cpu_constants::registers::STACK_SIZE)
        throw std::overflow_error("STACK OVERFLOW: Stack is full.");

    m_stack.push(return_address);
    m_pc   = jump_address;
    m_stkp = static_cast<types::Byte>(m_stack.size());
}

void Cpu::SE() {
    const auto value = static_cast<types::Byte>(get_operand(Operand::KK));
    const auto x     = static_cast<types::Byte>(get_operand(Operand::X));
    const auto v_x   = get_register(x);

    if (v_x == value) {
        const types::Address skip_to = m_pc + cpu_constants::registers::INSTRUCTION_SIZE;
        if (skip_to >= memory_constants::ROM_END) {
            throw std::out_of_range(std::format("SE skip to address out of bounds: {:#x}",
                                                skip_to));
        }
        m_pc = skip_to;
    }
}

void Cpu::SNE() {
    const auto value = static_cast<types::Byte>(get_operand(Operand::KK));
    const auto x     = static_cast<types::Byte>(get_operand(Operand::X));
    const auto v_x   = get_register(x);

    if (v_x != value) {
        const types::Address skip_to = m_pc + cpu_constants::registers::INSTRUCTION_SIZE;
        if (skip_to >= memory_constants::ROM_END) {
            throw std::out_of_range(std::format("SE skip to address out of bounds: {:#x}",
                                                skip_to));
        }
        m_pc = skip_to;
    }
}

void Cpu::SEV() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    if (v_x == v_y) {
        const types::Address skip_to = m_pc + cpu_constants::registers::INSTRUCTION_SIZE;
        if (skip_to >= memory_constants::ROM_END) {
            throw std::out_of_range(std::format("SE skip to address out of bounds: {:#x}",
                                                skip_to));
        }
        m_pc = skip_to;
    }
}

void Cpu::LD() {
    const auto value = static_cast<types::Byte>(get_operand(Operand::KK));
    const auto x     = static_cast<types::Byte>(get_operand(Operand::X));

    set_register(x, value);
}

void Cpu::ADD() {
    const auto value = static_cast<types::Byte>(get_operand(Operand::KK));
    const auto x     = static_cast<types::Byte>(get_operand(Operand::X));
    const auto v_x   = get_register(x);

    set_register(x, static_cast<types::Byte>((v_x + value) & 0x00FF));
}

void Cpu::LDV() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_y = get_register(y);

    set_register(x, v_y);
}

void Cpu::OR() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    set_register(x, v_x | v_y);
}

void Cpu::AND() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    set_register(x, v_x & v_y);
}

void Cpu::XOR() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    set_register(x, v_x ^ v_y);
}

void Cpu::ADDC() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    const types::Word sum = v_x + v_y;
    set_register(x, sum & 0xFF);
    set_register(0xF, sum > 0xFF ? 1 : 0);
}

void Cpu::SUB() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    const types::Byte not_borrow = v_x >= v_y ? 1 : 0;
    set_register(x, v_x - v_y);
    set_register(0xF, not_borrow);
}

void Cpu::SHR() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto v_x = get_register(x);

    const types::Byte lsb = v_x & 0x01;
    set_register(x, v_x >> 1);
    set_register(0x0F, lsb);
}

void Cpu::SUBN() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    const types::Byte not_borrow = v_y >= v_x ? 1 : 0;
    set_register(x, v_y - v_x);
    set_register(0xF, not_borrow);
}

void Cpu::SHL() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto v_x = get_register(x);

    const types::Byte msb = (v_x & (0x80)) >> 7;

    set_register(x, v_x << 1);
    set_register(0x0F, msb);
}

void Cpu::SNEV() {
    const auto x   = static_cast<types::Byte>(get_operand(Operand::X));
    const auto y   = static_cast<types::Byte>(get_operand(Operand::Y));
    const auto v_x = get_register(x);
    const auto v_y = get_register(y);

    if (v_x != v_y) {
        const types::Address skip_to = m_pc + cpu_constants::registers::INSTRUCTION_SIZE;
        if (skip_to >= memory_constants::ROM_END)
            throw std::out_of_range(std::format("SE skip to address out of bounds: {:#x}",
                                                skip_to));
        m_pc = skip_to;
    }
}

void Cpu::LDI() {
    const auto address = static_cast<types::Address>(get_operand(Operand::NNN));
    if (address >= memory_constants::ROM_END)
        throw std::out_of_range(std::format("LDI address out of bounds: {:#x}", address));
    m_i = address;
}

void Cpu::JPO() {
    const auto address = static_cast<types::Address>(get_operand(Operand::NNN));
    const auto v_0     = get_register(0x0);

    const types::Word target = address + v_0;
    if (target >= memory_constants::ROM_END)
        throw std::out_of_range(std::format("JPO target out of bounds: {:#x}", target));

    m_pc = target;
}

void Cpu::RND() {
    const auto x           = static_cast<types::Address>(get_operand(Operand::X));
    const auto kk          = static_cast<types::Address>(get_operand(Operand::KK));
    const auto random_byte = static_cast<types::Byte>(rand_byte(rng));

    const auto value = static_cast<types::Byte>(random_byte & kk);
    set_register(x, value);
}

void Cpu::DRW() { throw std::exception("DRW function not implemented"); }

void Cpu::SKP() { throw std::exception("SKP function not implemented"); }

void Cpu::SKNP() { throw std::exception("SKNP function not implemented"); }

void Cpu::LDT() {
    const auto x = static_cast<types::Address>(get_operand(Operand::X));

    set_register(x, m_dt);
}

void Cpu::LDK() { throw std::exception("LDK function not implemented"); }

void Cpu::LDTV() {
    const auto x = static_cast<types::Address>(get_operand(Operand::X));

    m_dt = get_register(x);
}

void Cpu::LDS() {
    const auto x = static_cast<types::Address>(get_operand(Operand::X));

    m_st = get_register(x);
}

void Cpu::ADDI() {
    const auto x   = static_cast<types::Address>(get_operand(Operand::X));
    const auto v_x = get_register(x);

    const types::Address target = m_i + v_x;
    if (target >= memory_constants::ROM_END)
        throw std::out_of_range(std::format("ADDI target address out of bounds: {:#x}", target));

    m_i = target;
}

void Cpu::LDF() { throw std::exception("LDF function not implemented"); }

void Cpu::LDBCD() { throw std::exception("LDBCD function not implemented"); }

void Cpu::LDIV() { throw std::exception("LDIV function not implemented"); }

void Cpu::LDVI() { throw std::exception("LDVI function not implemented"); }

void Cpu::XXXX() { throw std::runtime_error(std::format("Unknown opcode: {:#x}", m_opcode)); }

