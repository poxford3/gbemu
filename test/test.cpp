#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>
#include "cpu.hpp"
#include "opcodes.hpp"
#include "opcycles.hpp"

TEST_CASE("AND instruction", "[AND]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = AND_B; // AND B with A
    cpu.B = 0xB1;
    cpu.A = 0xB4;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[AND_B], opcode, memory);
    REQUIRE(cpu.A == (0xB4 & 0xB1));
}

TEST_CASE("ADD instruction", "[ADD]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    cpu.A = 0x90;
    cpu.B = 0x80;
    // casting this to byte loses some data but that is expected behavior
    // the carry flag is meant to show the result of the leftover
    Byte _90and80 = static_cast<Byte>(0x90 + 0x80); // 0x80
    memory[cpu.PC] = ADD_A_B; // ADD B to A
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[ADD_A_B], opcode, memory);
    REQUIRE(cpu.A == _90and80);
    REQUIRE((cpu.F & 0x10) != 0); // Check that carry flag is set
}

TEST_CASE("SUB B Instruction", "[SUB B]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = SUB_B; // SUB B from A
    cpu.A = 0x50;
    cpu.B = 0x20;
    Byte expectedResult = static_cast<Byte>(0x50 - 0x20); // 0x30
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[SUB_B], opcode, memory);
    REQUIRE(cpu.A == expectedResult);
    REQUIRE(cpu.F == 0b0100'0000); // Check that N flag is set and C flag is reset
}

TEST_CASE("RRA Instruction", "[RRA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RRA; // Rotate A right
    cpu.A = 0b1001'0001;
    cpu.F = 0b0001'0000;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[RRA], opcode, memory);
    REQUIRE(cpu.A == 0b1100'1000); // After RRA, A should be rotated right and bit 7 should be set to old bit 0
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("RLA Instruction", "[RLA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RLA; // Rotate A left
    cpu.A = 0b1001'0001;
    cpu.F = 0b0001'0000; // Set C flag to 1 for testing RLA through carry
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[RLA], opcode, memory);
    REQUIRE(cpu.A == 0b0010'0011); // c8
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("RLCA Instruction", "[RLCA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RLCA;
    cpu.A = 0b1001'0001;
    cpu.F = 0b0001'0000; // Set C flag to 1 for testing RLA through carry
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[RLCA], opcode, memory);
    REQUIRE(cpu.A == 0b0010'0011); // 23
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("LD A,(BC) Instruction", "[LD A,(BC)]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_A_BCmem; // LD A,(BC)
    cpu.A = 0x69;
    memory[cpu.BC] = 0x42; // default BC is 0x0013
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_A_BCmem], opcode, memory);
    REQUIRE(cpu.A == 0x42); // After LD A,(BC), A should be loaded with value at memory address BC (0x0013)
}

TEST_CASE("LD (BC), A Instruction", "[LD (BC),A]") {

    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_BCmem_A; // LD (BC),A
    cpu.A = 0x42;
    memory[cpu.BC] = 0x69; // Set memory at address BC (0x0013) to 0x69 for testing LD (BC),A
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_BCmem_A], opcode, memory);
    REQUIRE(memory[cpu.BC] == 0x42); // After LD (BC),A, memory at address BC should be loaded with value of A (0x42)
}

TEST_CASE("LDi Instruction", "[LDi], A") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_HLi_A; // load A into memory location HL and increment HL
    cpu.A = 0x69;
    cpu.HL = 0x1010;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_HLi_A], opcode, memory);
    REQUIRE(memory[0x1010] == 0x69);
    REQUIRE(cpu.HL == ((0x1010 + 1) & 0xFFFF)); // check if the HL register pair has incresaed by 1 after A is passed into it
}

TEST_CASE("LDd Instruction", "[LDd], A") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_HLd_A; // load A into memory location HL and increment HL
    cpu.A = 0x69;
    cpu.HL = 0x1010;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_HLd_A], opcode, memory);
    REQUIRE(memory[0x1010] == 0x69);
    REQUIRE(cpu.HL == ((0x1010 - 1) & 0xFFFF)); // check if the HL register pair has incresaed by 1 after A is passed into it
}

TEST_CASE("LD (HL), d8 Instruction", "[LD (HL), d8]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_HLmem_d8; // LD (HL), d8
    memory[cpu.PC+1] = 0x69;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_HLmem_d8], opcode, memory);
    REQUIRE(memory[cpu.HL] == 0x69);
}

