#include "types.hpp"

enum Opcodes : Byte {
    // Opcodes
    
    // x0 opcodes
    NOP = 0x00, // No operation
    STOP = 0x01, // stops clock and oscillator
    JR_NZ_s8 = 0x20, // Jump to relative address if Z flag is not set
    JR_NC_s8 = 0x30, // Jump to relative address if C flag is not set
    LD_B_B = 0x40, // Load B into B (essentially a NOP for B)
    LD_D_B = 0x50, // Load B into D
    LD_H_B = 0x60, // Load B into H
    LD_HLmem_B = 0x70, // Load B into (HL) memory
    ADD_A_B = 0x80, // Add B to A
    SUB_B = 0x90, // Subtract B from A
    AND_B = 0xA0, // Logical AND B with A
    OR_B = 0xB0, // Logical OR B with A
    RET_NZ = 0xC0, // Return if Z flag is not set
    RET_NC = 0xD0, // Return if C flag
    LD_a8mem_A = 0xE0, // Load A into (a8) memory
    LD_A_a8mem = 0xF0, // Load (a8) memory into A
    
    // x1 opcodes
    LD_BC_d16 = 0x10, // Load 16-bit immediate into BC
    LD_DE_d16 = 0x11, // Load 16-bit immediate into DE
    LD_HL_d16 = 0x21, // Load 16-bit immediate into HL
    LD_SP_d16 = 0x31, // Load 16-bit immediate into SP
    LD_B_C = 0x41, // Load C into B
    LD_D_C = 0x51, // Load C into D
    LD_H_C = 0x61, // Load C into H
    LD_HLmem_C = 0x71, // Load C into (HL) memory
    ADD_A_C = 0x81, // Add C to A
    SUB_C = 0x91, // Subtract C from A
    AND_C = 0xA1, // Logical AND C with A
    OR_C = 0xB1, // Logical OR C with A
    POP_BC = 0xC1, // Pop 16-bit value from stack into BC
    POP_DE = 0xD1, // Pop 16-bit value from stack into DE
    POP_HL = 0xE1, // Pop 16-bit value from stack into HL
    POP_AF = 0xF1, // Pop 16-bit value from stack into AF

    // x2 opcodes
    LD_BCmem_A = 0x02, // Load A into (BC) memory
    LD_DEmem_A = 0x12, // Load A into (DE) memory
    LD_HLi_A = 0x22, // Load A into (HL) memory and increment HL
    LD_HLd_A = 0x32, // Load A into (HL) memory and decrement HL
    LD_B_D = 0x42, // Load D into B
    LD_D_D = 0x52, // Load D into D
    LD_H_D = 0x62, // Load D into H
    LD_HLmem_D = 0x72, // Load D into (HL) memory
    ADD_A_D = 0x82, // Add D to A
    SUB_D = 0x92, // Subtract D from A
    AND_D = 0xA2, // Logical AND D with A
    OR_D = 0xB2, // Logical OR D with A
    JP_NZ_a16 = 0xC2, // Jump to absolute address if Z flag is not set
    JP_NC_a16 = 0xD2, // Jump to absolute address if C flag is not set
    LD_Cmem_A = 0xE2, // Load A into (C) memory
    LD_A_Cmem = 0xF2, // Load (C) memory into A

    // x3 opcodes
    INC_BC = 0x03, // Increment BC
    INC_DE = 0x13, // Increment DE
    INC_HL = 0x23, // Increment HL
    INC_SP = 0x33, // Increment SP
    LD_B_E = 0x43, // Load E into B
    LD_D_E = 0x53, // Load E into D
    LD_H_E = 0x63, // Load E into H
    LD_HLmem_E = 0x73, // Load E into (HL) memory
    ADD_A_E = 0x83, // Add E to A
    SUB_E = 0x93, // Subtract E from A
    AND_E = 0xA3, // Logical AND E with A
    OR_E = 0xB3, // Logical OR E with A
    JP_a16 = 0xC3, // Jump to absolute address
    // 0xD3 & 0xE3 are unused
    DI = 0xF3, // Disable interrupts

    // x4 opcodes
    INC_B = 0x04, // Increment B
    INC_D = 0x14, // Increment D
    INC_H = 0x24, // Increment H
    INC_HLmem = 0x34, // Increment (HL) memory
    LD_B_H = 0x44, // Load H into B
    LD_D_H = 0x54, // Load H into D
    LD_H_H = 0x64, // Load H into H
    LD_HLmem_H = 0x74, // Load H into (HL) memory
    ADD_A_H = 0x84, // Add H to A
    SUB_H = 0x94, // Subtract H from A
    AND_H = 0xA4, // Logical AND H with A
    OR_H = 0xB4, // Logical OR H with A
    CALL_NZ_A16 = 0xC4, // Call subroutine at absolute address if Z flag is not set
    CALL_NC_A16 = 0xD4, // Call subroutine at absolute address if C flag is not set
    // 0xE4 and 0xF4 are unused

    // x5 opcodes
    DEC_B = 0x05, // Decrement B
    DEC_D = 0x15, // Decrement D
    DEC_H = 0x25, // Decrement H
    DEC_HLmem = 0x35, // Decrement (HL) memory
    LD_B_L = 0x45, // Load L into B
    LD_D_L = 0x55, // Load L into D
    LD_H_L = 0x65, // Load L into H
    LD_HLmem_L = 0x75, // Load L into (HL) memory
    ADD_A_L = 0x85, // Add L to A
    SUB_L = 0x95, // Subtract L from A
    AND_L = 0xA5, // Logical AND L with A
    OR_L = 0xB5, // Logical OR L with A
    PUSH_BC = 0xC5, // Push BC onto stack
    PUSH_DE = 0xD5, // Push DE onto stack
    PUSH_HL = 0xE5, // Push HL onto stack
    PUSH_AF = 0xF5, // Push AF onto stack

    // x6 opcodes
    LD_B_d8 = 0x06, // Load 8-bit immediate into B
    LD_D_d8 = 0x16, // Load 8-bit immediate into D
    LD_H_d8 = 0x26, // Load 8-bit immediate into H
    LD_HLmem_d8 = 0x36, // Load 8-bit immediate into (HL) memory
    LD_B_HLmem = 0x46, // Load (HL) memory into B
    LD_D_HLmem = 0x56, // Load (HL) memory into D
    LD_H_HLmem = 0x66,
    HALT = 0x76, // Halt CPU until interrupt occurs
    ADD_A_HLmem = 0x86, // Add (HL) memory to A
    SUB_HLmem = 0x96, // Subtract (HL) memory from A
    AND_HLmem = 0xA6, // Logical AND (HL) memory with A
    OR_HLmem = 0xB6, // Logical OR (HL) memory with A
    ADD_A_d8 = 0xC6, // Add 8-bit immediate to A
    SUB_d8 = 0xD6, // Subtract 8-bit immediate from A
    AND_d8 = 0xE6, // Logical AND 8-bit immediate with A
    OR_d8 = 0xF6, // Logical OR 8-bit immediate with A

    // xA opcodes
    LD_A_BCmem = 0x0A, // Load (BC) memory into A
    LD_A_DEmem = 0x1A, // Load (DE) memory into A

};
