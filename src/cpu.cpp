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

Byte Cpu::fetchInstruction(uint &cycles, Mem &memory) {
    Byte opcode = memory[PC];
    std::cout << "Fetched opcode: 0x" << std::hex << static_cast<int>(opcode) << " at address: 0x" << std::hex << PC << std::dec << std::endl;
    PC++; // Increment program counter to point to the next instruction
    cycles--;
    return opcode;
}

void Cpu::executeInstruction(uint cycles, Mem &memory) {
    while (cycles > 0) {
        Byte opcode = fetchInstruction(cycles, memory);

        switch (opcode) {
            case NOP: {
                std::cout << "Executed NOP" << std::endl;
                break;
            } case JP_NZ_a16: {
                break;
            } case LD_BC_d16: {
                Byte low = fetchInstruction(cycles, memory);
                Byte high = fetchInstruction(cycles, memory);
                BC = (high << 8) | low;
                std::cout << "Executed LD BC,d16 with value: 0x" << std::hex << BC << std::dec << std::endl;
                break;
            }
            default:
                    std::cout << "Unknown opcode: 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
                    break;
        }
    }
};