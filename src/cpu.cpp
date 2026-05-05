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

Word Cpu::incWord(Word value) {
    // todo, probably need to check this is the right implementation
    return (value + 1) & 0xFFFF; // Increment and wrap around at 0xFFFF
}

Word Cpu::decWord(Word value) {
    return (value - 1) & 0xFFFF; // Decrement and wrap around at 0x0000
}

void Cpu::rotateLeft(Byte &value, std::optional<bool> throughCarry) {

    bool oldBit7 = (value >> 7) & 1; // Get the old bit 7
    value <<= 1; // Shift left by 1
    if (throughCarry.has_value() && throughCarry.value()) {
        value |= (F & 0x10) >> 4; // Rotate through carry: old bit 7 goes to carry, and old carry goes to bit 0 (RL)
    } else {
        value |= oldBit7 ? 1 : 0; // Rotate: old bit 7 goes to bit 0 (RLC)
    }
    // either way, old bit 7 goes to carry
    F = (F & 0xEF) | (oldBit7 ? 0x10 : 0); // Update carry flag based on old bit 7
}

void Cpu::rotateRight(Byte &value, std::optional<bool> throughCarry) {
    bool oldBit0 = value & 1; // Get the old bit 0
    value >>= 1; // Shift right by 1
    if (throughCarry.has_value() && throughCarry.value()) {
        value |= (F & 0x10) << 3; // Rotate through carry: old bit 0 goes to carry, and old carry goes to bit 7
    } else {
        value |= oldBit0 ? 0x80 : 0; // Rotate: old bit 0 goes to bit 7
    }
    F = (F & 0xEF) | (oldBit0 ? 0x10 : 0); // Update carry flag based on old bit 0
}

void Cpu::jp(Word address, std::optional<bool> condition) {
    if (!condition.has_value() || condition.value()) { // checks if condition is not provided (always jump) or if condition is true
        PC = address;
    } else {
        PC++;
    }
}

