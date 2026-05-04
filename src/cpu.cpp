#include <iostream>
#include "cpu.hpp"
#include "opcodes.hpp"
#include "opcycles.hpp"


void Cpu::reset(Mem &memory) {
    // https://gbdev.io/pandocs/Power_Up_Sequence.html?highlight=boot#cpu-registers
    // Initial values based on Game Boy
    PC = 0x0100;
    SP = 0xFFFE;    // Stack pointer starts at the end of memory
    A = 0x01;       // Accumulator
    F = 0xB0;       // Initial flags (Z=1, N=0, H=1, C=1) 1011 = 0xB0, bottom 4 bits are always 0
    B = 0x00;
    C = 0x13;
    D = 0x00;
    E = 0xD8;
    H = 0x01;
    L = 0x4D;

    memory.init(); // Initialize memory to 0
}

void Cpu::updateFlags(Byte result, bool isSubtraction, bool halfCarry, bool carry) {
    bool flag_z = (result == 0); // Set Z flag if result is zero
    bool flag_n = isSubtraction; // Set N flag for subtraction, reset for addition
    bool flag_h = halfCarry; // Set H flag based on half carry
    bool flag_c = carry; // Set C flag based on carry
    F = (flag_z << 7) | (flag_n << 6) | (flag_h << 5) | (flag_c << 4); // Update flags in F register
}

void Cpu::showAllRegisterValues() {
    Byte flags = F;
    std::cout << "Flags: Z=" << ((flags & 0x80) ? "1" : "0") << " N=" << ((flags & 0x40) ? "1" : "0") << " H=" << ((flags & 0x20) ? "1" : "0") << " C=" << ((flags & 0x10) ? "1" : "0") << std::endl;
    std::cout << "AF: 0x" << std::hex << AF << std::dec << " (A: 0x" << std::hex << static_cast<int>(A) 
              << ", F: 0x" << std::hex << static_cast<int>(F) << std::dec << ")" << std::endl;
    std::cout << "BC: 0x" << std::hex << BC << std::dec << " (B: 0x" << std::hex << static_cast<int>(B) 
              << ", C: 0x" << std::hex << static_cast<int>(C) << std::dec << ")" << std::endl;
    std::cout << "DE: 0x" << std::hex << DE << std::dec << " (D: 0x" << std::hex << static_cast<int>(D) 
              << ", E: 0x" << std::hex << static_cast<int>(E) << std::dec << ")" << std::endl;
    std::cout << "HL: 0x" << std::hex << HL << std::dec << " (H: 0x" << std::hex << static_cast<int>(H) 
              << ", L: 0x" << std::hex << static_cast<int>(L) << std::dec << ")" << std::endl;
    std::cout << "SP: 0x" << std::hex << SP << std::dec << std::endl;
    std::cout << "PC: 0x" << std::hex << PC << std::dec << std::endl;
}

Byte Cpu::fetchInstruction(uint &cycles, Mem &memory) {
    Byte opcode = memory[PC];
    std::cout << "Fetched opcode: 0x" << std::hex << static_cast<int>(opcode) << " at address: 0x" << std::hex << PC << std::dec << std::endl;
    PC++; // Increment program counter to point to the next instruction
    cycles--;
    return opcode;
}

Byte Cpu::readByte(Mem &memory, Word address) {
    Byte value = memory[address];
    return value;
}

Word Cpu::loadWord(Mem &memory) {
    Byte low = memory[PC];
    PC++; // Increment program counter to point to the next byte
    Byte high = memory[PC];
    PC++; // Increment program counter to point to the next instruction
    Word value = (high << 8) | low;
    return value;
}

Byte Cpu::loadByte(Mem &memory) {
    Byte value = memory[PC];
    PC++; // Increment program counter to point to the next instruction
    return value;
}

Word Cpu::incWord(Word value) {
    // todo, probably need to check this is the right implementation
    return (value + 1) & 0xFFFF; // Increment and wrap around at 0xFFFF
}

Word Cpu::decWord(Word value) {
    return (value - 1) & 0xFFFF; // Decrement and wrap around at 0x0000
}

void Cpu::loadRegToMemory(Mem &memory, Word address, Byte &reg) {
    memory[address] = reg;
}

void Cpu::loadRegFromMemory(Mem &memory, Word address, Byte &reg) {
    reg = memory[address];
}

void Cpu::loadRegToReg(Byte &dest, Byte &src) {
    dest = src;
}

void Cpu::andRegToA(Byte &src) {
    A &= src;
    updateFlags(A, false, true, false); // Z flag is set based on result, N H C flags are reset
}

void Cpu::orRegToA(Byte &src) {
    A |= src;
    updateFlags(A, false, false, false); // Z flag is set based on result, N H C flags are reset
}

