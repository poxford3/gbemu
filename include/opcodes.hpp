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

    // x7 opcodes
    RLCA = 0x07, // Rotate A left. Old bit 7 to Carry flag
    RLA = 0x17, // Rotate A left through Carry flag
    DAA = 0x27, // Decimal adjust A
    SCF = 0x37, // Set Carry flag
    LD_B_A = 0x47, // Load A into B
    LD_D_A = 0x57, // Load A into D
    LD_H_A = 0x67, // Load A into H
    LD_HLmem_A = 0x77, // Load A into (HL) memory
    ADD_A_A = 0x87, // Add A to A
    SUB_A = 0x97, // Subtract A from A (result will be 0, but flags will be affected)
    AND_A = 0xA7, // Logical AND A with A (result will be A, but flags will be affected)
    OR_A = 0xB7, // Logical OR A with A (result will be A, but flags will be affected)
    RST_0 = 0xC7, // Call subroutine at address 0x00 (RST 00h)
    RST_2 = 0xD7, // Call subroutine at address 0x10 (RST 10h)
    RST_4 = 0xE7, // Call subroutine at address 0x20 (RST 20h)
    RST_6 = 0xF7, // Call subroutine at address 0x30 (RST 30h)

    // x8 opcodes
    LD_a16mem_SP = 0x08, // Load SP into (a16) memory
    JR_s8 = 0x18,
    JR_Z_s8 = 0x28, // Jump to relative address if Z flag is set
    JR_C_s8 = 0x38, // Jump to relative address if C flag
    LD_C_B = 0x48, // Load B into C
    LD_E_B = 0x58, // Load B into E
    LD_L_B = 0x68, // Load B into L
    LD_A_B = 0x78, // Load B into A
    ADC_A_B = 0x88, // Add B to A with carry
    SBC_A_B = 0x98, // Subtract B from A with carry
    XOR_B = 0xA8, // Logical XOR B with A
    CP_B = 0xB8, // Compare B with A (essentially A - B, but only affects flags)
    RET_Z = 0xC8, // Return if Z flag is set
    RET_C = 0xD8, // Return if C flag is set
    ADD_SP_s8 = 0xE8, // Add signed 8-bit immediate to SP
    LD_HL_SP_s8 = 0xF8, // Load SP + signed 8-bit immediate into HL

    // x9 opcodes
    ADD_HL_BC = 0x09, // Add BC to HL
    ADD_HL_DE = 0x19, // Add DE to HL
    ADD_HL_HL = 0x29, // Add HL to HL
    ADD_HL_SP = 0x39, // Add SP to HL
    LD_C_C = 0x49, // Load C into C (essentially a NOP for C)
    LD_E_C = 0x59, // Load C into E
    LD_L_C = 0x69, // Load C into L
    LD_A_C = 0x79, // Load C into A
    ADC_A_C = 0x89, // Add C to A with carry
    SBC_A_C = 0x99, // Subtract C from A with carry
    XOR_C = 0xA9, // Logical XOR C with A
    CP_C = 0xB9, // Compare C with A (essentially A - C, but only affects flags)
    RET = 0xC9, // Return from subroutine
    RETI = 0xD9, // Return from interrupt
    JP_HL = 0xE9, // Jump to address in HL
    LD_SP_HL = 0xF9, // Load HL into SP

    // xA opcodes
    LD_A_BCmem = 0x0A, // Load (BC) memory into A
    LD_A_DEmem = 0x1A, // Load (DE) memory into A
    LD_A_HLi = 0x2A, // Load (HL) memory into A and increment HL
    LD_A_HLd = 0x3A, // Load (HL) memory into A and decrement HL
    LD_C_D = 0x4A, // Load D into C
    LD_E_D = 0x5A, // Load D into E
    LD_L_D = 0x6A, // Load D into L
    LD_A_D = 0x7A, // Load D into A
    ADC_A_D = 0x8A, // Add D to A with carry
    SBC_A_D = 0x9A, // Subtract D from A with carry
    XOR_D = 0xAA, // Logical XOR D with A
    CP_D = 0xBA, // Compare D with A (essentially A - D, but only affects flags)
    JP_Z_a16 = 0xCA, // Jump to absolute address if Z flag is set
    JP_C_a16 = 0xDA, // Jump to absolute address if C flag is set
    LD_a16mem_A = 0xEA, // Load A into (a16) memory
    LD_A_a16mem = 0xFA, // Load (a16) memory into A

    // xB opcodes
    DEC_BC = 0x0B, // Decrement BC
    DEC_DE = 0x1B, // Decrement DE
    DEC_HL = 0x2B, // Decrement HL
    DEC_SP = 0x3B, // Decrement SP
    LD_C_E = 0x4B, // Load E into C
    LD_E_E = 0x5B, // Load E into E
    LD_L_E = 0x6B, // Load E into L
    LD_A_E = 0x7B, // Load E into A
    ADC_A_E = 0x8B, // Add E to A with carry
    SBC_A_E = 0x9B, // Subtract E from A with carry
    XOR_E = 0xAB, // Logical XOR E with A
    CP_E = 0xBB, // Compare E with A (essentially A - E, but only affects flags)
    // 0xCB, 0xDB, 0xEB are unused
    // 0xCB is a prefix for extended opcodes, which we will handle separately
    EI = 0xFB, // Enable interrupts

    // xC opcodes
    INC_C = 0x0C, // Increment C
    INC_E = 0x1C, // Increment E
    INC_L = 0x2C, // Increment L
    INC_A = 0x3C, // Increment A
    LD_C_H = 0x4C, // Load H into C
    LD_E_H = 0x5C, // Load H into E
    LD_L_H = 0x6C, // Load H into L
    LD_A_H = 0x7C, // Load H into A
    ADC_A_H = 0x8C, // Add H to A with carry
    SBC_A_H = 0x9C, // Subtract H from A with carry
    XOR_H = 0xAC, // Logical XOR H with A
    CP_H = 0xBC, // Compare H with A (essentially A - H, but only affects flags)
    CALL_Z_a16 = 0xCC, // Call subroutine at absolute address if Z flag is set
    CALL_C_a16 = 0xDC, // Call subroutine at absolute address if C flag is set
    // 0xEC and 0xFC are unused

    // xD opcodes
    DEC_C = 0x0D, // Decrement C
    DEC_E = 0x1D, // Decrement E
    DEC_L = 0x2D, // Decrement L
    DEC_A = 0x3D, // Decrement A
    LD_C_L = 0x4D, // Load L into C
    LD_E_L = 0x5D, // Load L into E
    LD_L_L = 0x6D, // Load L into L
    LD_A_L = 0x7D, // Load L into A
    ADC_A_L = 0x8D, // Add L to A with carry
    SBC_A_L = 0x9D, // Subtract L from A with carry
    XOR_L = 0xAD, // Logical XOR L with A
    CP_L = 0xBD, // Compare L with A (essentially A - L, but only affects flags)
    CALL_a16 = 0xCD, // Call subroutine at absolute address
    // 0xDD, 0xEd, and 0xFD are unused

    // xE opcodes
    LD_C_d8 = 0x0E, // Load 8-bit immediate into C
    LD_E_d8 = 0x1E, // Load 8-bit immediate into E
    LD_L_d8 = 0x2E, // Load 8-bit immediate into L
    LD_A_d8 = 0x3E, // Load 8-bit immediate into A
    LD_C_HLmem = 0x4E, // Load (HL) memory into C
    LD_E_HLmem = 0x5E, // Load (HL) memory into E
    LD_L_HLmem = 0x6E, // Load (HL) memory into L
    LD_A_HLmem = 0x7E, // Load (HL) memory into A
    ADC_A_HLmem = 0x8E, // Add (HL) memory to A with carry
    SBC_A_HLmem = 0x9E, // Subtract (HL) memory from A with carry
    XOR_HLmem = 0xAE, // Logical XOR (HL) memory with A
    CP_HLmem = 0xBE, // Compare (HL) memory with A (essentially A - (HL), but only affects flags)
    ADC_A_d8 = 0xCE, // Add 8-bit immediate to A with carry
    SBC_A_d8 = 0xDE, // Subtract 8-bit immediate from A with carry
    XOR_d8 = 0xEE, // Logical XOR 8-bit immediate with A
    CP_d8 = 0xFE, // Compare 8-bit immediate with A (essentially A - d8, but only affects flags)

    // xF opcodes
    RRCA = 0x0F, // Rotate A right. Old bit 0 to Carry flag
    RRA = 0x1F, // Rotate A right through Carry flag
    CPL = 0x2F, // Complement A (flip all bits)
    CCF = 0x3F, // Complement Carry flag
    LD_C_A = 0x4F, // Load A into C
    LD_E_A = 0x5F, // Load A into E
    LD_L_A = 0x6F, // Load A into L
    LD_A_A = 0x7F, // Load A into A (essentially a NOP for A)
    ADC_A_A = 0x8F, // Add A to A with carry
    SBC_A_A = 0x9F, // Subtract A from A with carry
    XOR_A = 0xAF, // Logical XOR A with A
    CP_A = 0xBF, // Compare A with A (essentially A - A, but only affects flags)
    RST_1 = 0xCF, // Call subroutine at address 0x08 (RST 08h)
    RST_3 = 0xDF, // Call subroutine at address 0x18 (RST 18h)
    RST_5 = 0xEF, // Call subroutine at address 0x28 (RST 28h)
    RST_7 = 0xFF, // Call subroutine at address 0x38 (RST 38h)

};

enum OpcodeExtended : Byte {
    // Extended opcodes (prefixed with 0xCB)
    RLC_B = 0x00, // Rotate B left. Old bit 7 to Carry flag
    RLC_C = 0x01, // Rotate C left. Old bit 7 to Carry flag
    RLC_D = 0x02, // Rotate D left. Old bit 7 to Carry flag
    RLC_E = 0x03, // Rotate E left. Old bit 7 to Carry flag
    RLC_H = 0x04, // Rotate H left. Old bit 7 to Carry flag
    RLC_L = 0x05, // Rotate L left. Old bit 7 to Carry flag
    RLC_HLmem = 0x06, // Rotate (HL) memory left. Old bit 7 to Carry flag
    RLC_A = 0x07, // Rotate A left. Old bit 7 to Carry flag
    // ... (other extended opcodes would go here)
};