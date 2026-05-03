#include <iostream>
#include <cassert>
#include <stdint.h>
#include <ctype.h>


#ifndef CPU_HPP
#define CPU_HPP

// 8 bits
using Byte = uint8_t;
// 16 bits
using Word = uint16_t;
using uint = uint32_t;

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


    // https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register
    // https://stackoverflow.com/questions/57958631/game-boy-half-carry-flag-and-16-bit-instructions-especially-opcode-0xe8
    union {
        struct {
            Byte F; // flags register, only 4 bits used (Z N H C)
            Byte A; // accumulator
        };
        Word AF; // AF = A << 8 | F
    };

    union {
        struct {
            Byte C;
            Byte B;
        };
        Word BC; // BC = B << 8 | C
    };

    union {
        struct {
            Byte E;
            Byte D;
        };
        Word DE; // DE = D << 8 | E
    };

    union {
        struct {
            Byte L;
            Byte H;
        };
        Word HL; // HL = H << 8 | L
    };

    void reset(Mem &memory);
    Byte readByte(uint &cycles, Mem &memory, Word address);
    Word loadWord(uint &cycles, Mem &memory);
    Byte loadByte(uint &cycles, Mem &memory);
    void loadRegToReg(Byte &dest, Byte &src);
    Word incWord(Word value); // increment a 16-bit word, wrapping around at 0xFFFF
    void loadToMemory(uint &cycles, Mem &memory, Word address, Byte &reg);
    void loadFromMemory(uint &cycles, Mem &memory, Word address, Byte &reg);
    Byte fetchInstruction(uint &cycles, Mem &memory);
    void executeInstruction(uint cycles, Mem &memory);
    void showAllRegisterValues();

    // Opcodes
    
    // x0 opcodes
    static constexpr Byte NOP = 0x00; // No operation
    static constexpr Byte STOP = 0x01; // stops clock and oscillator
    static constexpr Byte JP_NZ_s8 = 0x20; // Jump to relative address if Z flag is not set
    static constexpr Byte JR_NC_s8 = 0x30; // Jump to relative address if C flag is not set
    static constexpr Byte LD_B_B = 0x40; // Load B into B (essentially a NOP for B)
    static constexpr Byte LD_D_B = 0x50; // Load B into D
    static constexpr Byte LD_H_B = 0x60; // Load B into H
    static constexpr Byte LD_HLmem_B = 0x70; // Load B into (HL) memory
    static constexpr Byte ADD_A_B = 0x80; // Add B to A
    static constexpr Byte SUB_B = 0x90; // Subtract B from A
    static constexpr Byte AND_B = 0xA0; // Logical AND B with A
    static constexpr Byte OR_B = 0xB0; // Logical OR B with A
    static constexpr Byte RET_NZ = 0xC0; // Return if Z flag is not set
    static constexpr Byte RET_NC = 0xD0; // Return if C flag
    static constexpr Byte LD_a8mem_A = 0xE0; // Load A into (a8) memory
    static constexpr Byte LD_A_a8mem = 0xF0; // Load (a8) memory into A
    

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

    // x2 opcodes
    static constexpr Byte LD_BCmem_A = 0x02; // Load A into (BC) memory
    static constexpr Byte LD_DEmem_A = 0x12; // Load A into (DE) memory
    static constexpr Byte LD_HLi_A = 0x22; // Load A into (HL) memory and increment HL
    static constexpr Byte LD_HLd_A = 0x32; // Load A into (HL) memory and decrement HL
    static constexpr Byte LD_B_D = 0x42; // Load D into B
    static constexpr Byte LD_D_D = 0x52; // Load D into D
    static constexpr Byte LD_H_D = 0x62; // Load D into H
    static constexpr Byte LD_HLmem_D = 0x72; // Load D into (HL) memory
    static constexpr Byte ADD_A_D = 0x82; // Add D to A
    static constexpr Byte SUB_D = 0x92; // Subtract D from A
    static constexpr Byte AND_D = 0xA2; // Logical AND D with A
    static constexpr Byte OR_D = 0xB2; // Logical OR D with A
    static constexpr Byte JP_NZ_a16 = 0xC2; // Jump to absolute address if Z flag is not set
    static constexpr Byte JP_NC_a16 = 0xD2; // Jump to absolute address if C flag is not set
    static constexpr Byte LD_Cmem_A = 0xE2; // Load A into (C) memory
    static constexpr Byte LD_A_Cmem = 0xF2; // Load (C) memory into A

    // x3 opcodes
    static constexpr Byte INC_BC = 0x03; // Increment BC
    static constexpr Byte INC_DE = 0x13; // Increment DE
    static constexpr Byte INC_HL = 0x23; // Increment HL
    static constexpr Byte INC_SP = 0x33; // Increment SP
    static constexpr Byte LD_B_E = 0x43; // Load E into B
    static constexpr Byte LD_D_E = 0x53; // Load E into D
    static constexpr Byte LD_H_E = 0x63; // Load E into H
    static constexpr Byte LD_HLmem_E = 0x73; // Load E into (HL) memory
    static constexpr Byte ADD_A_E = 0x83; // Add E to A
    static constexpr Byte SUB_E = 0x93; // Subtract E from A
    static constexpr Byte AND_E = 0xA3; // Logical AND E with A
    static constexpr Byte OR_E = 0xB3; // Logical OR E with A
    static constexpr Byte JP_a16 = 0xC3; // Jump to
    static constexpr Byte DI = 0xF3; // Disable interrupts

    // xA opcodes
    static constexpr Byte LD_A_BCmem = 0x0A; // Load (BC) memory into A
    static constexpr Byte LD_A_DEmem = 0x1A; // Load (DE) memory into A

    // opcode cycles
    // https://meganesu.github.io/generate-gb-opcodes/
    static constexpr uint opcycles[256] = {
        1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, // 0x00-0x0F
        1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 0x10-0x1F
        3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 0x20-0x2F
        3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 0x30-0x3F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x40-0x4F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x50-0x5F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x60-0x6F
        2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, // 0x70-0x7F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x80-0x8F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x90-0x9F
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0xA0-0xAF
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0xB0-0xBF
        5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 0, 6, 6, 2, 4, // 0xC0-0xCF
        5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4, // 0xD0-0xDF
        3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4, // 0xE0-0xEF
        3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4  // 0xF0-0xFF
    };

};

#endif // CPU_HPP