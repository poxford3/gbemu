#include "square.hpp"

void Square::update(Mmu &memory, uint &cycles) {
    if (hasSweep) {
        NRx0 = memory.getIoRegister(Mmu::NR10);
        NRx1 = memory.getIoRegister(Mmu::NR11);
        NRx2 = memory.getIoRegister(Mmu::NR12);
        NRx3 = memory.getIoRegister(Mmu::NR13);
        NRx4 = memory.getIoRegister(Mmu::NR14);
    } else {
        NRx1 = memory.getIoRegister(Mmu::NR21);
        NRx2 = memory.getIoRegister(Mmu::NR22);
        NRx3 = memory.getIoRegister(Mmu::NR23);
        NRx4 = memory.getIoRegister(Mmu::NR24);
    }
    
}