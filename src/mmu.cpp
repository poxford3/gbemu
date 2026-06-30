#include "mmu.hpp"

Mmu::Mmu() {
    std::fill(std::begin(romBank0), std::end(romBank0), 0);
    std::fill(std::begin(romBankN), std::end(romBankN), 0);
    std::fill(std::begin(VRam), std::end(VRam), 0);
    std::fill(std::begin(workRamBank0), std::end(workRamBank0), 0);
    std::fill(std::begin(workRamBankN), std::end(workRamBankN), 0);
    std::fill(std::begin(oam), std::end(oam), 0);
    std::fill(std::begin(HRam), std::end(HRam), 0);
    ioRegisters[interruptEnableRegister - 0xFF00] = 0;
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

    memcpy(title.data(), program.data() + 0x134, 16); // set the title, which comes from 0x134 to 0x143 in ROM header
    printf("title %s\n", title.c_str());

    std::copy(program.begin(), program.end(), entireRom);
    // pandocs pg 164
    getMBCType(program[0x147]);
    // pandocs pg 156
    ROMSize = 0x7D00 * (1 << program[0x148]); // 32kb * number of banks
    RAMSize = (program[0x149] == 0) ? 0 : (1 << (program[0x149] - 1)) * 0x2000; // todo verify this
    RAMEnabled = false;
}


void Mmu::getMBCType(Byte MBCvalue) {
    // https://gbdev.io/pandocs/MBCs.html#mbc-unmapped-ram-bank-access
    // https://gbdev.io/pandocs/The_Cartridge_Header.html?highlight=%240148#0147--cartridge-type
    switch (MBCvalue) {
        case 0x00: // MBC 0
            MBCType = 0;
            break;
        case 0x01: 
        case 0x02: 
        case 0x03: // MBC 1
            MBCType = 1;
            break;
        case 0x05:
        case 0x06: // MBC 2
            MBCType = 2;
            break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13: // MBC 3
            MBCType = 3;
            break;
    }
}


void Mmu::getRamSize(Byte RAMvalue, Byte MBCvalue) {
    // all the carts that have ram on the board
    std::vector<Byte> listOfPossibleRamCarts = {
        0x02, 0x03, 0x08, 0x09, 0x0C, 0x0D,
        0x10, 0x12, 0x13, 0x1A, 0x1B, 0x1D,
        0x1E, 0x22, 0xFF
    };
    // https://stackoverflow.com/a/24139474/7361467
    if ((std::find(listOfPossibleRamCarts.begin(), listOfPossibleRamCarts.end(), MBCvalue)) != listOfPossibleRamCarts.end()) {

    }
}


