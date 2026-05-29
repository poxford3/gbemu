#include <iostream>
#include <cstdint>
#include "cpu.hpp"
#include "types.hpp"
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

    IME = false;

    memory[P1] = 0xCF;
    memory[SB] = 0x0;
    memory[SC] = 0x7E;
    memory[DIV] = 0x18;
    memory[TIMA] = 0x0;
    memory[TMA] = 0x0;
    memory[TAC] = 0xF8;
    memory[IF] = 0xE1;
    memory[NR10] = 0x80;
    memory[NR11] = 0xBF;
    memory[NR12] = 0xF3;
    memory[NR13] = 0xFF;
    memory[NR14] = 0xBF;
    memory[NR21] = 0x3F;
    memory[NR22] = 0x0;
    memory[NR23] = 0xFF;
    memory[NR24] = 0xBF;
    memory[NR30] = 0x7F;
    memory[NR31] = 0xFF;
    memory[NR32] = 0x9F;
    memory[NR33] = 0xFF;
    memory[NR34] = 0xBF;
    memory[NR41] = 0xFF;
    memory[NR42] = 0x0;
    memory[NR43] = 0x0;
    memory[NR44] = 0xBF;
    memory[NR50] = 0x77;
    memory[NR51] = 0xF3;
    memory[NR52] = 0xF1;
    memory[LCDC] = 0x91;
    memory[STAT] = 0x81;
    memory[SCY] = 0x0;
    memory[SCX] = 0x0;
    memory[LY] = 0x91;
    memory[LYC] = 0x0;
    memory[DMA] = 0xFF;
    memory[BGP] = 0xFC;
    // memory[OBP0] = 0x??7;
    // memory[OBP1] = 0x??7;
    memory[WY] = 0x0;
    memory[WX] = 0x0;
    // memory[KEY0] = 0x—;
    // memory[KEY1] = 0x—;
    // memory[VBK] = 0x—;
    // memory[BANK] = 0x—;
    // memory[HDMA1] = 0x—;
    // memory[HDMA2] = 0x—;
    // memory[HDMA3] = 0x—;
    // memory[HDMA4] = 0x—;
    // memory[HDMA5] = 0x—;
    // memory[RP] = 0x—;
    // memory[BCPS] = 0x—;
    // memory[BCPD] = 0x—;
    // memory[OCPS] = 0x—;
    // memory[OCPD] = 0x—;
    // memory[SVBK] = 0x—;
    memory[IE] = 0x0;
}

void Cpu::loadProgram(std::vector<Byte> program, uint numBytes, Mem &memory) {
    // check for less than 0x8000 since that's the 32kb limit of cart rom
    for (uint i = 0; i < numBytes && i < 0x8000; i++) {
        memory[i] = program[i];
    }

    // printf("ROM header: %02X %02X %02X %02X %02X %02X %02X %02X\n", 
    // memory[0x0100], memory[0x0101], memory[0x0102], memory[0x0103],
    // memory[0x0104], memory[0x0105], memory[0x0106], memory[0x0107]);
    // printf("MBC type: %02X\n", memory[0x0147]);
    // printf("ROM size: %02X\n", memory[0x0148]);
};

void Cpu::runProgram(Mem &memory) {
    std::cout << "run" << std::endl;
    // while (PC < 0x0410) {
    while (1) {
        Byte opcode = loadByte(memory);

        uint cycles = 100;
        std::cout << "opcode: (0x" << std::hex << static_cast<int>(opcode) << "), F: 0x" << std::hex << static_cast<int>(F) << std::dec;
        std::printf(" PC: 0x%04X", PC);
        std::printf(" SP: 0x%04X\n", SP);
        executeInstructions(opcode, memory);
    }
};

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

void Cpu::TEST_showAllRegValuesDecimal() {
    Byte flags = F;
    std::cout << "Flags: Z=" << ((flags & 0x80) ? "1" : "0") << " N=" << ((flags & 0x40) ? "1" : "0") << " H=" << ((flags & 0x20) ? "1" : "0") << " C=" << ((flags & 0x10) ? "1" : "0") << std::endl;
    std::cout << "(A: " << static_cast<int>(A) 
              << ", F: " << static_cast<int>(F) << ")" << std::endl;
    std::cout << "(B: " << static_cast<int>(B) 
            << ", C: " << static_cast<int>(C) << ")" << std::endl;
    std::cout << "(D: " << static_cast<int>(D) 
              << ", E: " << static_cast<int>(E) << ")" << std::endl;
    std::cout << "(H: " << static_cast<int>(H) 
              << ", L: " << static_cast<int>(L) << ")" << std::endl;
    std::cout << "IME: " << IME << std::endl;
    std::cout << "SP: " << SP << std::endl;
    std::cout << "PC: " << PC << std::endl;
}

Byte Cpu::readByte(Mem &memory, Word address) {
    // if (address >= 0x4000 && address <= 0x7FFF) {
    //     return ROM[(currentROMBank * 0x4000) + (address - 0x4000)];
    // }
    Byte value = memory[address];
    return value;
}

Word Cpu::incWord(Word value) {
    return (value + 1) & 0xFFFF; // Increment and wrap around at 0xFFFF
}

Byte Cpu::incByte(Byte value) {
    bool currentCFlag = (F >> 4) & 1;
    bool halfCarry = (value & 0x0F) == 0x0F;
    value++;
    updateFlags(value, 0, halfCarry, currentCFlag);
    return value;
}

Word Cpu::decWord(Word value) {
    return (value - 1) & 0xFFFF; // Decrement and wrap around at 0x0000
}

Byte Cpu::decByte(Byte value) {
    bool currentCFlag = (F >> 4) & 1;
    bool halfCarry = (value & 0x0F) == 0;
    value--;
    updateFlags(value, 1, halfCarry, currentCFlag);
    return value;
}

void Cpu::jp(Word address) {
    // jump to address
    // std::cout << "jump to 0x" << std::hex << address << std::dec << std::endl;
    PC = address;
}

void Cpu::jr(int8_t offset) {
    // jump to PC + offset (offset is signed)
    PC += offset;
}

void Cpu::call(Word address, Mem &memory) {
    // Push current PC onto stack
    pushRegToStack(PC, memory);
    // Jump to the specified address
    PC = address;
}