TEST_CASE("LD (a16), SP Instruction", "[LD (a16), SP]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_a16mem_SP;
    memory[cpu.PC+1] = 0x69;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[LD_a16mem_SP], opcode, memory);
    REQUIRE(memory[0x69] == (cpu.SP & 0xFF));
}

TEST_CASE("PUSH BC Instruction", "[PUSH BC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = PUSH_BC;
    cpu.BC = 0xB0C0;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[PUSH_BC], opcode, memory);
    REQUIRE(memory[cpu.SP] == 0xC0);
    REQUIRE(memory[cpu.SP+1] == 0xB0); // higher byte is B so would come after C
}

TEST_CASE("ADC A, B Instruction", "[ADC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = ADC_A_B; // ADC A, B
    cpu.A = 0x90;
    cpu.B = 0x80;
    cpu.F = 0b0001'0000; // Set carry flag to 1 for testing ADC
    Byte expectedResult = static_cast<Byte>(0x90 + 0x80 + 1); // 0x81
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[ADC_A_B], opcode, memory);
    REQUIRE(cpu.A == expectedResult);
    REQUIRE((cpu.F & 0x10) != 0); // Check that carry flag is set
}

TEST_CASE("SBC A, B Instruction", "[SBC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = SBC_A_B; // SBC A, B
    cpu.A = 0x90;
    cpu.B = 0x80;
    cpu.F = 0b0001'0000; // Set carry flag to 1 for testing SBC
    Byte expectedResult = static_cast<Byte>(0x90 - 0x80 - 1); // 0x0F
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[SBC_A_B], opcode, memory);
    REQUIRE(cpu.A == expectedResult);
    REQUIRE(cpu.F == 0b0110'0000); // Check that carry flag is set
}

TEST_CASE("DAA Instruction", "[DAA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = DAA; // DAA
    cpu.A = 0x45; // Example value that would require adjustment
    cpu.F = 0b0010'0000; // Clear flags for testing DAA
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[DAA], opcode, memory);
    REQUIRE(cpu.A == 0x4B); // After DAA, A should be adjusted to a valid BCD representation (in this case, it remains the same)

    memory[cpu.PC] = DAA; // DAA
    cpu.A = 0x9A; // Example value that would require adjustment
    cpu.F = 0b0001'0000; // Set carry flag to 1 for testing DAA
    opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[DAA], opcode, memory);
    REQUIRE(cpu.A == 0xFA);
}

TEST_CASE("SCF Instruction", "[SCF]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = SCF; // SCF
    cpu.F = 0b1100'0000; // Clear all flags for testing SCF
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[SCF], opcode, memory);
    REQUIRE(cpu.F == 0b1001'0000); // After SCF, C flag should be set, N and H flags should be reset, Z flag should be unaffected
}

TEST_CASE("CCF Instruction", "[CCF]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CCF; // CCF
    cpu.F = 0b1001'0000; // Set C flag to 1 for testing CCF
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CCF], opcode, memory);
    REQUIRE(cpu.F == 0b1000'0000);

    memory[cpu.PC] = CCF; // CCF
    cpu.F = 0b0000'0000; // Set C flag to 0 for testing CCF
    opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CCF], opcode, memory);
    REQUIRE(cpu.F == 0b0001'0000);
}

TEST_CASE("CPL Instruction", "[CPL]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CPL; // CPL
    cpu.A = 0b1010'1010; // Example value for testing CPL
    cpu.F = 0b0000'0000; // Clear flags for testing CPL
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CPL], opcode, memory);
    REQUIRE(cpu.A == 0b0101'0101); // After CPL, A should be bitwise complemented
    REQUIRE(cpu.F == 0b0110'0000); // After CPL, N and H flags should be set, C flag should be reset, Z flag should be unaffected
}

TEST_CASE("POP BC Instruction", "[POP BC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = POP_BC; // POP BC
    cpu.SP = 0xFFFE; // Set stack pointer to top of stack
    memory[cpu.SP] = 0x34; // Set value at SP to 0x34 (C)
    memory[cpu.SP + 1] = 0x12; // Set value at SP+1 to 0x12 (B)
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[POP_BC], opcode, memory);
    REQUIRE(cpu.BC == 0x1234); // After POP BC, BC should be loaded with value from stack (0x1234)
    REQUIRE(cpu.SP == 0x0000); // After POP BC, SP should be incremented by 2 (from 0xFFFE to 0x0000)
}

