#include <iostream>
#include <cassert>
#include <stdint.h>
#include <cstdint>
#include <ctype.h>
#include "types.hpp"


#ifndef CPU_HPP
#define CPU_HPP


struct Mem {
    // https://gbdev.io/pandocs/Memory_Map.html
    static constexpr uint size = 1024 * 64; // 64KB of memory on DMG $0000-$FFFF
    Byte Data[size];

    void init() {
        // Initialize memory to 0
        for (uint i = 0; i < size; ++i) {
            Data[i] = 0;
        }
    }

    bool inCartRom0(Word address) {
        if (address >= 0x0000 && address <= 0x3FFF) return true;
        return false;
    }

    bool inCartRom1N(Word address) {
        // todo, figure out rom switching
        if (address >= 0x4000 && address <= 0x7FFF) return true;
        return false;
    }

    bool inVRAM(Word address) {
        if (address >= 0x8000 && address <= 0x9FFF) return true;
        return false;
    }

    bool inExRAM(Word address) {
        // external ram, if any
        if (address >= 0xA000 && address <= 0xBFFF) return true;
        return false;
    }

    bool inWorkRAM0(Word address) {
        if (address >= 0xC000 && address <= 0xCFFF) return true;
        return false;
    }

    bool inWorkRAM1(Word address) {
        if (address >= 0xD000 && address <= 0xDFFF) return true;
        return false;
    }

    bool inEchoRAM(Word address) {
        // mirror of 0xC000 - DDFF, not to be used
        if (address >= 0xE000 && address <= 0xFDFF) return true;
        return false;
    }

    bool inOAM(Word address) {
        if (address >= 0xFE00 && address <= 0xFE9F) return true;
        return false;
    }

    bool inNotUsable(Word address) {
        // not to be used
        if (address >= 0xFEA0 && address <= 0xFEFF) return true;
        return false;
    }

    bool inIORegisters(Word address) {
        if (address >= 0xFF80 && address <= 0xFF7F) return true;
        return false;
    }

    bool inHRAM(Word address) {
        // High RAM
        if (address >= 0xFF80 && address <= 0xFFFE) return true;
        return false;
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

    // I/O register locations

    bool IME = false;
    bool pendingIME = false;
    bool halted = false;
    bool paused = false;

    Word P1 = 0xFF00;
    Word SB = 0xFF01;
    Word SC = 0xFF02;
    Word DIV = 0xFF04;
    Word TIMA = 0xFF05;
    Word TMA = 0xFF06;
    Word TAC = 0xFF07;
    Word IF = 0xFF0F;
    Word NR10 = 0xFF10;
    Word NR11 = 0xFF11;
    Word NR12 = 0xFF12;
    Word NR13 = 0xFF13;
    Word NR14 = 0xFF14;
    Word NR21 = 0xFF16;
    Word NR22 = 0xFF17;
    Word NR23 = 0xFF18;
    Word NR24 = 0xFF19;
    Word NR30 = 0xFF1A;
    Word NR31 = 0xFF1B;
    Word NR32 = 0xFF1C;
    Word NR33 = 0xFF1D;
    Word NR34 = 0xFF1E;
    Word NR41 = 0xFF20;
    Word NR42 = 0xFF21;
    Word NR43 = 0xFF22;
    Word NR44 = 0xFF23;
    Word NR50 = 0xFF24;
    Word NR51 = 0xFF25;
    Word NR52 = 0xFF26;
    Word LCDC = 0xFF40;
    Word STAT = 0xFF41;
    Word SCY = 0xFF42;
    Word SCX = 0xFF43;
    Word LY = 0xFF44;
    Word LYC = 0xFF45;
    Word DMA = 0xFF46;
    Word BGP = 0xFF47;
    Word OBP0 = 0xFF48;
    Word OBP1 = 0xFF49;
    Word WY = 0xFF4A;
    Word WX = 0xFF4B;
    Word KEY0 = 0xFF4C;
    Word KEY1 = 0xFF4D;
    Word VBK = 0xFF4F;
    Word BANK = 0xFF50;
    Word HDMA1 = 0xFF51;
    Word HDMA2 = 0xFF52;
    Word HDMA3 = 0xFF53;
    Word HDMA4 = 0xFF54;
    Word HDMA5 = 0xFF55;
    Word RP = 0xFF56;
    Word BCPS = 0xFF68;
    Word BCPD = 0xFF69;
    Word OCPS = 0xFF6A;
    Word OCPD = 0xFF6B;
    Word SVBK = 0xFF70;
    Word IE = 0xFFFF;
    uint numBytes;

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
    void loadProgram(std::vector<Byte> program, uint numBytes, Mem &memory);
    void runProgram(Mem &memory);
    void updateFlags(Byte result, bool isSubtraction, bool halfCarry, bool carry);
    Byte readByte(Mem &memory, Word address);
    Word incWord(Word value); // increment a 16-bit word, wrapping around at 0xFFFF
    Byte incByte(Byte value); // increase a Byte's value and update the flags around it
    Word decWord(Word value); // decrement a 16-bit word, wrapping around at 0x0000
    Byte decByte(Byte value); // decrease a Byte's value and update the flags around it
    void jp(Word address);
    void jr(int8_t offset);
    void call(Word address, Mem &memory);
    void handleInterrupt(Mem &memory);
    void _RET(Mem &memory);
    void _EI();
    void _DI();

    // load operations
    Word loadWord(Mem &memory);
    Byte loadByte(Mem &memory);
    int8_t loadInt(Mem &memory);
    void loadRegToReg(Byte &dest, Byte &src);
    void loadRegToReg(Word &dest, Word &src);
    void loadRegToMemory(Mem &memory, Word address, Byte &reg);
    void loadRegFromMemory(Mem &memory, Word address, Byte &reg);
    void RST(Word address, Mem &memory);

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

    void popStackToReg(Word &reg, Mem &memory);
    void pushRegToStack(Word reg, Mem &memory);

    uint executeInstructions(Byte opcode, Mem &memory);
    uint executeExtendedOpcode(Byte opcode, Mem &memory);
    void showAllRegisterValues();
    void TEST_showAllRegValuesDecimal();

};

#endif // CPU_HPP