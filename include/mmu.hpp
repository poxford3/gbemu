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
        Byte ioRegisters[0x80]; // IO Regsiters, start 0xFF00
        Byte HRam[0x7F]; // High RAM, start 0xFF80
        Byte interruptEnableRegister; // IE Register, start 0xFFFF

        void loadRom(const std::vector<Byte>& program);

        
    private:
};

#endif // MMU_HPP