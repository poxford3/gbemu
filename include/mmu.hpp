#include "utils/types.hpp"
#include "utils/file.hpp"

#ifndef MMU_HPP
#define MMU_HPP

class Mmu {
    public:
        Mmu();

        Byte romBank0[0x4000]; // first 16KB of cart, start 0x0000
        Byte romBankN[0x4000]; // switchable 16KB bank, start 0x4000
        Byte VRam[0x2000]; // 8KB of VRAM, start 0x8000
        Byte workRamBank0[0x1000]; // 4KB of work RAM, start 0xC000
        Byte workRamBankN[0x1000]; // switchable 4KB bank, start 0xD000
        Byte oam[0xA0]; // Object Attribute Memory, start 0xFE00
        Byte HRam[0x7F]; // High RAM, start 0xFF80
        Byte ioRegisters[0x80]; // IO Regsiters, start 0xFF00
        Byte interruptEnableRegister; // IE Register, start 0xFFFF

        // io register addresses
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
        
        void loadRom(const std::vector<Byte>& program);
        void swapRomBank(Byte bank);
        void reset();
        void writeByte(Word address, Byte value);
        Byte readByte(Word address);
        
        
    private:
        Byte ioRegisters[0x80]; // IO Regsiters, start 0xFF00
        
};

#endif // MMU_HPP