#include <iostream>
#include "cpu.hpp"


void Cpu::reset(Mem &memory) {
    // https://gbdev.io/pandocs/Power_Up_Sequence.html?highlight=boot#cpu-registers
    PC = 0x0100;
    SP = 0xFFFE;    // Stack pointer starts at the end of memory
    A = 0x01;       // Initial values based on Game Boy
    F = 0xB0;       // Initial flags (Z=1, N=0, H=1, C=1) 1011 = 0xB0
    B = 0x00;
    C = 0x13;
    D = 0x00;
    E = 0xD8;
    H = 0x01;
    L = 0x4D;

    memory.init(); // Initialize memory to 0
}

void Cpu::showAllRegisterValues() {
    std::cout << "AF: 0x" << std::hex << AF << std::dec << " (A: 0x" << std::hex << static_cast<int>(A) 
              << ", F: 0x" << std::hex << static_cast<int>(F) << std::dec << ")" << std::endl;
    std::cout << "BC: 0x" << std::hex << BC << std::dec << " (B: 0x" << std::hex << static_cast<int>(B) 
              << ", C: 0x" << std::hex << static_cast<int>(C) << std::dec << ")" << std::endl;
    std::cout << "DE: 0x" << std::hex << DE << std::dec << " (D: 0x" << std::hex << static_cast<int>(D) 
              << ", E: 0x" << std::hex << static_cast<int>(E) << std::dec << ")" << std::endl;
    std::cout << "HL: 0x" << std::hex << HL << std::dec << " (H: 0x" << std::hex << static_cast<int>(H) 
              << ", L: 0x" << std::hex << static_cast<int>(L) << std::dec << ")" << std::endl;
}

Byte Cpu::fetchInstruction(uint &cycles, Mem &memory) {
    Byte opcode = memory[PC];
    std::cout << "Fetched opcode: 0x" << std::hex << static_cast<int>(opcode) << " at address: 0x" << std::hex << PC << std::dec << std::endl;
    PC++; // Increment program counter to point to the next instruction
    cycles--;
    return opcode;
}

Byte Cpu::readByte(uint &cycles, Mem &memory, Word address) {
    Byte value = memory[address];
    // cycles--;
    return value;
}

Word Cpu::loadWord(uint &cycles, Mem &memory) {
    Byte low = memory[PC];
    PC++; // Increment program counter to point to the next byte
    Byte high = memory[PC];
    PC++; // Increment program counter to point to the next instruction
    Word value = (high << 8) | low;
    // cycles -= 2; // Assuming reading a word takes 2 cycles
    return value;
}

Byte Cpu::loadByte(uint &cycles, Mem &memory) {
    Byte value = memory[PC];
    PC++; // Increment program counter to point to the next instruction
    // cycles--; // Assuming reading a byte takes 1 cycle
    return value;
}

Word Cpu::incWord(Word value) {
    return (value + 1) & 0xFFFF; // Increment and wrap around at 0xFFFF
}

void Cpu::loadToMemory(uint &cycles, Mem &memory, Word address, Byte &reg) {
    memory[address] = reg;
    // cycles--; // Assuming memory access takes 1 cycle
}

void Cpu::loadFromMemory(uint &cycles, Mem &memory, Word address, Byte &reg) {
    reg = memory[address];
    // cycles--; // Assuming memory access takes 1 cycle
}

void Cpu::loadRegToReg(Byte &dest, Byte &src) {
    dest = src;
}

void Cpu::executeInstruction(uint cycles, Mem &memory) {
    while (cycles > 0) {
        Byte opcode = fetchInstruction(cycles, memory);
        std::cout << "cycles: " << cycles << std::endl;

        switch (opcode) {
            case NOP: {
                std::cout << "Executed NOP" << std::endl;
                cycles -= Cpu::opcycles[opcode] - 1;
                break;
            } 
            case LD_BC_d16: {
                BC = loadWord(cycles, memory);
                cycles -= Cpu::opcycles[opcode] - 1;
                std::cout << "Executed LD BC,d16 with value: 0x" << std::hex << BC << std::dec << std::endl;
                break;
            }
            case LD_B_C: {
                loadRegToReg(B, C);
                cycles -= Cpu::opcycles[opcode] - 1;
                std::cout << "Executed LD B,C with value: 0x" << std::hex << static_cast<int>(B) << std::dec << std::endl;
                break;
            }
            case LD_BCmem_A: {
                loadFromMemory(cycles, memory, BC, A);
                cycles -= Cpu::opcycles[opcode] - 1;
                std::cout << "Executed LD (BC),A with value: 0x" << std::hex << static_cast<int>(A) << std::dec << std::endl;
                break;
            }
            case LD_A_BCmem: {
                loadToMemory(cycles, memory, BC, A);
                cycles -= Cpu::opcycles[opcode] - 1;
                std::cout << "Executed LD A,(BC) with value: 0x" << std::hex << static_cast<int>(A) << std::dec << std::endl;
                break;
            }
            default:
                    std::cout << "Unknown opcode: 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
                    break;
        }
    }
};