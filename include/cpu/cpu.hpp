#include <iostream>
#include <cassert>
#include <stdint.h>
#include <cstdint>
#include <ctype.h>
#include "utils/types.hpp"
#include "mmu.hpp"


#ifndef CPU_HPP
#define CPU_HPP


class Cpu {
    public:
    Word PC; // program counter
    Word SP; // stack pointer

    // I/O register locations

    bool IME = false;
    bool pendingIME = false;
    bool halted = false;
    bool paused = false;

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

    void reset();
    void updateFlags(Byte result, bool isSubtraction, bool halfCarry, bool carry);
    Word incWord(Word value); // increment a 16-bit word, wrapping around at 0xFFFF
    Byte incByte(Byte value); // increase a Byte's value and update the flags around it
    Word decWord(Word value); // decrement a 16-bit word, wrapping around at 0x0000
    Byte decByte(Byte value); // decrease a Byte's value and update the flags around it
    void jp(Word address);
    void jr(int8_t offset);
    void call(Word address, Mmu &memory);
    void _RET(Mmu &memory);
    void _EI();
    void _DI();

    // load operations
    Word loadWord(Mmu &memory);
    Byte loadByte(Mmu &memory);
    int8_t loadInt(Mmu &memory);
    void loadRegToReg(Byte &dest, Byte &src);
    void loadRegToReg(Word &dest, Word &src);
    void RST(Word address, Mmu &memory);

    // logical operations
    void andRegToA(Byte &src);
    void orRegToA(Byte &src);
    void xorRegToA(Byte &src);
    void _RRA(std::optional<bool> throughCarry = std::nullopt);
    void _RLA(std::optional<bool> throughCarry = std::nullopt);
    void rotateLeft(Byte &value, std::optional<bool> throughCarry = std::nullopt);
    void rotateRight(Byte &value, std::optional<bool> throughCarry = std::nullopt);
    void shiftLeft(Byte &value);
    void shiftRight(Byte &value, bool arithmetic);
    void swapNibbles(Byte &value);
    void bit(Byte &value, int bit);
    void res(Byte &value, int bit);
    void set(Byte &value, int bit);

    // math operations
    void addRegToReg(Byte &dest, Byte &src);
    void addRegToReg(Word &dest, Word &src);
    Word addByteToWord(Word &dest, int8_t src);
    void subRegToReg(Byte &dest, Byte &src);
    void ADC(Byte src);
    void SBC(Byte src);
    void CP(Byte src); // essentially A - src, but only affects flags, does not store result
    void _DAA(Byte &A); // Decimal Adjust Accumulator

    void popStackToReg(Word &reg, Mmu &memory);
    void pushRegToStack(Word reg, Mmu &memory);

    uint executeInstructions(Byte opcode, Mmu &memory);
    uint executeExtendedOpcode(Byte opcode, Mmu &memory);
    void showAllRegisterValues();
    void TEST_showAllRegValuesDecimal();

};

#endif // CPU_HPP