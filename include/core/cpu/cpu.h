#pragma once

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H 1

#include <cstddef>
#include <cstdio>

#include "constants/cpu_constants.h"
#include "constants/memory_constants.h"
#include "core/memory/memory.h"
#include "types/types.h"

namespace chip8::cpu
{
    class Cpu final
    {
        using Byte    = chip8::types::Byte;
        using Word    = chip8::types::Word;
        using Address = chip8::types::Address;
        using Stack   = chip8::types::Storage<Word, chip8::constants::cpu::registers::STACK_SIZE>;
        using Registers =
            chip8::types::Storage<Byte, chip8::constants::cpu::registers::V_REGISTERS>;
        using Memory = chip8::memory::Memory;

    public:
        explicit Cpu(Memory& memory) : m_memory(memory) {}

        inline auto fetch() {
            auto opcode = m_memory.read<Word>(m_pc);
            std::printf("Fetched instruction: 0x%04X, pc=0x%04x\n", opcode, m_pc);
            return opcode;
        };

        inline void decode_and_execute() {
            auto instruction = fetch();
            std::printf("Decoding instruction 0x%04X -> ", instruction);

            const auto opcode_group = instruction & 0xF000;

            switch (opcode_group) {
                case 0x0000: {
                    switch (instruction) {
                        case 0x00E0: {
                            std::printf("Decoded as (00E0) CLS (Clear screen).\n");
                            m_pc += 2;
                            return;
                        }
                        case 0x00EE: {
                            std::printf("Decoded as (00EE) RET (Return from subroutine).\n");
                            // pop stack item, set return address
                            m_pc = m_stk[m_stkp];

                            // set poped item to default clear, and decrement current stack pointer
                            m_stk[m_stkp--] = 0x0000;
                            return;
                        }
                        default: {
                            std::printf("Unsupported instruction 0x%04X.\n", instruction);
                            return;
                        }
                    }
                }

                case 0x1000: {
                    const Word nnn = instruction & 0x0FFF;
                    std::printf(
                        "Decoded as (1NNN) JP NNN (Set PC to NNN).\n"
                        "Effect: PC=0x%04X\n",
                        nnn
                    );
                    m_pc = nnn;
                    return;
                }

                case 0x2000: {
                    const Word nnn = instruction & 0x0FFF;
                    std::printf(
                        "Decoded as (2NNN) CALL NNN (Push return address and jump to NNN)\n"
                        "Effect: push current pc to stack, set PC=0x%04X\n",
                        nnn
                    );
                    m_stk[m_stkp++] = m_pc;
                    m_pc            = nnn;
                    return;
                }
                case 0x3000: {
                    const Byte x  = (instruction & 0x0F00) >> 8;
                    const Byte kk = instruction & 0x00FF;

                    bool skip_instruction = (m_v[x] == kk);

                    std::printf(
                        "Decoded as (3XKK) SE VX, KK (Skip next instruction if V[X] == KK)\n"
                        "Effect: set PC=PC+4 if V[x] == kk.\n"
                        "x=0x%02X, kk=0x%02X, V[x]->0x%02X, Skip next instruction? (%s)\n",
                        x, kk, m_v[x], (skip_instruction ? "true" : "false")
                    );
                    // skip next instruction
                    m_pc += skip_instruction ? 4 : 2;
                    if (skip_instruction) { std::printf("Skipped: PC=0x%04X\n", m_pc); }
                    return;
                }
                case 0x4000: {
                    const Byte x  = (instruction & 0x0F00) >> 8;
                    const Byte kk = (instruction & 0x00FF);

                    bool skip_instruction = (m_v[x] != kk);

                    std::printf(
                        "Decoded as (4XKK) SNE VX, KK (Skip next instruction if V[X] != KK)\n"
                        "Effect: set PC=PC+4 if V[x] != kk.\n"
                        "x=0x%02X, kk=0x%02X, V[x]->0x%02X, Skip next instruction? (%s)\n",
                        x, kk, m_v[x], (skip_instruction ? "true" : "false")
                    );
                    // skip next instruction
                    m_pc += skip_instruction ? 4 : 2;
                    if (skip_instruction) { std::printf("Skipped: PC=0x%04X\n", m_pc); }
                    return;
                }
                case 0x5000: {
                    const Byte x = (instruction & 0x0F00) >> 8;
                    const Byte y = (instruction & 0x00F0) >> 4;

                    bool skip_instruction = (m_v[x] == m_v[y]);

                    std::printf(
                        "Decoded as (5XY0) SE VX, VY (Skip next instruction if V[X] == V[Y])\n"
                        "Effect: set PC=PC+4 if V[x] == V[Y].\n"
                        "x=0x%02X, y=0x%02X, V[x]=0x%02X, V[y]->0x%02X, Skip next instruction? "
                        "(%s)\n",
                        x, y, m_v[x], m_v[y], (skip_instruction ? "true" : "false")
                    );
                    // skip next instruction
                    m_pc += skip_instruction ? 4 : 2;
                    if (skip_instruction) { std::printf("Skipped: PC=0x%04X\n", m_pc); }
                    return;
                }
                case 0x6000: {
                    const Byte x  = (instruction & 0x0F00) >> 8;
                    const Byte kk = (instruction & 0x00FF);

                    std::printf(
                        "Decoded as (6XKK) LD VX, KK (Set V[X] to KK)\n"
                        "Effect: V[x] = kk\n"
                        "x=0x%02X, kk=0x%02X, Old V[x]=0x%02X, ",
                        x, kk, m_v[x]
                    );

                    m_v[x] = kk;
                    m_pc += 2;

                    std::printf("New V[x]=0x%02X\n", m_v[x]);
                    return;
                }
                case 0x7000: {
                    const Byte x  = (instruction & 0x0F00) >> 8;
                    const Byte kk = (instruction & 0x00FF);

                    std::printf(
                        "Decoded as (7XKK) ADD VX, KK (Add KK to V[X], keep low 8-bits)\n"
                        "Effect: V[x] = ((V[x] + kk) & 0x00FF)\n"
                        "x=0x%02X, kk=0x%02X, Old V[x]=0x%02X, ",
                        x, kk, m_v[x]
                    );

                    m_v[x] = (m_v[x] + kk) & 0x00FF;
                    m_pc += 2;

                    std::printf("New V[x]=0x%02X\n", m_v[x]);
                    return;
                }
                case 0x8000: break;
                case 0x9000: break;
                case 0xA000: break;
                case 0xB000: break;
                case 0xC000: break;
                case 0xD000: break;
                case 0xE000: break;
                case 0xF000: break;
                default    : break;
            }
        };

    private:
        /// @brief Bus to internal RAM.
        Memory& m_memory;

        /// @brief Index register used for memory addressing (16-bit).
        Address m_i {0x0000};

        /// @brief Program counter (16-bit). Starts at 0x0200 (ROM_BEGIN).
        Address m_pc {chip8::constants::memory::ROM_BEGIN};

        /// @brief Stack Pointer. Initialized at 0.
        std::size_t m_stkp {0};

        /// @brief General purpose V registers (16 in total, 8-bit).
        Registers m_v {chip8::constants::memory::CLEAR_BYTE};

        /// @brief Stack. Store subroutine return addresses.
        Stack m_stk {0x0000};

        /// @brief Delay Timer. Decrements once every 1/60 second.
        Byte m_dt {chip8::constants::memory::CLEAR_BYTE};

        /// @brief Sound Timer. Decrements once every 1/60 second.
        Byte m_st {chip8::constants::memory::CLEAR_BYTE};
    };
}  // namespace chip8::cpu

#endif  // !CHIP8_CPU_H

