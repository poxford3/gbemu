#include "mmu.hpp"

Mmu::Mmu() {
    // initialize ioRegisters to 0
    std::fill(std::begin(romBank0), std::end(romBank0), 0);
    std::fill(std::begin(romBankN), std::end(romBankN), 0);
    std::fill(std::begin(VRam), std::end(VRam), 0);
    std::fill(std::begin(workRamBank0), std::end(workRamBank0), 0);
    std::fill(std::begin(workRamBankN), std::end(workRamBankN), 0);
    std::fill(std::begin(oam), std::end(oam), 0);
    std::fill(std::begin(HRam), std::end(HRam), 0);
    interruptEnableRegister = 0;
    std::fill(std::begin(ioRegisters), std::end(ioRegisters), 0);
}

void Mmu::loadRom(const std::vector<Byte>& program) {
    uint numBytes = program.size();

    for (uint i = 0; i < numBytes && i < 0x8000; i++) {
        if (i < 0x4000) {
            romBank0[i] = program[i];
        } else {
            romBankN[i - 0x4000] = program[i];
        }
    }
}


void Mmu::swapRomBank(Byte bank) {

}


void Mmu::reset() {
    std::fill(std::begin(romBank0), std::end(romBank0), 0);
    std::fill(std::begin(romBankN), std::end(romBankN), 0);
    std::fill(std::begin(VRam), std::end(VRam), 0);
    std::fill(std::begin(workRamBank0), std::end(workRamBank0), 0);
    std::fill(std::begin(workRamBankN), std::end(workRamBankN), 0);
    std::fill(std::begin(oam), std::end(oam), 0);
    std::fill(std::begin(HRam), std::end(HRam), 0);
    interruptEnableRegister = 0x00;
    ioRegisters[P1 - 0xFF00] = 0xCF;
    ioRegisters[SB - 0xFF00] = 0x0;
    ioRegisters[SC - 0xFF00] = 0x7E;
    ioRegisters[DIV - 0xFF00] = 0x18;
    ioRegisters[TIMA - 0xFF00] = 0x0;
    ioRegisters[TMA - 0xFF00] = 0x0;
    ioRegisters[TAC - 0xFF00] = 0xF8;
    ioRegisters[IF - 0xFF00] = 0xE1;
    ioRegisters[NR10 - 0xFF00] = 0x80;
    ioRegisters[NR11 - 0xFF00] = 0xBF;
    ioRegisters[NR12 - 0xFF00] = 0xF3;
    ioRegisters[NR13 - 0xFF00] = 0xFF;
    ioRegisters[NR14 - 0xFF00] = 0xBF;
    ioRegisters[NR21 - 0xFF00] = 0x3F;
    ioRegisters[NR22 - 0xFF00] = 0x0;
    ioRegisters[NR23 - 0xFF00] = 0xFF;
    ioRegisters[NR24 - 0xFF00] = 0xBF;
    ioRegisters[NR30 - 0xFF00] = 0x7F;
    ioRegisters[NR31 - 0xFF00] = 0xFF;
    ioRegisters[NR32 - 0xFF00] = 0x9F;
    ioRegisters[NR33 - 0xFF00] = 0xFF;
    ioRegisters[NR34 - 0xFF00] = 0xBF;
    ioRegisters[NR41 - 0xFF00] = 0xFF;
    ioRegisters[NR42 - 0xFF00] = 0x0;
    ioRegisters[NR43 - 0xFF00] = 0x0;
    ioRegisters[NR44 - 0xFF00] = 0xBF;
    ioRegisters[NR50 - 0xFF00] = 0x77;
    ioRegisters[NR51 - 0xFF00] = 0xF3;
    ioRegisters[NR52 - 0xFF00] = 0xF1;
    ioRegisters[LCDC - 0xFF00] = 0x91;
    ioRegisters[STAT - 0xFF00] = 0x81;
    ioRegisters[SCY - 0xFF00] = 0x0;
    ioRegisters[SCX - 0xFF00] = 0x0;
    ioRegisters[LY - 0xFF00] = 0x91;
    ioRegisters[LYC - 0xFF00] = 0x0;
    ioRegisters[DMA - 0xFF00] = 0xFF;
    ioRegisters[BGP - 0xFF00] = 0xFC;
    // ioRegisters[OBP0 - 0xFF00] = 0x??7;
    // ioRegisters[OBP1 - 0xFF00] = 0x??7;
    ioRegisters[WY - 0xFF00] = 0x0;
    ioRegisters[WX - 0xFF00] = 0x0;
    // ioRegisters[KEY0 - 0xFF00] = 0x—;
    // ioRegisters[KEY1 - 0xFF00] = 0x—;
    // ioRegisters[VBK - 0xFF00] = 0x—;
    // ioRegisters[BANK - 0xFF00] = 0x—;
    // ioRegisters[HDMA1 - 0xFF00] = 0x—;
    // ioRegisters[HDMA2 - 0xFF00] = 0x—;
    // ioRegisters[HDMA3 - 0xFF00] = 0x—;
    // ioRegisters[HDMA4 - 0xFF00] = 0x—;
    // ioRegisters[HDMA5 - 0xFF00] = 0x—;
    // ioRegisters[RP - 0xFF00] = 0x—;
    // ioRegisters[BCPS - 0xFF00] = 0x—;
    // ioRegisters[BCPD - 0xFF00] = 0x—;
    // ioRegisters[OCPS - 0xFF00] = 0x—;
    // ioRegisters[OCPD - 0xFF00] = 0x—;
    // ioRegisters[SVBK - 0xFF00] = 0x—;
}


void Mmu::writeByte(Word address, Byte value) {
    if (address >= 0x0000 && address <= 0x3FFF) {
        romBank0[address] = value;
    } else if (address >= 0x4000 && address <= 0x7FFF) {
        romBankN[address - 0x4000] = value;
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        VRam[address - 0x8000] = value;
    } else if (address >= 0xC000 && address <= 0xCFFF) {
        workRamBank0[address - 0xC000] = value;
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        workRamBankN[address - 0xD000] = value;
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
        oam[address - 0xFE00] = value;
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
        HRam[address - 0xFF80] = value;
    } else if (address >= 0xFF00 && address <= 0xFF7F) {
        ioRegisters[address - 0xFF00] = value;
    } else if (address == 0xFFFF) {
        interruptEnableRegister = value; 
    } else {
        return; // if not mapped, nothing to write to
    }
}


Byte Mmu::readByte(Word address) {
    if (address >= 0xFF00 && address <= 0xFF7F) {
        return ioRegisters[address - 0xFF00];
    } else if (address == 0xFFFF) {
        return interruptEnableRegister; 
    } else if (address >= 0x0000 && address <= 0x3FFF) {
        return romBank0[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
        return romBankN[address - 0x4000];
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        return VRam[address - 0x8000];
    } else if (address >= 0xC000 && address <= 0xCFFF) {
        return workRamBank0[address - 0xC000];
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        return workRamBankN[address - 0xD000];
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
        return oam[address - 0xFE00];
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
        return HRam[address - 0xFF80];
    } else {
        // unmapped memory, return 0xFF
        return 0xFF;
    }
}