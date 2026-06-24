#include <string>
#include "utils/types.hpp"
#include "utils/file.hpp"

#ifndef MMU_HPP
#define MMU_HPP

class Mmu {
    public:
        Mmu();

        std::string title;

        Byte romBank0[0x4000]; // first 16KB of cart, start 0x0000
        Byte romBankN[0x4000]; // switchable 16KB bank, start 0x4000
        Byte VRam[0x2000]; // 8KB of VRAM, start 0x8000
        Byte workRamBank0[0x1000]; // 4KB of work RAM, start 0xC000
        Byte workRamBankN[0x1000]; // switchable 4KB bank, start 0xD000
        Byte oam[0xA0]; // Object Attribute Memory, start 0xFE00
        Byte HRam[0x7F]; // High RAM, start 0xFF80
        Byte ioRegisters[0x80]; // IO Regsiters, start 0xFF00
        Byte interruptEnableRegister; // IE Register, memory location 0xFFFF
        
        // io register addresses
        static const Word P1 = 0xFF00;      // Joypad (JOYP)
        static const Word SB = 0xFF01;      // Serial Transfer Data
        static const Word SC = 0xFF02;      // Serial Transfer Control
        static const Word DIV = 0xFF04;     // Divider Register
        static const Word TIMA = 0xFF05;    // Timer Counter
        static const Word TMA = 0xFF06;     // Timer Modulo
        static const Word TAC = 0xFF07;     // TImer Control
        static const Word IF = 0xFF0F;      // Interrupt Flag
        static const Word NR10 = 0xFF10;
        static const Word NR11 = 0xFF11;
        static const Word NR12 = 0xFF12;
        static const Word NR13 = 0xFF13;
        static const Word NR14 = 0xFF14;
        static const Word NR21 = 0xFF16;
        static const Word NR22 = 0xFF17;
        static const Word NR23 = 0xFF18;
        static const Word NR24 = 0xFF19;
        static const Word NR30 = 0xFF1A;
        static const Word NR31 = 0xFF1B;
        static const Word NR32 = 0xFF1C;
        static const Word NR33 = 0xFF1D;
        static const Word NR34 = 0xFF1E;
        static const Word NR41 = 0xFF20;
        static const Word NR42 = 0xFF21;
        static const Word NR43 = 0xFF22;
        static const Word NR44 = 0xFF23;
        static const Word NR50 = 0xFF24;
        static const Word NR51 = 0xFF25;
        static const Word NR52 = 0xFF26;    // Sound On/Off
        static const Word LCDC = 0xFF40;    // LCD Controller
        static const Word STAT = 0xFF41;    // status of the LCD
        static const Word SCY = 0xFF42;     // Scroll Y
        static const Word SCX = 0xFF43;     // Scroll X
        static const Word LY = 0xFF44;      // LCD Y Coord
        static const Word LYC = 0xFF45;     // LY Compare
        static const Word DMA = 0xFF46;     // OAM DMA (Direct Memory Access) source address and start
        static const Word BGP = 0xFF47;     // BG Palette Data
        static const Word OBP0 = 0xFF48;    // OBJ Palette 0 Data
        static const Word OBP1 = 0xFF49;    // OBJ Palette 1 Data
        static const Word WY = 0xFF4A;      // Window Y
        static const Word WX = 0xFF4B;      // Window X
        static const Word KEY0 = 0xFF4C;    // CPU Mode Select (SYS)
        static const Word KEY1 = 0xFF4D;    // Prepare Speed Switch (SPD)
        static const Word VBK = 0xFF4F;     // VRAM Bank
        static const Word BANK = 0xFF50;    // Boot ROM Mapping Control
        static const Word HDMA1 = 0xFF51;
        static const Word HDMA2 = 0xFF52;
        static const Word HDMA3 = 0xFF53;
        static const Word HDMA4 = 0xFF54;
        static const Word HDMA5 = 0xFF55;
        static const Word RP = 0xFF56;      // Infrared Comms port
        static const Word BCPS = 0xFF68;    // Bg color palette spec/index
        static const Word BCPD = 0xFF69;    // Bg color palette data
        static const Word OCPS = 0xFF6A;    // Obj color palette spec/index
        static const Word OCPD = 0xFF6B;    // Obj color palette data
        static const Word SVBK = 0xFF70;    // WRAM bank
        
        
        void reset();
        void loadRom(const std::vector<Byte>& program);
        void swapRomBank(Byte bank);
        void writeByte(Word address, Byte value);
        Byte readByte(Word address);  
        int8_t readInt(Word address);  
        
    private:
        // https://gbdev.io/pandocs/The_Cartridge_Header.html#0147--cartridge-type
        Byte MBCType = 0; // type of memory bank controller, value found at 0x147 in ROM header

        Byte entireRom[0x18000]; // max ROM size is 2MB, allocating that
        Byte currentRomBank = 1;
        Byte ROMSize = 0; // 0x148 in ROM header

        Byte externalRam[0x8000]; // max size of external RAM is 32KB, so allocating plenty. Most use less
        Byte currentRamBank = 0;
        Byte RAMSize = 0; // 0x149 in ROM header

};

#endif // MMU_HPP