void Cpu::jr(Byte offset, std::optional<bool> condition) {
    if (!condition.has_value() || condition.value()) { // checks if condition is not provided (always jump) or if condition is true
        PC += static_cast<Byte>(offset); // offset is signed, so we cast it to Byte to get the correct behavior for negative offsets
    } else {
        PC++;
    }
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

void Cpu::loadRegToReg(Byte &dest, Byte &src) {
    dest = src;
}

void Cpu::loadRegToReg(Word &dest, Word &src) {
    dest = src;
}

void Cpu::loadRegToMemory(Mem &memory, Word address, Byte &reg) {
    memory[address] = reg;
}

void Cpu::loadRegFromMemory(Mem &memory, Word address, Byte &reg) {
    reg = memory[address];
}

void Cpu::RST(Word address, Mem &memory) {
    // Push current PC onto stack
    pushRegToStack(PC, memory);
    // Jump to the specified address
    PC = address;
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
    bool flag_h = ((dest & 0x0F) + (src & 0x0F)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = (dest + src) > 0xFF; // Set C flag if there is a carry from bit 7
    dest += src;
    updateFlags(dest, false, flag_h, flag_c);
}

void Cpu::addRegToReg(Word &dest, Word &src) {
    bool flag_h = ((dest & 0x0F) + (src & 0x0F)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = (dest + src) > 0xFFFF; // Set C flag if there is a carry from bit 15
    dest += src;
    updateFlags(dest, false, flag_h, flag_c);
}

Word Cpu::addByteToWord(Word &dest, Byte src) {
    bool flag_h = ((dest & 0x0F) + (src & 0x0F)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = (dest + src) > 0xFF; // Set C flag if there is a carry from bit 7
    Word result = dest + src;
    updateFlags(result, false, flag_h, flag_c);
    return result;
}

void Cpu::subRegToReg(Byte &dest, Byte &src) {
    // todo
    // flag_h = (dest & 0x0F) < (src & 0x0F); // Set H flag if there is a borrow from bit 4
    // flag_c = dest < src; // Set C flag if there is a borrow from bit 8
    // dest -= src;
    // updateFlags(dest, true, flag_h, flag_c);
}

// these need good testing
void Cpu::ADC(Byte src) {
    Word result = A + src + ((F >> 4) & 1); // Calculate A + src + carry
    bool flag_z = (result & 0xFF) == 0; // Set Z flag if result is zero
    bool flag_n = false; // Reset N flag for addition
    bool flag_h = ((A & 0x0F) + (src & 0x0F) + ((F >> 4) & 1)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = result > 0xFF; // Set C flag if there is a carry from bit 7
    A = result & 0xFF; // Store the result back in A, ensuring it stays within 8 bits
    updateFlags(A, false, flag_h, flag_c); // Update flags based on the result of the addition
}

void Cpu::SBC(Byte src) {
    Word result = A - src - ((F >> 4) & 1); // Calculate A - src - carry
    bool flag_z = (result & 0xFF) == 0; // Set Z flag if result is zero
    bool flag_n = true; // Set N flag for subtraction
    bool flag_h = ((A & 0x0F) < (src & 0x0F) + ((F >> 4) & 1)); // Set H flag if there is a borrow from bit 4
    bool flag_c = result > 0xFF; // Set C flag if there is a borrow from bit 8
    A = result & 0xFF; // Store the result back in A, ensuring it stays within 8 bits
    updateFlags(A, true, flag_h, flag_c); // Update flags based on the result of the subtraction
}

void Cpu::CP(Byte src) {
    bool h_flag = (A & 0x0F) < (src & 0x0F); // Set H flag if there is a borrow from bit 4
    bool c_flag = A < src; // Set C flag if there is a borrow from bit 8
    Byte result = A - src; // Calculate the result of A - src, but do not store it back in A
    updateFlags(result, true, h_flag, c_flag); // Update flags based on the result of the comparison
}

void Cpu::_DAA() {
    // https://www.reddit.com/r/EmuDev/comments/cdtuyw/comment/etwcyvy/
    if ((F >> 6) & 1) { // if sub
        if ((F >> 5) & 1) { // if half carry
            A = (A - 0x60) & 0xFF;
        }
        if ((F >> 4) & 1) { // if carry
            A = (A - 0x60) & 0xFF;
        }
    } else {
        if ((F >> 5) & 1) { // if half carry
            A = (A + 0x06) & 0xFF;
        }
        if (((F >> 4) & 1) || A > 0x9F) { // if carry or A > 0x9F
            A = (A + 0x60) & 0xFF;
        }
    }

    A ? F &= 0x7F : F |= 0x80; // if A=0, Z flag 1 else 0
    F &= 0x7F; // N flag reset -- potentially unnecessary 
    F &= 0xDF; // H flag reset
}


void Cpu::popStackToReg(Word &reg, Mem &memory) {
    reg = memory[SP] | (memory[SP + 1] << 8); // Pop low byte and high byte from stack
    SP += 2; // Increment stack pointer by 2
}

void Cpu::pushRegToStack(Word reg, Mem &memory) {
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
                bool z_flag = (F >> 7) & 1;
                Byte a8 = loadByte(memory);
                jr(a8, z_flag == 0);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JR_NC_s8: {
                bool c_flag = (F >> 4) & 1;
                Byte a8 = loadByte(memory);
                jr(a8, c_flag == 0);
                cycles -= opcycles[opcode] - 1;
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
                popStackToReg(BC, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_DE: {
                popStackToReg(DE, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_HL: {
                popStackToReg(HL, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case POP_AF: {
                popStackToReg(AF, memory);
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
                bool z_flag = (F >> 7) & 1;
                Word a16 = loadWord(memory);
                jp(a16, z_flag == 0);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JP_NC_a16: {
                bool c_flag = (F >> 4) & 1;
                Word a16 = loadWord(memory);
                jp(a16, c_flag == 0);
                cycles -= opcycles[opcode] - 1;
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

            // x3 opcodes
            case INC_BC: {
                BC = incWord(BC);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_DE: {
                DE = incWord(DE);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_HL: {
                HL = incWord(HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_SP: {
                SP = incWord(SP);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_E: {
                loadRegToReg(B, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_E: {
                loadRegToReg(D, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_E: {
                loadRegToReg(H, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_E: {
                loadRegToMemory(memory, HL, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_E: {
                addRegToReg(A, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_E: {
                subRegToReg(A, E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_E: {
                andRegToA(E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_E: {
                orRegToA(E);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JP_a16: {
                Word a16 = loadWord(memory);
                jp(a16);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case DI: {
                // TODO
                break;
            }

            // x4 opcodes
            case INC_B: {
                B = incWord(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_D: {
                D = incWord(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_H: {
                H = incWord(H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case INC_HLmem: {
                // load byte from memory, increment it, then store it back
                Byte value = readByte(memory, HL);
                value = incWord(value);
                loadRegToMemory(memory, HL, value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_H: {
                loadRegToReg(B, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_H: {
                loadRegToReg(D, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_H: {
                cycles -= opcycles[opcode] - 1; // effectively NOP
                break;
            }
            case LD_HLmem_H: {
                loadRegToMemory(memory, HL, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_H: {
                addRegToReg(A, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_H: {
                subRegToReg(A, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_H: {
                andRegToA(H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_H: {
                orRegToA(H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case CALL_NZ_a16: {
                // TODO
                break;
            }
            case CALL_NC_a16: {
                // TODO
                break;
            }

            // x5 opcodes
            case DEC_B: {
                B = decWord(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case DEC_D: {
                D = decWord(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case DEC_H: {
                H = decWord(H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case DEC_HLmem: {
                // load byte from memory, decrement it, then store it back
                Byte value = readByte(memory, HL);
                value = decWord(value);
                loadRegToMemory(memory, HL, value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_L: {
                loadRegToReg(B, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_L: {
                loadRegToReg(D, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_L: {
                loadRegToReg(H, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_L: {
                loadRegToMemory(memory, HL, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_L: {
                addRegToReg(A, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_L: {
                subRegToReg(A, L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_L: {
                andRegToA(L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_L: {
                orRegToA(L);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case PUSH_BC: {
                pushRegToStack(BC, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case PUSH_DE: {
                pushRegToStack(DE, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case PUSH_HL: {
                pushRegToStack(HL, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case PUSH_AF: {
                pushRegToStack(AF, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x6 opcodes
            case LD_B_d8: {
                Byte d8 = loadByte(memory);
                loadRegToReg(B, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_d8: {
                Byte d8 = loadByte(memory);
                loadRegToReg(D, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_d8: {
                Byte d8 = loadByte(memory);
                loadRegToReg(H, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_d8: {
                Byte d8 = loadByte(memory);
                loadRegToMemory(memory, HL, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_HLmem: {
                loadRegFromMemory(memory, HL, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_HLmem: {
                loadRegFromMemory(memory, HL, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_HLmem: {
                loadRegFromMemory(memory, HL, H);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case HALT: {
                // TODO
                break;
            }
            case ADD_A_HLmem: {
                Byte value = readByte(memory, HL);
                addRegToReg(A, value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_HLmem: {
                Byte value = readByte(memory, HL);
                subRegToReg(A, value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_HLmem: {
                Byte value = readByte(memory, HL);
                andRegToA(value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_HLmem: {
                Byte value = readByte(memory, HL);
                orRegToA(value);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_d8: {
                Byte d8 = loadByte(memory);
                addRegToReg(A, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_d8: {
                Byte d8 = loadByte(memory);
                subRegToReg(A, d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_d8: {
                Byte d8 = loadByte(memory);
                andRegToA(d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_d8: {
                Byte d8 = loadByte(memory);
                orRegToA(d8);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x7 opcodes
            case RLCA: {
                rotateLeft(A, false);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RLA: {
                rotateLeft(A, true);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case DAA: {
                _DAA();
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SCF: {
                Byte currentZFlag = (F >> 7) & 1; // Store current Z flag value
                updateFlags(currentZFlag, false, false, true); // Set C flag, reset N and H flags, Z flag is unaffected
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_B_A: {
                loadRegToReg(B, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_D_A: {
                loadRegToReg(D, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_H_A: {
                loadRegToReg(H, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HLmem_A: {
                loadRegToMemory(memory, HL, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_A_A: {
                addRegToReg(A, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SUB_A: {
                subRegToReg(A, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case AND_A: {
                andRegToA(A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case OR_A: {
                orRegToA(A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RST_0: {
                // memory locations 
                // 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, and 0x38 reserved for RST instructions
                // RST 0-7
                RST(0x00, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RST_2: {
                RST(0x10, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RST_4: {
                RST(0x20, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RST_6: {
                RST(0x30, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x8 opcodes
            case LD_a16mem_SP: {
                Word a16 = loadWord(memory);
                Byte SP_low = SP & 0xFF;
                loadRegToMemory(memory, a16, SP_low);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JR_s8: {
                Byte a8 = loadByte(memory);
                jr(a8);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JR_Z_s8: {
                bool z_flag = (F >> 7) & 1;
                Byte a8 = loadByte(memory);
                jr(a8, z_flag == 1);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JR_C_s8: {
                bool c_flag = (F >> 4) & 1;
                Byte a8 = loadByte(memory);
                jr(a8, c_flag == 1);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_C_B: {
                loadRegToReg(C, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_E_B: {
                loadRegToReg(E, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_L_B: {
                loadRegToReg(L, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_B: {
                loadRegToReg(A, B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADC_A_B: {
                ADC(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SBC_A_B: {
                SBC(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case XOR_B: {
                xorRegToA(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case CP_B: {
                CP(B);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RET_Z: {
                // TODO
                break;
            }
            case RET_C: {
                // TODO
                break;
            }
            case ADD_SP_s8: {
                Byte s8 = loadByte(memory);
                addByteToWord(SP, s8); // add signed 8-bit value to SP
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_HL_SP_s8: {
                Byte s8 = loadByte(memory);
                Word SPadds8 = addByteToWord(SP, s8); // calculate SP + signed 8-bit value for flag updates
                HL = SP + SPadds8; // add signed 8-bit value to SP and store in HL
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // x9 opcodes
            case ADD_HL_BC: {
                addRegToReg(HL, BC);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_HL_DE: {
                addRegToReg(HL, DE);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_HL_HL: {
                addRegToReg(HL, HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADD_HL_SP: {
                addRegToReg(HL, SP);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_C_C: {
                cycles -= opcycles[opcode] - 1; // effectively NOP
                break;
            }
            case LD_E_C: {
                loadRegToReg(E, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_L_C: {
                loadRegToReg(L, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_C: {
                loadRegToReg(A, C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADC_A_C: {
                ADC(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SBC_A_C: {
                SBC(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case XOR_C: {
                xorRegToA(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case CP_C: {
                CP(C);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RET: {
                // todo check this
                popStackToReg(PC, memory);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case RETI: {
                // TODO
                break;
            }
            case JP_HL: {
                loadRegToReg(PC, HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_SP_HL: {
                loadRegToReg(SP, HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // xA opcodes
            case LD_A_BCmem: {
                loadRegToMemory(memory, BC, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_DEmem: {
                loadRegToMemory(memory, DE, A);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_HLi: {
                loadRegToMemory(memory, HL, A);
                HL = incWord(HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_HLd: {
                loadRegToMemory(memory, HL, A);
                HL = decWord(HL);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_C_D: {
                loadRegToReg(C, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_E_D: {
                loadRegToReg(E, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_L_D: {
                loadRegToReg(L, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case LD_A_D: {
                loadRegToReg(A, D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case ADC_A_D: {
                ADC(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case SBC_A_D: {
                SBC(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case XOR_D: {
                xorRegToA(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case CP_D: {
                CP(D);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JP_Z_a16: {
                bool z_flag = (F >> 7) & 1;
                Word a16 = loadWord(memory);
                jp(a16, z_flag == 1);
                cycles -= opcycles[opcode] - 1;
                break;
            }
            case JP_C_a16: {
                bool c_flag = (F >> 4) & 1;
                Word a16 = loadWord(memory);
                jp(a16, c_flag == 1);
                cycles -= opcycles[opcode] - 1;
                break;
            }

            // nested cases for unimplemented opcodes
            case 0xD3:
            case 0xDB:
            case 0xDD:
            case 0xE3:
            case 0xE4:
            case 0xEB:
            case 0xEC:
            case 0xED:
            case 0xF4:
            case 0xFC:
            case 0xFD: {
                // todo, implement a crash system for invalid opcodes
                break;
            }
            default:
                    std::cout << "Unknown opcode: 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
                    break;
        }
    }
};