TEST_CASE("RET Instruction", "[RET]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RET; // RET
    cpu.SP = 0xFFFE; // Set stack pointer to top of stack
    memory[cpu.SP] = 0x34; // Set value at SP to 0x34 (lower byte of return address)
    memory[cpu.SP + 1] = 0x12; // Set value at SP+1 to 0x12 (higher byte of return address)
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[RET], opcode, memory);
    REQUIRE(cpu.PC == 0x1234); // After RET, PC should be loaded with return address from stack (0x1234)
    REQUIRE(cpu.SP == 0x0000); // After RET, SP should be incremented by 2 (from 0xFFFE to 0x0000)
}

TEST_CASE("JP NZ a16 Instruction", "[JP NZ a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = JP_NZ_a16; // RET
    memory[cpu.PC+1] = 0x42;
    memory[cpu.PC+2] = 0x24; // a16 == 0x2442
    cpu.F = 0b0001'0000; // Z flag is 0;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[JP_NZ_a16], opcode, memory);
    REQUIRE(cpu.PC == 0x2442);
}

TEST_CASE("JR NZ s8 Instruction", "[JR NZ s8]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = JR_NZ_s8; // RET
    memory[cpu.PC+1] = 0x42; // s8 = 42
    cpu.F = 0b0001'0000; // Z flag is 0;
    Word endLocation = (0x42 + cpu.PC + 2); // +2 for opcode + operand
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[JR_NZ_s8], opcode, memory);
    REQUIRE(cpu.PC == endLocation);
}

TEST_CASE("CALL NC, a16 C=0 Instruction", "[CALL NC, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_NC_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0000'0000; // C flag is 0;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_NC_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFC);
    REQUIRE(cpu.PC == 0xC250);
    REQUIRE(memory[cpu.SP+1] == 0x01);
    REQUIRE(memory[cpu.SP] == 0x03);
}

TEST_CASE("CALL NC, a16 C=1 Instruction", "[CALL NC, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_NC_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0001'0000; // C flag is 1;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_NC_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFE);
    REQUIRE(cpu.PC == 0x0103);
}

TEST_CASE("CALL C, a16 C=0 Instruction", "[CALL C, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_C_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0000'0000; // C flag is 0;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_C_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFE);
    REQUIRE(cpu.PC == 0x0103);
}

TEST_CASE("CALL C, a16 C=1 Instruction", "[CALL C, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_C_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0001'0000; // C flag is 0;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_C_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFC);
    REQUIRE(cpu.PC == 0xC250);
    REQUIRE(memory[cpu.SP+1] == 0x01);
    REQUIRE(memory[cpu.SP] == 0x03);
}

TEST_CASE("CALL NZ, a16 Z=1 Instruction", "[CALL NZ, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_NZ_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b1000'0000; // Z flag is 1;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_NZ_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFE);
    REQUIRE(cpu.PC == 0x103); // PC will just be incremented, since no Call was made
}

TEST_CASE("CALL NZ, a16 Z=0 Instruction", "[CALL NZ, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_NZ_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0000'0000; // Z flag is 1;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_NZ_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFC);
    REQUIRE(cpu.PC == 0xC250);
    REQUIRE(memory[cpu.SP+1] == 0x01);
    REQUIRE(memory[cpu.SP] == 0x03);
}

TEST_CASE("CALL Z, a16 Z=1 Instruction", "[CALL Z, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_Z_a16;
    memory[cpu.PC+1] = 0x50;
    memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b1000'0000; // Z flag is 1;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_Z_a16], opcode, memory);
    REQUIRE(cpu.SP == 0xFFFC);
    REQUIRE(cpu.PC == 0xC250);
    REQUIRE(memory[cpu.SP+1] == 0x01);
    REQUIRE(memory[cpu.SP] == 0x03);
}

TEST_CASE("CALL Z, a16 Z=0 Instruction", "[CALL Z, a16]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CALL_Z_a16;
    // memory[cpu.PC+1] = 0x50;
    // memory[cpu.PC+2] = 0xC2; // a16 == 0xC250
    cpu.F = 0b0000'0000; // Z flag is 1;
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(opcycles[CALL_Z_a16], opcode, memory);
    // REQUIRE(cpu.SP == 0xFFFE);
    REQUIRE(cpu.PC == 0x103); // PC will just be incremented, since no Call was made
}

//