void Mmu::reset() {
    std::fill(std::begin(romBank0), std::end(romBank0), 0);
    std::fill(std::begin(romBankN), std::end(romBankN), 0);
    std::fill(std::begin(externalRam), std::end(externalRam), 0);
    std::fill(std::begin(VRam), std::end(VRam), 0);
    std::fill(std::begin(workRamBank0), std::end(workRamBank0), 0);
    std::fill(std::begin(workRamBankN), std::end(workRamBankN), 0);
    std::fill(std::begin(oam), std::end(oam), 0);
    std::fill(std::begin(HRam), std::end(HRam), 0);
    currentRomBank = 1;
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



void Mmu::handleRomWrite(Word address, Byte value) {
	int romsize = 0x7D00 * (1 << ROMSize); // 32,000 kib ($7D00 == 32000)

	switch (MBCType) {
        case 0: {
            // nothing happens when you write to a cartrige area in a non-MBC
            break;
        }
        case 1: { // MBC1
            Byte reg = address >> 13 & 0x02; // get the 13th and 14th bits of the address (selects the regiser)
            switch (reg) {
                case 0x00: { // RAM Enable
                    std::cout << "case 0x00 " << int(value) << " | " << int(value & 0x0F) << std::endl;
                    if ((value & 0x0F) == 0xA) { //  check bottom 4 bits to see if $A
                        RAMEnabled = true;
                    } else {
                        RAMEnabled = false;
                    }
                    break;
                }
                case 0x01: { // ROM bank number
                    // value & 0x1F will return 1 of 32 possible values (0-31)
                    // std::cout << "case 0x01" << std::endl;
                    currentRomBank = value & 0x1F;
                    if (currentRomBank == 0) currentRomBank = 1; //  bank num cannot be 0 to not use first 16kb of ROM
                    break;
                }
                case 0x02: { // RAM bank number or Upper Bits of ROM Bank number
                    // std::cout << "case 0x02" << std::endl;
                    if (romsize >= 1000000) { //  roms 1mb and larger
                        currentRomBank = value & 0x60; // get bits 5 and 6 TODO FINISH HERE
                    } else {
                        currentRamBank = value & 0x03; // get last 2 bits
                    }
                    break;
                }
                case 0x03: { //  Banking mode select
                    // std::cout << "case 0x03" << std::endl;
                    bankingMode = value & 0x1;
                    break;
                }
            }
        }
        case 2: { // MBC2

        }
    }
}


void Mmu::writeByte(Word address, Byte value) {
    //     if (address >= 0x0000 && address <= 0x3FFF) {
    //     romBank0[address] = value; // todo look into if this should be read only so never write to it
    // } else if (address >= 0x4000 && address <= 0x7FFF) {
    //     romBankN[address - 0x4000] = value;
    // } 

    if (address >= 0x0000 && address <= 0x7FFF) {
        handleRomWrite(address, value);
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        VRam[address - 0x8000] = value;
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        if (RAMEnabled) { // check if external RAM is enabled
            externalRam[address - 0xA000] = value;
        }
    } else if (address >= 0xC000 && address <= 0xCFFF) {
        workRamBank0[address - 0xC000] = value;
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        workRamBankN[address - 0xD000] = value;
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
        oam[address - 0xFE00] = value;
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
        HRam[address - 0xFF80] = value;
    } else if (address >= 0xFF00 && address <= 0xFF7F) {
        if (address == DIV) {
            ioRegisters[address - 0xFF00] = 0; // writes to DIV (0xFF04) always result in 0
        } else {
            ioRegisters[address - 0xFF00] = value;
        }
    } else if (address == 0xFFFF) {
        interruptEnableRegister = value; 
    } else {
        return; // if not mapped, nothing to write to
    }
}


Byte Mmu::readByte(Word address) {
    if (ioRegisters[0xFF02 - 0xFF00] == 0x81) {
        char c = ioRegisters[0xFF01 - 0xFF00];
        printf("%c", c);
        fflush(stdout);
        ioRegisters[0xFF02 - 0xFF00] = 0x00;
    }

    if (address >= 0xFF00 && address <= 0xFF7F) {
        return ioRegisters[address - 0xFF00];
    } else if (address == 0xFFFF) {
        return interruptEnableRegister; 
    } else if (address >= 0x0000 && address <= 0x3FFF) {
        return romBank0[address];
    } else if (address >= 0x4000 && address <= 0x7FFF) {
        return romBankN[address - 0x4000];
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        if (RAMSize > 0) { // todo verify this behavior (only putting out a value if it's enabled)
            return externalRam[address - 0xA000];
        } else return 0xFF; // if external ram isn't enabled, often just returning 0xFF
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        return VRam[address - 0x8000];
    } else if (address >= 0xC000 && address <= 0xCFFF) {
        return workRamBank0[address - 0xC000];
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        return workRamBankN[address - 0xD000];
    } else if (address >= 0xE000 && address <= 0xFDFF) {
        return workRamBank0[address - 0xE000]; // echo, shouldn't be used but mirrors work RAM bank 0
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
        return oam[address - 0xFE00];
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {
        std::cerr << "Warning: attempting to access prohibited section of memory at address 0x" << std::hex << address << std::dec << std::endl;
        return 0xFF;
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
        return HRam[address - 0xFF80];
    } else {
        // unmapped memory, return 0xFF
        return 0xFF;
    }
}


int8_t Mmu::readInt(Word address) {
    if (address >= 0xFF00 && address <= 0xFF7F) {
        return static_cast<int8_t>(ioRegisters[address - 0xFF00]);
    } else if (address == 0xFFFF) {
        return static_cast<int8_t>(interruptEnableRegister);
    } else if (address >= 0x0000 && address <= 0x3FFF) {
        return static_cast<int8_t>(romBank0[address]);
    } else if (address >= 0x4000 && address <= 0x7FFF) {
        return static_cast<int8_t>(romBankN[address - 0x4000]);
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        return static_cast<int8_t>(VRam[address - 0x8000]);
    } else if (address >= 0xC000 && address <= 0xCFFF) {
        return static_cast<int8_t>(workRamBank0[address - 0xC000]);
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        return static_cast<int8_t>(workRamBankN[address - 0xD000]);
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
        return static_cast<int8_t>(oam[address - 0xFE00]);
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
        return static_cast<int8_t>(HRam[address - 0xFF80]);
    } else {
        // unmapped memory, return 0xFF
        return 0xFF;
    }
}