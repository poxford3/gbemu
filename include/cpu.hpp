#include <iostream>
#include <cassert>
#include <stdint.h>
#include <cstdint>
#include <ctype.h>
#include "types.hpp"


#ifndef CPU_HPP
#define CPU_HPP


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
        return Data[address];
    }

    // write 1 byte
    Byte& operator[](uint address) {
        assert (address < size);
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
    void updateFlags(Byte result, bool isSubtraction, bool halfCarry, bool carry);
    Byte readByte(Mem &memory, Word address);
    Word incWord(Word value); // increment a 16-bit word, wrapping around at 0xFFFF
    Word decWord(Word value); // decrement a 16-bit word, wrapping around at 0x0000
    void rotateLeft(Byte &value, std::optional<bool> throughCarry = std::nullopt);
    void rotateRight(Byte &value, std::optional<bool> throughCarry = std::nullopt);
    void jp(Word address);
    void jr(int8_t offset);
    void call(Word address, Mem &memory);
    void RET(Mem &memory);

    // load operations
    Word loadWord(Mem &memory);
    Byte loadByte(Mem &memory);
    void loadRegToReg(Byte &dest, Byte &src);
    void loadRegToReg(Word &dest, Word &src);
    void loadRegToMemory(Mem &memory, Word address, Byte &reg);
    void loadRegFromMemory(Mem &memory, Word address, Byte &reg);
    void RST(Word address, Mem &memory);

    // logical operations
    void andRegToA(Byte &src);
    void orRegToA(Byte &src);
    void xorRegToA(Byte &src);

    // math operations
    void addRegToReg(Byte &dest, Byte &src);
    void addRegToReg(Word &dest, Word &src);
    Word addByteToWord(Word &dest, Byte src);
    void subRegToReg(Byte &dest, Byte &src);
    void ADC(Byte src);
    void SBC(Byte src);
    void CP(Byte src); // essentially A - src, but only affects flags, does not store result
    void _DAA(); // Decimal Adjust Accumulator

    void popStackToReg(Word &reg, Mem &memory);
    void pushRegToStack(Word reg, Mem &memory);

    Byte fetchInstruction(uint &cycles, Mem &memory);
    void executeInstruction(uint cycles, Mem &memory);
    void showAllRegisterValues();

};

#endif // CPU_HPP