#include <iostream>
#include <cassert>


#ifndef CPU_HPP
#define CPU_HPP

// 8 bits
using Byte = unsigned char;
// 16 bits
using Word = unsigned short;
using uint = unsigned int;

struct Mem
{
    // https://gbdev.io/pandocs/Memory_Map.html
    static constexpr uint size = 1024 * 64; // 64KB of memory on DMG $0000-$FFFF
    Byte Data[size];

    void init() {
        // Initialize memory to 0
        for (uint i = 0; i < size; ++i) {
            Data[i] = 0;
        }
    }

    // read 1 byte
    Byte operator[](uint address) const {
        assert (address < size);
        // Add bounds checking if necessary
        return Data[address];
    }

    // write 1 byte
    Byte& operator[](uint address) {
        assert (address < size);
        // Add bounds checking if necessary
        return Data[address];
    }
};


struct Cpu {

    Word PC; // program counter
    Word SP; // stack pointer

    Byte A; // accumulator
    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register
    Byte F; // flags register, only 4 bits used (Z N H C)

    // 8 bit halves of registers
    Byte B;
    Byte C;
    Byte D;
    Byte E;
    Byte H;
    Byte L;

    // 16 bit pairs
    Word AF; // AF = A << 8 | F
    Word BC; // BC = B << 8 | C
    Word DE; // DE = D << 8 | E
    Word HL; // HL = H << 8 | L

    void reset(Mem &memory);
    Byte fetchInstruction(uint &cycles, Mem &memory);
    void executeInstruction(uint cycles, Mem &memory);

    // Opcodes
    
    // x0 opcodes
    static constexpr Byte NOP = 0x00; // No operation
    static constexpr Byte STOP = 0x01; // stops clock and oscillator
    static constexpr Byte JP_NZ_s8 = 0x20; // Jump to relative address if Z flag is not set

    // x1 opcodes
    static constexpr Byte LD_BC_d16 = 0x01; // Load 16-bit immediate into BC
    static constexpr Byte LD_DE_d16 = 0x11; // Load 16-bit immediate into DE
    static constexpr Byte LD_HL_d16 = 0x21; // Load 16-bit immediate into HL
    static constexpr Byte LD_SP_d16 = 0x31; // Load 16-bit immediate into SP
    static constexpr Byte LD_B_C = 0x41; // Load C into B
    static constexpr Byte LD_D_C = 0x51; // Load C into D
    static constexpr Byte LD_H_C = 0x61; // Load C into H
    static constexpr Byte LD_HLmem_C = 0x71; // Load C into (HL) memory
    static constexpr Byte ADD_A_C = 0x81; // Add C to A
    static constexpr Byte SUB_C = 0x91; // Subtract C from A
    static constexpr Byte AND_C = 0xA1; // Logical AND C with A
    static constexpr Byte OR_C = 0xB1; // Logical OR C with A
    static constexpr Byte POP_BC = 0xC1; // Pop 16-bit value from stack into BC
    static constexpr Byte POP_DE = 0xD1; // Pop 16-bit value from stack into DE
    static constexpr Byte POP_HL = 0xE1; // Pop 16-bit value from stack into HL
    static constexpr Byte POP_AF = 0xF1; // Pop 16-bit value


};

#endif // CPU_HPP