void Cpu::xorRegToA(Byte &src) {
    A ^= src;
    updateFlags(A, false, false, false); // Z flag is set based on result, N H C flags are reset
}

void Cpu::addRegToReg(Byte &dest, Byte &src) {
    // todo
    bool flag_h = ((dest & 0x0F) + (src & 0x0F)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = (dest + src) > 0xFF; // Set C flag if there is a carry from bit 7
    dest += src;
    updateFlags(dest, false, flag_h, flag_c);
}

void Cpu::subRegToReg(Byte &dest, Byte &src) {
    // todo
    // flag_h = (dest & 0x0F) < (src & 0x0F); // Set H flag if there is a borrow from bit 4
    // flag_c = dest < src; // Set C flag if there is a borrow from bit 8
    // dest -= src;
    // updateFlags(dest, true, flag_h, flag_c);
}

void Cpu::PopStackToReg(Word &reg, Mem &memory) {
    reg = memory[SP] | (memory[SP + 1] << 8); // Pop low byte and high byte from stack
    SP += 2; // Increment stack pointer by 2
}

void Cpu::PushRegToStack(Word reg, Mem &memory) {
    SP -= 1;
    memory[SP] = (reg >> 8) & 0xFF; // Push high byte onto stack
    SP -= 1;
    memory[SP] = reg & 0xFF; // Push low byte onto stack
}


void Cpu::executeInstruction(uint cycles, Mem &memory) {
    while (cycles > 0) {
        Byte opcode = fetchInstruction(cycles, memory);
        // std::cout << "cycles: " << cycles << std::endl;

        switch (opcode) {
            // x0 opcodes
            case NOP: {
                std::cout << "Executed NOP" << std::endl;
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case STOP: {
                // TODO
                std::cout << "Executed STOP" << std::endl;
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JR_NZ_s8: {
                // TODO
                break;
            }
            case JR_NC_s8: {
                // TODO
                break;
            }
            case LD_B_B: {
                cycles -= opcycles[opcode] - 1; // effectively NOP
                break;
            }
            case LD_D_B: {
                loadRegToReg(D, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_B: {
                loadRegToReg(H, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_B: {
                loadRegToMemory(memory, HL, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_B: {
                addRegToReg(A, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_B: {
                subRegToReg(A, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_B: {
                andRegToA(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_B: {
                orRegToA(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RET_NZ: {
                // TODO
                break;
            }
            case RET_NC: {
                // TODO
                break;
            }
            case LD_a8mem_A: {
                Byte a8 = loadByte(memory);
                loadRegToMemory(memory, a8, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_a8mem: {
                Byte a8 = loadByte(memory);
                loadRegFromMemory(memory, a8, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x1 opcodes
            case LD_BC_d16: {
                BC = loadWord(memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_DE_d16: {
                DE = loadWord(memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HL_d16: {
                HL = loadWord(memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_SP_d16: {
                SP = loadWord(memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_C: {
                loadRegToReg(B, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_C: {
                loadRegToReg(D, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_C: {
                loadRegToReg(H, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_C: {
                loadRegToMemory(memory, HL, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_C: {
                addRegToReg(A, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_C: {
                subRegToReg(A, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_C: {
                andRegToA(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_C: {    
                orRegToA(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_BC: {
                PopStackToReg(BC, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_DE: {
                PopStackToReg(DE, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_HL: {
                PopStackToReg(HL, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_AF: {
                PopStackToReg(AF, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x2 opcodes
            case LD_BCmem_A: {
                loadRegToMemory(memory, BC, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_DEmem_A: {
                loadRegToMemory(memory, DE, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLi_A: {
                loadRegToMemory(memory, HL, A);
                HL = incWord(HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLd_A: {
                loadRegToMemory(memory, HL, A);
                HL = decWord(HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_D: {
                loadRegToReg(B, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_D: {
                cycles -= opcycles[opcode] - 1; // effectively NOP
                break;
            }
            case LD_H_D: {
                loadRegToReg(H, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_D: {
                loadRegToMemory(memory, HL, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_D: {
                addRegToReg(A, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_D: {
                subRegToReg(A, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_D: {
                andRegToA(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_D: {
                orRegToA(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JP_NZ_a16: {
                // TODO
                break;
            }
            case JP_NC_a16: {
                // TODO
                break;
            }
            case LD_Cmem_A: {
                loadRegToMemory(memory, C, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_Cmem: {
                loadRegFromMemory(memory, C, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }


            case LD_A_BCmem: {
                loadRegToMemory(memory, BC, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            default:
                    std::cout << "Unknown opcode: 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
                    break;
        }
    }
};