void Cpu::handleInterrupt(Mem &memory) {
       
    Byte pending = memory[IE] & memory[IF];
    if (pending == 0) return;
    
    halted = false;
    if (!IME) {
        // printf("interrupt pending but IME=0, IE=%02X IF=%02X\n", memory[IE], memory[IF]);
        return;
    } // if the master interrupt says there are no interrupts, then we just move on
    IME = false;

    for (int i = 0; i < 5; i++) {
        if (pending & (1 << i)) {
            // printf("handling interrupt %d, PC=%04X IME=%d\n", i, PC, IME);
            memory[IF] &= ~(1 << i);
            pushRegToStack(PC, memory);

            switch (i) {
                case 0: PC = 0x0040; break; // VBlank
                case 1: PC = 0x0048; break; // LCD
                case 2: PC = 0x0050; break; // Timer
                case 3: PC = 0x0058; break; // Serial
                case 4: PC = 0x0060; break; // Joypad
            }
            // cycles -= 20; // implemented once the clock is done
            return;
        }
    }
}

void Cpu::_RET(Mem &memory) {
    // Pop return address from stack and jump to it
    popStackToReg(PC, memory);
}

void Cpu::_EI() {
    // IME = true;
    pendingIME = true;
}

void Cpu::_DI() {
    IME = false;
    pendingIME = false;
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

int8_t Cpu::loadInt(Mem &memory) {
    int8_t value = memory[PC];
    // printf("PC: %20X, IE: %20X\n", PC, memory[IE]);
    PC++;
    return value;
}

void Cpu::loadRegToReg(Byte &dest, Byte &src) {
    dest = src;
}

void Cpu::loadRegToReg(Word &dest, Word &src) {
    dest = src;
}

void Cpu::loadRegToMemory(Mem &memory, Word address, Byte &reg) {
    // aka writeByte
    // if (address == 0xFF02) {
    //     printf("SC write: %02X SB: %02X PC: %04X\n", reg, memory[0xFF01], PC);
    // }
    // if (address >= 0x2000 && address <= 0x3FFF) {
    //     // MBC1 ROM bank number (lower 5 bits)
    //     Byte bank = value & 0x1F;
    //     if (bank == 0) bank = 1; // bank 0 is not selectable here
    //     currentROMBank = bank;
    //     return;
    // }
    memory[address] = reg;
    if (memory[0xFF02] == 0x81) {
        char c = memory[0xFF01];
        printf("%c", c);
        fflush(stdout);
        memory[0xFF02] = 0x00;
    }
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

void Cpu::_RRA(std::optional<bool> throughCarry) {
    bool oldBit0 = A & 1; // Get the old bit 0
    A >>= 1; // Shift right by 1
    if (throughCarry.has_value() && throughCarry.value()) {
        A |= (F & 0x10) << 3; // Rotate through carry: old bit 0 goes to carry, and old carry goes to bit 7
    } else {
        A |= oldBit0 ? 0x80 : 0; // Rotate: old bit 0 goes to bit 7
    }
    // RR(C)A affects the Z flag directly to zero by passing in any value BUT zero
    updateFlags(0x01, false, false, oldBit0); // Z flag is set based on result, N H flags are reset, C flag is updated above
}

void Cpu::_RLA(std::optional<bool> throughCarry) {
    bool oldBit7 = (A >> 7) & 1; // Get the old bit 7
    A <<= 1; // Shift left by 1
    if (throughCarry.has_value() && throughCarry.value()) {
        A |= (F & 0x10) >> 4; // Rotate through carry: old bit 7 goes to carry, and old carry goes to bit 0 (RL)
    } else {
        A |= oldBit7 ? 1 : 0; // Rotate: old bit 7 goes to bit 0 (RLC)
    }
    // RL(C)A affects Z flag directly to zero by passing in any value BUT zero
    updateFlags(0x01, false, false, oldBit7); // Z flag is set to 0 always (passing in nonzero to ensure), N H flags are reset, C flag is updated above
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
    updateFlags(value, false, false, oldBit7); // Z flag is set based on result, N H flags are reset, C flag is updated above
}

void Cpu::rotateRight(Byte &value, std::optional<bool> throughCarry) {
    bool oldBit0 = value & 1; // Get the old bit 0
    value >>= 1; // Shift right by 1
    if (throughCarry.has_value() && throughCarry.value()) {
        value |= (F & 0x10) << 3; // Rotate through carry: old bit 0 goes to carry, and old carry goes to bit 7
    } else {
        value |= oldBit0 ? 0x80 : 0; // Rotate: old bit 0 goes to bit 7
    }
    updateFlags(value, false, false, oldBit0); // Z flag is set based on result, N H flags are reset, C flag is updated above
}

void Cpu::shiftLeft(Byte &value) {
    bool oldBit7 = (value >> 7) & 1; // Get the old bit 7
    value <<= 1; // Shift left by 1
    updateFlags(value, false, false, oldBit7); // Z flag is set based on result, N H flags are reset, C flag is updated above
}

void Cpu::shiftRight(Byte &value, bool arithmetic) {
    // arithmetic == SRA, logical == SRL
    bool oldBit0 = value & 1; // Get the old bit 0
    if (arithmetic) {
        // For arithmetic shift right, we need to preserve the sign bit (bit 7)
        value = (value >> 1) | (value & 0x80); // Shift right and preserve bit 7
    } else {
        value >>= 1; // Logical shift right
    }
    updateFlags(value, false, false, oldBit0); // Z flag is set based on result, N H flags are reset, C flag is updated above
}

void Cpu::swapNibbles(Byte &value) {
    value = (value << 4) | (value >> 4); // Swap upper and lower nibbles
    updateFlags(value, false, false, false); // Z flag is set based on result, N H C flags are reset
}

void Cpu::bit(Byte &value, int bit) {
    // bit test
    bool bitValue = (value >> bit) & 1; // Get the value of the specified bit
    bool currentCFlag = (F >> 4) & 1; // keep C flag the same
    updateFlags(bitValue, false, true, currentCFlag); // Z flag is set based on bit value, N flag is reset, H flag is set
}

void Cpu::res(Byte &value, int bit) {
    // reset bit
    value &= ~(1 << bit); // Clear the specified bit
}

void Cpu::set(Byte &value, int bit) {
    // set bit
    value |= 1 << bit; // Set the specified bit
}


void Cpu::addRegToReg(Byte &dest, Byte &src) {
    bool flag_h = ((dest & 0x0F) + (src & 0x0F)) > 0x0F; // Set H flag if there is a carry from bit 3
    bool flag_c = (dest + src) > 0xFF; // Set C flag if there is a carry from bit 7
    dest += src;
    updateFlags(dest, false, flag_h, flag_c);
}

void Cpu::addRegToReg(Word &dest, Word &src) {
    bool flag_h = ((dest & 0x0FFF) + (src & 0x0FFF)) > 0x0FFF; // carry from bit 11
    // case dest and src to uint to make sure they don't wrap around before checking carry
    bool flag_c = ((uint)dest + (uint)src) > 0xFFFF; // Set C flag if there is a carry from bit 15
    dest += src;
    bool currentZFlag = (F >> 7) & 1;
    Byte resultZ = currentZFlag ? 0x00: 0x01;
    updateFlags(resultZ, false, flag_h, flag_c);
}

Word Cpu::addByteToWord(Word &dest, int8_t src) {
    uint8_t usrc = (uint8_t)src; // treat as unsigned for flag checks
    bool flag_h = ((dest & 0x0F) + (usrc & 0x0F)) > 0x0F;
    bool flag_c = ((dest & 0xFF) + usrc) > 0xFF;
    Word result = dest + src; // keep signed for correct result
    updateFlags(1, false, flag_h, flag_c);
    return result;
}

void Cpu::subRegToReg(Byte &dest, Byte &src) {
    bool flag_h = (dest & 0x0F) < (src & 0x0F); // Set H flag if there is a borrow from bit 4
    bool flag_c = dest < src; // Set C flag if there is a borrow from bit 8
    dest -= src;
    updateFlags(dest, true, flag_h, flag_c);
}


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

void Cpu::_DAA(Byte &A) {
    // https://www.reddit.com/r/EmuDev/comments/cdtuyw/comment/etwcyvy/
    bool currentZFlag = (F >> 7) & 1;
    bool currentNFlag = (F >> 6) & 1;
    bool currentHFlag = (F >> 5) & 1;
    bool currentCFlag = (F >> 4) & 1;

    if (!currentNFlag) {
        if (currentCFlag || A > 0x99) {
            A += 0x60;
            currentCFlag = 1;
        }
        if (currentHFlag || (A & 0x0F) > 0x09) {
            A += 0x6;
        }
    } else {
        if (currentCFlag) {
            A -= 0x60;
            currentCFlag = 1;
        }
        if (currentHFlag) {
            A -= 0x6;
        }
    }
    // H flag always cleared
    updateFlags(A, currentNFlag, false, currentCFlag);
}


void Cpu::popStackToReg(Word &reg, Mem &memory) {
    reg = memory[SP] | (memory[SP + 1] << 8); // Pop low byte and high byte from stack
    SP += 2; // Increment stack pointer by 2
}

void Cpu::pushRegToStack(Word reg, Mem &memory) {
    --SP;
    memory[SP] = (reg >> 8) & 0xFF; // Push high byte onto stack
    --SP;
    memory[SP] = reg & 0xFF; // Push low byte onto stack
}


uint Cpu::executeInstructions(Byte opcode, Mem &memory) {

    uint cycles;
    switch (opcode) {
        // x0 opcodes
        case NOP: {
            // std::cout << "Executed NOP" << std::endl;
            cycles = opcycles[opcode];
            break;
        }
        case STOP: {
            // TODO, will be clock related
            // std::cout << "Executed STOP" << std::endl;
            cycles = opcycles[opcode];
            break;
        }
        case JR_NZ_s8: {
            bool currentZFlag = (F >> 7) & 1;
            int8_t s8 = loadInt(memory);
            if (currentZFlag == 0) {
                jr(s8);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 2
                cycles = 2;
            }
            break;
        }
        case JR_NC_s8: {
            bool currentCFlag = (F >> 4) & 1;
            int8_t s8 = loadInt(memory);
            if (currentCFlag == 0) {
                jr(s8);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 2
                cycles = 2;
            }
            break;
        }
        case LD_B_B: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_D_B: {
            loadRegToReg(D, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_B: {
            loadRegToReg(H, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_B: {
            loadRegToMemory(memory, HL, B);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_B: {
            addRegToReg(A, B);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_B: {
            subRegToReg(A, B);
            cycles = opcycles[opcode];
            break;
        }
        case AND_B: {
            andRegToA(B);
            cycles = opcycles[opcode];
            break;
        }
        case OR_B: {
            orRegToA(B);
            cycles = opcycles[opcode];
            break;
        }
        case RET_NZ: {
            bool currentZFlag = (F >> 7) & 1;
            if (currentZFlag == 0) {
                _RET(memory);
                cycles = opcycles[opcode];
            } else {
                cycles = 2;
            }
            break;
        }
        case RET_NC: {
            bool currentCFlag = (F >> 4) & 1;
            if (currentCFlag == 0) {
                _RET(memory);
                cycles = opcycles[opcode];
            } else {
                cycles = 2;
            }
            break;
        }
        case LD_a8mem_A: {
            Byte a8 = loadByte(memory);
            Word a8MemPadded = 0xFF00 + a8;
            loadRegToMemory(memory, a8MemPadded, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_a8mem: {
            Byte a8 = loadByte(memory);
            Word a8address = 0xFF00 + a8; // only the lower half of the address is provided, opcode always between 0xFF00 - 0xFFFF
            // std::cout << std::hex << a8address << std::dec << std::endl;
            loadRegFromMemory(memory, a8address, A);
            cycles = opcycles[opcode];
            break;
        }

        // x1 opcodes
        case LD_BC_d16: {
            BC = loadWord(memory);
            cycles = opcycles[opcode];
            break;
        }
        case LD_DE_d16: {
            DE = loadWord(memory);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HL_d16: {
            HL = loadWord(memory);
            cycles = opcycles[opcode];
            break;
        }
        case LD_SP_d16: {
            SP = loadWord(memory);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_C: {
            loadRegToReg(B, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_C: {
            loadRegToReg(D, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_C: {
            loadRegToReg(H, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_C: {
            loadRegToMemory(memory, HL, C);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_C: {
            addRegToReg(A, C);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_C: {
            subRegToReg(A, C);
            cycles = opcycles[opcode];
            break;
        }
        case AND_C: {
            andRegToA(C);
            cycles = opcycles[opcode];
            break;
        }
        case OR_C: {    
            orRegToA(C);
            cycles = opcycles[opcode];
            break;
        }
        case POP_BC: {
            popStackToReg(BC, memory);
            cycles = opcycles[opcode];
            break;
        }
        case POP_DE: {
            popStackToReg(DE, memory);
            cycles = opcycles[opcode];
            break;
        }
        case POP_HL: {
            popStackToReg(HL, memory);
            cycles = opcycles[opcode];
            break;
        }
        case POP_AF: {
            popStackToReg(AF, memory);
            AF &= 0xFFF0; // mask lower nibble of F
            cycles = opcycles[opcode];
            break;
        }

        // x2 opcodes
        case LD_BCmem_A: {
            loadRegToMemory(memory, BC, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_DEmem_A: {
            loadRegToMemory(memory, DE, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLi_A: {
            loadRegToMemory(memory, HL, A);
            HL = incWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLd_A: {
            loadRegToMemory(memory, HL, A);
            HL = decWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_D: {
            loadRegToReg(B, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_D: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_H_D: {
            loadRegToReg(H, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_D: {
            loadRegToMemory(memory, HL, D);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_D: {
            addRegToReg(A, D);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_D: {
            subRegToReg(A, D);
            cycles = opcycles[opcode];
            break;
        }
        case AND_D: {
            andRegToA(D);
            cycles = opcycles[opcode];
            break;
        }
        case OR_D: {
            orRegToA(D);
            cycles = opcycles[opcode];
            break;
        }
        case JP_NZ_a16: {
            bool currentZFlag = (F >> 7) & 1;
            Word a16 = loadWord(memory);
            if (currentZFlag == 0) {
                jp(a16);
                cycles = opcycles[opcode];
            } else {
                // cycles go from 6 to 3 if condition not met, because we still need to read the a16 operand but we don't jump
                cycles = 3;
            }
            break;
        }
        case JP_NC_a16: {
            bool currentCFlag = (F >> 4) & 1;
            Word a16 = loadWord(memory);
            if (currentCFlag == 0) {
                jp(a16);
                cycles = opcycles[opcode];
            } else {
                cycles = 3;
            }
            break;
        }
        case LD_Cmem_A: {
            Word cMemPadded = 0xFF00 + C;
            loadRegToMemory(memory, cMemPadded, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_Cmem: {
            Word cMemAddress = 0xFF00 + C; // only the lower half of the address is provided, opcode always between 0xFF00 - 0xFFFF
            loadRegFromMemory(memory, cMemAddress, A);
            cycles = opcycles[opcode];
            break;
        }

        // x3 opcodes
        case INC_BC: {
            BC = incWord(BC);
            cycles = opcycles[opcode];
            break;
        }
        case INC_DE: {
            DE = incWord(DE);
            cycles = opcycles[opcode];
            break;
        }
        case INC_HL: {
            HL = incWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case INC_SP: {
            SP = incWord(SP);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_E: {
            loadRegToReg(B, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_E: {
            loadRegToReg(D, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_E: {
            loadRegToReg(H, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_E: {
            loadRegToMemory(memory, HL, E);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_E: {
            addRegToReg(A, E);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_E: {
            subRegToReg(A, E);
            cycles = opcycles[opcode];
            break;
        }
        case AND_E: {
            andRegToA(E);
            cycles = opcycles[opcode];
            break;
        }
        case OR_E: {
            orRegToA(E);
            cycles = opcycles[opcode];
            break;
        }
        case JP_a16: {
            Word a16 = loadWord(memory);
            jp(a16);
            cycles = opcycles[opcode];
            break;
        }
        case DI: {
            _DI();
            cycles = opcycles[opcode];
            break;
        }

        // x4 opcodes
        case INC_B: {
            B = incByte(B);
            cycles = opcycles[opcode];
            break;
        }
        case INC_D: {
            D = incByte(D);
            cycles = opcycles[opcode];
            break;
        }
        case INC_H: {
            H = incByte(H);
            cycles = opcycles[opcode];
            break;
        }
        case INC_HLmem: {
            // load byte from memory, increment it, then store it back
            Byte value = readByte(memory, HL);
            value = incByte(value);
            loadRegToMemory(memory, HL, value);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_H: {
            loadRegToReg(B, H);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_H: {
            loadRegToReg(D, H);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_H: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_HLmem_H: {
            loadRegToMemory(memory, HL, H);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_H: {
            addRegToReg(A, H);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_H: {
            subRegToReg(A, H);
            cycles = opcycles[opcode];
            break;
        }
        case AND_H: {
            andRegToA(H);
            cycles = opcycles[opcode];
            break;
        }
        case OR_H: {
            orRegToA(H);
            cycles = opcycles[opcode];
            break;
        }
        case CALL_NZ_a16: {
            bool currentZFlag = (F >> 7) & 1;
            Word a16 = loadWord(memory);
            if (currentZFlag == 0) {
                call(a16, memory);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 3
                cycles = 3;
            }
            break;
        }
        case CALL_NC_a16: {
            bool currentCFlag = (F >> 4) & 1;
            Word a16 = loadWord(memory);
            if (currentCFlag == 0) {
                call(a16, memory);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 3
                cycles = 3;
            }
            break;
        }

        // x5 opcodes
        case DEC_B: {
            B = decByte(B);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_D: {
            D = decByte(D);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_H: {
            H = decByte(H);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_HLmem: {
            // load byte from memory, decrement it, then store it back
            Byte value = readByte(memory, HL);
            value = decByte(value);
            loadRegToMemory(memory, HL, value);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_L: {
            loadRegToReg(B, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_L: {
            loadRegToReg(D, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_L: {
            loadRegToReg(H, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_L: {
            loadRegToMemory(memory, HL, L);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_L: {
            addRegToReg(A, L);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_L: {
            subRegToReg(A, L);
            cycles = opcycles[opcode];
            break;
        }
        case AND_L: {
            andRegToA(L);
            cycles = opcycles[opcode];
            break;
        }
        case OR_L: {
            orRegToA(L);
            cycles = opcycles[opcode];
            break;
        }
        case PUSH_BC: {
            pushRegToStack(BC, memory);
            cycles = opcycles[opcode];
            break;
        }
        case PUSH_DE: {
            pushRegToStack(DE, memory);
            cycles = opcycles[opcode];
            break;
        }
        case PUSH_HL: {
            pushRegToStack(HL, memory);
            cycles = opcycles[opcode];
            break;
        }
        case PUSH_AF: {
            pushRegToStack(AF, memory);
            cycles = opcycles[opcode];
            break;
        }

        // x6 opcodes
        case LD_B_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(B, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(D, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(H, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_d8: {
            Byte d8 = loadByte(memory);
            loadRegToMemory(memory, HL, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_HLmem: {
            loadRegFromMemory(memory, HL, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_HLmem: {
            loadRegFromMemory(memory, HL, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_HLmem: {
            loadRegFromMemory(memory, HL, H);
            cycles = opcycles[opcode];
            break;
        }
        case HALT: {
            halted = true;
            paused = true;
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_HLmem: {
            Byte value = readByte(memory, HL);
            addRegToReg(A, value);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_HLmem: {
            Byte value = readByte(memory, HL);
            subRegToReg(A, value);
            cycles = opcycles[opcode];
            break;
        }
        case AND_HLmem: {
            Byte value = readByte(memory, HL);
            andRegToA(value);
            cycles = opcycles[opcode];
            break;
        }
        case OR_HLmem: {
            Byte value = readByte(memory, HL);
            orRegToA(value);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_d8: {
            Byte d8 = loadByte(memory);
            addRegToReg(A, d8);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_d8: {
            Byte d8 = loadByte(memory);
            subRegToReg(A, d8);
            cycles = opcycles[opcode];
            break;
        }
        case AND_d8: {
            Byte d8 = loadByte(memory);
            andRegToA(d8);
            cycles = opcycles[opcode];
            break;
        }
        case OR_d8: {
            Byte d8 = loadByte(memory);
            orRegToA(d8);
            cycles = opcycles[opcode];
            break;
        }

        // x7 opcodes
        case RLCA: {
            _RLA(false);
            cycles = opcycles[opcode];
            break;
        }
        case RLA: {
            _RLA(true);
            cycles = opcycles[opcode];
            break;
        }
        case DAA: {
            _DAA(A);
            cycles = opcycles[opcode];
            break;
        }
        case SCF: {
            Byte currentZFlag = (F >> 7) & 1; // Store current Z flag value
            Byte resultZ = currentZFlag ? 0x00: 0x01; // if current zflag is 1, resultZ zero to set it to one
            updateFlags(resultZ, false, false, true); // Set C flag, reset N and H flags, Z flag is unaffected
            cycles = opcycles[opcode];
            break;
        }
        case LD_B_A: {
            loadRegToReg(B, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_D_A: {
            loadRegToReg(D, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_H_A: {
            loadRegToReg(H, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_HLmem_A: {
            loadRegToMemory(memory, HL, A);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_A_A: {
            addRegToReg(A, A);
            cycles = opcycles[opcode];
            break;
        }
        case SUB_A: {
            subRegToReg(A, A);
            cycles = opcycles[opcode];
            break;
        }
        case AND_A: {
            andRegToA(A);
            cycles = opcycles[opcode];
            break;
        }
        case OR_A: {
            orRegToA(A);
            cycles = opcycles[opcode];
            break;
        }
        case RST_0: {
            // memory locations 
            // 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, and 0x38 reserved for RST instructions
            // RST 0-7
            RST(0x00, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_2: {
            RST(0x10, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_4: {
            RST(0x20, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_6: {
            RST(0x30, memory);
            cycles = opcycles[opcode];
            break;
        }

        // x8 opcodes
        case LD_a16mem_SP: {
            Word a16 = loadWord(memory);
            // printf("saving SP %04X to address %04X\n", SP, a16);
            Byte SP_low = SP & 0xFF;
            Byte SP_high = (SP >> 8) & 0xFF;
            loadRegToMemory(memory, a16, SP_low);
            loadRegToMemory(memory, a16 + 1, SP_high);
            cycles = opcycles[opcode];
            break;
        }
        case JR_s8: {
            int8_t s8 = loadInt(memory);
            jr(s8);
            cycles = opcycles[opcode];
            break;
        }
        case JR_Z_s8: {
            bool currentZFlag = (F >> 7) & 1;
            int8_t s8 = loadInt(memory);
            if (currentZFlag == 1) {
                jr(s8);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 2
                cycles = 2;
            }
            break;
        }
        case JR_C_s8: {
            bool currentCFlag = (F >> 4) & 1;
            int8_t s8 = loadInt(memory);
            if (currentCFlag == 1) {
                jr(s8);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 2
                cycles = 2;
            }
            break;
        }
        case LD_C_B: {
            loadRegToReg(C, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_B: {
            loadRegToReg(E, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_B: {
            loadRegToReg(L, B);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_B: {
            loadRegToReg(A, B);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_B: {
            ADC(B);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_B: {
            SBC(B);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_B: {
            xorRegToA(B);
            cycles = opcycles[opcode];
            break;
        }
        case CP_B: {
            CP(B);
            cycles = opcycles[opcode];
            break;
        }
        case RET_Z: {
            bool currentZFlag = (F >> 7) & 1;
            if (currentZFlag == 1) {
                _RET(memory);
                cycles = opcycles[opcode];
            } else {
                cycles = 2;
            }
            break;
        }
        case RET_C: {
            bool currentCFlag = (F >> 4) & 1;
            if (currentCFlag == 1) {
                _RET(memory);
                cycles = opcycles[opcode];
            } else {
                cycles = 2;
            }
            break;
        }
        case ADD_SP_s8: {
            int8_t s8 = loadInt(memory);
            SP = addByteToWord(SP, s8); // add signed 8-bit value to SP
            cycles = opcycles[opcode];
            break;
        }
        case LD_HL_SP_s8: {
            int8_t s8 = loadInt(memory);
            Word SPadds8 = addByteToWord(SP, s8); // calculate SP + signed 8-bit value for flag updates
            loadRegToReg(HL, SPadds8);
            cycles = opcycles[opcode];
            break;
        }

        // x9 opcodes
        case ADD_HL_BC: {
            addRegToReg(HL, BC);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_HL_DE: {
            addRegToReg(HL, DE);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_HL_HL: {
            addRegToReg(HL, HL);
            cycles = opcycles[opcode];
            break;
        }
        case ADD_HL_SP: {
            addRegToReg(HL, SP);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_C: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_E_C: {
            loadRegToReg(E, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_C: {
            loadRegToReg(L, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_C: {
            loadRegToReg(A, C);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_C: {
            ADC(C);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_C: {
            SBC(C);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_C: {
            xorRegToA(C);
            cycles = opcycles[opcode];
            break;
        }
        case CP_C: {
            CP(C);
            cycles = opcycles[opcode];
            break;
        }
        case RET: {
            _RET(memory);
            cycles = opcycles[opcode];
            break;
        }
        case RETI: {
            _RET(memory);
            _EI();
            cycles = opcycles[opcode];
            break;
        }
        case JP_HL: {
            loadRegToReg(PC, HL);
            cycles = opcycles[opcode];
            break;
        }
        case LD_SP_HL: {
            loadRegToReg(SP, HL);
            cycles = opcycles[opcode];
            break;
        }

        // xA opcodes
        case LD_A_BCmem: {
            loadRegFromMemory(memory, BC, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_DEmem: {
            loadRegFromMemory(memory, DE, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_HLi: {
            loadRegFromMemory(memory, HL, A);
            HL = incWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_HLd: {
            loadRegFromMemory(memory, HL, A);
            HL = decWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_D: {
            loadRegToReg(C, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_D: {
            loadRegToReg(E, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_D: {
            loadRegToReg(L, D);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_D: {
            loadRegToReg(A, D);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_D: {
            ADC(D);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_D: {
            SBC(D);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_D: {
            xorRegToA(D);
            cycles = opcycles[opcode];
            break;
        }
        case CP_D: {
            CP(D);
            cycles = opcycles[opcode];
            break;
        }
        case JP_Z_a16: {
            bool currentZFlag = (F >> 7) & 1;
            Word a16 = loadWord(memory);
            if (currentZFlag == 1) {
                jp(a16);
                cycles = opcycles[opcode];
            } else {
                // cycles go from 6 to 3 if condition not met, because we still need to read the a16 operand but we don't jump
                cycles = 3;
            }
            break;
        }
        case JP_C_a16: {
            bool currentCFlag = (F >> 4) & 1;
            Word a16 = loadWord(memory);
            if (currentCFlag == 1) {
                jp(a16);
                cycles = opcycles[opcode];
            } else {
                // cycles go from 6 to 3 if condition not met, because we still need to read the a16 operand but we don't jump
                cycles = 3;
            }
            break;
        }
        case LD_a16mem_A: {
            Word a16 = loadWord(memory);
            loadRegToMemory(memory, a16, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_a16mem: {
            Word a16 = loadWord(memory);
            loadRegFromMemory(memory, a16, A);
            cycles = opcycles[opcode];
            break;
        }

        // xB opcodes
        case DEC_BC: {
            BC = decWord(BC);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_DE: {
            DE = decWord(DE);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_HL: {
            HL = decWord(HL);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_SP: {
            SP = decWord(SP);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_E: {
            loadRegToReg(C, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_E: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_L_E: {
            loadRegToReg(L, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_E: {
            loadRegToReg(A, E);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_E: {
            ADC(E);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_E: {
            SBC(E);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_E: {
            xorRegToA(E);
            cycles = opcycles[opcode];
            break;
        }
        case CP_E: {
            CP(E);
            cycles = opcycles[opcode];
            break;
        }
        case 0xCB: {
            Byte cbOpcode = loadByte(memory);
            cycles = executeExtendedOpcode(cbOpcode, memory);
            break;
        }
        case EI: {
            _EI();
            cycles = opcycles[opcode];
            break;
        }

        // xC opcodes
        case INC_C: {
            C = incByte(C);
            cycles = opcycles[opcode];
            break;
        }
        case INC_E: {
            E = incByte(E);
            cycles = opcycles[opcode];
            break;
        }
        case INC_L: {
            L = incByte(L);
            cycles = opcycles[opcode];
            break;
        }
        case INC_A: {
            A = incByte(A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_H: {
            loadRegToReg(C, H);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_H: {
            loadRegToReg(E, H);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_H: {
            loadRegToReg(L, H);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_H: {
            loadRegToReg(A, H);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_H: {
            ADC(H);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_H: {
            SBC(H);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_H: {
            xorRegToA(H);
            cycles = opcycles[opcode];
            break;
        }
        case CP_H: {
            CP(H);
            cycles = opcycles[opcode];
            break;
        }
        case CALL_Z_a16: {
            bool currentZFlag = (F >> 7) & 1;
            Word a16 = loadWord(memory);
            if (currentZFlag == 1) {
                call(a16, memory);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 3
                cycles = 3;
            }
            break;
        }
        case CALL_C_a16: {
            bool currentCFlag = (F >> 4) & 1;
            Word a16 = loadWord(memory);
            if (currentCFlag == 1) {
                call(a16, memory);
                cycles = opcycles[opcode];
            } else {
                // if condition not met, number of cycles required goes to 3
                cycles = 3;
            }
            break;
        }

        // xD opcodes
        case DEC_C: {
            C = decByte(C);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_E: {
            E = decByte(E);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_L: {
            L = decByte(L);
            cycles = opcycles[opcode];
            break;
        }
        case DEC_A: {
            A = decByte(A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_L: {
            loadRegToReg(C, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_L: {
            loadRegToReg(E, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_L: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case LD_A_L: {
            loadRegToReg(A, L);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_L: {
            ADC(L);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_L: {
            SBC(L);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_L: {
            xorRegToA(L);
            cycles = opcycles[opcode];
            break;
        }
        case CP_L: {
            CP(L);
            cycles = opcycles[opcode];
            break;
        }
        case CALL_a16: {
            Word a16 = loadWord(memory);
            call(a16, memory);
            cycles = opcycles[opcode];
            break;
        }

        // xE opcodes
        case LD_C_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(C, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(E, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(L, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_d8: {
            Byte d8 = loadByte(memory);
            loadRegToReg(A, d8);
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_HLmem: {
            loadRegFromMemory(memory, HL, C);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_HLmem: {
            loadRegFromMemory(memory, HL, E);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_HLmem: {
            loadRegFromMemory(memory, HL, L);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_HLmem: {
            loadRegFromMemory(memory, HL, A);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_HLmem: {
            Byte value = readByte(memory, HL);
            ADC(value);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_HLmem: {
            Byte value = readByte(memory, HL);
            SBC(value);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_HLmem: {
            Byte value = readByte(memory, HL);
            xorRegToA(value);
            cycles = opcycles[opcode];
            break;
        }
        case CP_HLmem: {
            Byte value = readByte(memory, HL);
            CP(value);
            cycles = opcycles[opcode];
            break;
        }
        case ADC_A_d8: {
            Byte d8 = loadByte(memory);
            ADC(d8);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_d8: {
            Byte d8 = loadByte(memory);
            SBC(d8);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_d8: {
            Byte d8 = loadByte(memory);
            xorRegToA(d8);
            cycles = opcycles[opcode];
            break;
        }
        case CP_d8: {
            Byte d8 = loadByte(memory);
            CP(d8);
            cycles = opcycles[opcode];
            break;
        }
        
        // xF opcodes
        case RRCA: {
            _RRA(false);
            cycles = opcycles[opcode];
            break;
        }
        case RRA: {
            _RRA(true);
            cycles = opcycles[opcode];
            break;
        }
        case CPL: {
            A = ~A; // compliment(A)
            Byte currentZFlag = (F >> 7) & 1; // Store current Z flag value
            bool currentCFlag = (F >> 4) & 1; // Store current C flag value
            Byte resultZ = currentZFlag ? 0x00: 0x01;
            updateFlags(resultZ, true, true, currentCFlag); // Toggle C flag, reset N and H flags, Z flag is unaffected
            cycles = opcycles[opcode];
            break;
        }
        case CCF: {
            Byte currentZFlag = (F >> 7) & 1; // Store current Z flag value
            bool currentCFlag = (F >> 4) & 1; // Store current C flag value
            Byte resultZ = currentZFlag ? 0x00: 0x01;
            updateFlags(resultZ, false, false, !currentCFlag); // Toggle C flag, reset N and H flags, Z flag is unaffected
            cycles = opcycles[opcode];
            break;
        }
        case LD_C_A: {
            loadRegToReg(C, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_E_A: {
            loadRegToReg(E, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_L_A: {
            loadRegToReg(L, A);
            cycles = opcycles[opcode];
            break;
        }
        case LD_A_A: {
            cycles = opcycles[opcode]; // effectively NOP
            break;
        }
        case ADC_A_A: {
            ADC(A);
            cycles = opcycles[opcode];
            break;
        }
        case SBC_A_A: {
            SBC(A);
            cycles = opcycles[opcode];
            break;
        }
        case XOR_A: {
            xorRegToA(A);
            cycles = opcycles[opcode];
            break;
        }
        case CP_A: {
            CP(A);
            cycles = opcycles[opcode];
            break;
        }
        case RST_1: {
            RST(0x08, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_3: {
            RST(0x18, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_5: {
            RST(0x28, memory);
            cycles = opcycles[opcode];
            break;
        }
        case RST_7: {
            RST(0x38, memory);
            cycles = opcycles[opcode];
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
            std::cout << "illegal opcode" << std::endl;
            break;
        }
        default:
                std::cout << "Unknown opcode: 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
                break;
    }
    return cycles;
        // }
};

uint Cpu::executeExtendedOpcode(Byte opcode, Mem &memory) {
    uint exCycles;
    switch (opcode) {
        // x0 extended opcodes
        case RLC_B: {
            rotateLeft(B, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_B: {
            rotateLeft(B, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_B: {
            shiftLeft(B);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_B: {
            swapNibbles(B);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_B: {
            bit(B, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_B: {
            bit(B, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_B: {
            bit(B, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_B: {
            bit(B, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_B: {
            res(B, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_B: {
            res(B, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_B: {
            res(B, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_B: {
            res(B, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_B: {
            set(B, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_B: {
            set(B, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_B: {
            set(B, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_B: {
            set(B, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x1 extended opcodes
        case RLC_C: {
            rotateLeft(C, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_C: {
            rotateLeft(C, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_C: {
            shiftLeft(C);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_C: {
            swapNibbles(C);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_C: {
            bit(C, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_C: {
            bit(C, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_C: {
            bit(C, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_C: {
            bit(C, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_C: {
            res(C, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_C: {
            res(C, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_C: {
            res(C, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_C: {
            res(C, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_C: {
            set(C, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_C: {
            set(C, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_C: {
            set(C, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_C: {
            set(C, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x2 extended opcodes
        case RLC_D: {
            rotateLeft(D, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_D: {
            rotateLeft(D, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_D: {
            shiftLeft(D);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_D: {
            swapNibbles(D);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_D: {
            bit(D, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_D: {
            bit(D, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_D: {
            bit(D, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_D: {
            bit(D, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_D: {
            res(D, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_D: {
            res(D, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_D: {
            res(D, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_D: {
            res(D, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_D: {
            set(D, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_D: {
            set(D, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_D: {
            set(D, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_D: {
            set(D, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x3 extended opcodes
        case RLC_E: {
            rotateLeft(E, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_E: {
            rotateLeft(E, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_E: {
            shiftLeft(E);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_E: {
            swapNibbles(E);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_E: {
            bit(E, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_E: {
            bit(E, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_E: {
            bit(E, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_E: {
            bit(E, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_E: {
            res(E, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_E: {
            res(E, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_E: {
            res(E, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_E: {
            res(E, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_E: {
            set(E, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_E: {
            set(E, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_E: {
            set(E, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_E: {
            set(E, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x4 extended opcodes
        case RLC_H: {
            rotateLeft(H, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_H: {
            rotateLeft(H, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_H: {
            shiftLeft(H);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_H: {
            swapNibbles(H);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_H: {
            bit(H, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_H: {
            bit(H, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_H: {
            bit(H, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_H: {
            bit(H, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_H: {
            res(H, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_H: {
            res(H, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_H: {
            res(H, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_H: {
            res(H, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_H: {
            set(H, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_H: {
            set(H, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_H: {
            set(H, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_H: {
            set(H, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x5 extended opcodes
        case RLC_L: {
            rotateLeft(L, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_L: {
            rotateLeft(L, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_L: {
            shiftLeft(L);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_L: {
            swapNibbles(L);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_L: {
            bit(L, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_L: {
            bit(L, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_L: {
            bit(L, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_L: {
            bit(L, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_L: {
            res(L, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_L: {
            res(L, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_L: {
            res(L, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_L: {
            res(L, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_L: {
            set(L, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_L: {
            set(L, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_L: {
            set(L, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_L: {
            set(L, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x6 extended opcodes
        case RLC_HLmem: {
            rotateLeft(memory[HL], false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_HLmem: {
            rotateLeft(memory[HL], true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_HLmem: {
            shiftLeft(memory[HL]);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_HLmem: {
            swapNibbles(memory[HL]);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_HLmem: {
            bit(memory[HL], 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_HLmem: {
            bit(memory[HL], 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_HLmem: {
            bit(memory[HL], 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_HLmem: {
            bit(memory[HL], 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_HLmem: {
            res(memory[HL], 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_HLmem: {
            res(memory[HL], 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_HLmem: {
            res(memory[HL], 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_HLmem: {
            res(memory[HL], 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_HLmem: {
            set(memory[HL], 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_HLmem: {
            set(memory[HL], 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_HLmem: {
            set(memory[HL], 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_HLmem: {
            set(memory[HL], 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x7 extended opcodes
        case RLC_A: {
            rotateLeft(A, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RL_A: {
            rotateLeft(A, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SLA_A: {
            shiftLeft(A);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SWAP_A: {
            swapNibbles(A);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_0_A: {
            bit(A, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_2_A: {
            bit(A, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_4_A: {
            bit(A, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_6_A: {
            bit(A, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_0_A: {
            res(A, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_2_A: {
            res(A, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_4_A: {
            res(A, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_6_A: {
            res(A, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_0_A: {
            set(A, 0);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_2_A: {
            set(A, 2);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_4_A: {
            set(A, 4);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_6_A: {
            set(A, 6);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        
        // x8 extended opcodes
        case RRC_B: {
            rotateRight(B, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_B: {
            rotateRight(B, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_B: {
            shiftRight(B, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_B: {
            shiftRight(B, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_B: {
            bit(B, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_B: {
            bit(B, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_B: {
            bit(B, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_B: {
            bit(B, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_B: {
            res(B, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_B: {
            res(B, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_B: {
            res(B, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_B: {
            res(B, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_B: {
            set(B, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_B: {
            set(B, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_B: {
            set(B, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_B: {
            set(B, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // x9 extended opcodes
        case RRC_C: {
            rotateRight(C, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_C: {
            rotateRight(C, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_C: {
            shiftRight(C, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_C: {
            shiftRight(C, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_C: {
            bit(C, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_C: {
            bit(C, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_C: {
            bit(C, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_C: {
            bit(C, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_C: {
            res(C, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_C: {
            res(C, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_C: {
            res(C, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_C: {
            res(C, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_C: {
            set(C, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_C: {
            set(C, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_C: {
            set(C, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_C: {
            set(C, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xA extended opcodes
        case RRC_D: {
            rotateRight(D, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_D: {
            rotateRight(D, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_D: {
            shiftRight(D, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_D: {
            shiftRight(D, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_D: {
            bit(D, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_D: {
            bit(D, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_D: {
            bit(D, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_D: {
            bit(D, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_D: {
            res(D, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_D: {
            res(D, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_D: {
            res(D, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_D: {
            res(D, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_D: {
            set(D, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_D: {
            set(D, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_D: {
            set(D, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_D: {
            set(D, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xB extended opcodes
        case RRC_E: {
            rotateRight(E, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_E: {
            rotateRight(E, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_E: {
            shiftRight(E, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_E: {
            shiftRight(E, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_E: {
            bit(E, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_E: {
            bit(E, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_E: {
            bit(E, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_E: {
            bit(E, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_E: {
            res(E, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_E: {
            res(E, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_E: {
            res(E, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_E: {
            res(E, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_E: {
            set(E, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_E: {
            set(E, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_E: {
            set(E, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_E: {
            set(E, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xC extended opcodes
        case RRC_H: {
            rotateRight(H, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_H: {
            rotateRight(H, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_H: {
            shiftRight(H, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_H: {
            shiftRight(H, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_H: {
            bit(H, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_H: {
            bit(H, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_H: {
            bit(H, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_H: {
            bit(H, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_H: {
            res(H, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_H: {
            res(H, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_H: {
            res(H, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_H: {
            res(H, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_H: {
            set(H, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_H: {
            set(H, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_H: {
            set(H, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_H: {
            set(H, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xD extended opcodes
        case RRC_L: {
            rotateRight(L, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_L: {
            rotateRight(L, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_L: {
            shiftRight(L, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_L: {
            shiftRight(L, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_L: {
            bit(L, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_L: {
            bit(L, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_L: {
            bit(L, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_L: {
            bit(L, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_L: {
            res(L, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_L: {
            res(L, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_L: {
            res(L, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_L: {
            res(L, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_L: {
            set(L, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_L: {
            set(L, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_L: {
            set(L, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_L: {
            set(L, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xE extended opcodes
        case RRC_HLmem: {
            rotateRight(memory[HL], false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_HLmem: {
            rotateRight(memory[HL], true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_HLmem: {
            shiftRight(memory[HL], true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_HLmem: {
            shiftRight(memory[HL], false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_HLmem: {
            bit(memory[HL], 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_HLmem: {
            bit(memory[HL], 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_HLmem: {
            bit(memory[HL], 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_HLmem: {
            bit(memory[HL], 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_HLmem: {
            res(memory[HL], 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_HLmem: {
            res(memory[HL], 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_HLmem: {
            res(memory[HL], 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_HLmem: {
            res(memory[HL], 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_HLmem: {
            set(memory[HL], 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_HLmem: {
            set(memory[HL], 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_HLmem: {
            set(memory[HL], 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_HLmem: {
            set(memory[HL], 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // xF extended opcodes
        case RRC_A: {
            rotateRight(A, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RR_A: {
            rotateRight(A, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRA_A: {
            shiftRight(A, true);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SRL_A: {
            shiftRight(A, false);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_1_A: {
            bit(A, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_3_A: {
            bit(A, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_5_A: {
            bit(A, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case BIT_7_A: {
            bit(A, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_1_A: {
            res(A, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_3_A: {
            res(A, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_5_A: {
            res(A, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case RES_7_A: {
            res(A, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_1_A: {
            set(A, 1);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_3_A: {
            set(A, 3);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_5_A: {
            set(A, 5);
            exCycles = opcyclesExtended[opcode];
            break;
        }
        case SET_7_A: {
            set(A, 7);
            exCycles = opcyclesExtended[opcode];
            break;
        }

        // TODO
        default:
            std::cout << "Unknown extended opcode: 0xCB 0x" << std::hex << static_cast<int>(opcode) << std::dec << std::endl;
            break;
    }
    return exCycles;
};

//