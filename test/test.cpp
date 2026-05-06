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
    cpu.executeInstruction(opcycles[AND_B], memory);
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
    cpu.executeInstruction(opcycles[ADD_A_B], memory);
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
    cpu.executeInstruction(opcycles[SUB_B], memory);
    REQUIRE(cpu.A == expectedResult);
    REQUIRE(cpu.F == 0b01000000); // Check that N flag is set and C flag is reset
}

TEST_CASE("RRA Instruction", "[RRA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RRA; // Rotate A right
    cpu.A = 0b10010001;
    cpu.F = 0b00010000;
    cpu.executeInstruction(opcycles[RRA], memory);
    REQUIRE(cpu.A == 0b11001000); // After RRA, A should be rotated right and bit 7 should be set to old bit 0
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("RLA Instruction", "[RLA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RLA; // Rotate A left
    cpu.A = 0b10010001;
    cpu.F = 0b00010000; // Set C flag to 1 for testing RLA through carry
    cpu.executeInstruction(opcycles[RLA], memory);
    REQUIRE(cpu.A == 0b00100011); // c8
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("RLCA Instruction", "[RLCA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = RLCA;
    cpu.A = 0b10010001;
    cpu.F = 0b00010000; // Set C flag to 1 for testing RLA through carry
    cpu.executeInstruction(opcycles[RLCA], memory);
    REQUIRE(cpu.A == 0b00100011); // 23
    REQUIRE(cpu.F == 0x10);
}

TEST_CASE("LD A,(BC) Instruction", "[LD A,(BC)]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_A_BCmem; // LD A,(BC)
    cpu.A = 0x69;
    memory[cpu.BC] = 0x42; // default BC is 0x0013
    cpu.executeInstruction(opcycles[LD_A_BCmem], memory);
    REQUIRE(cpu.A == 0x42); // After LD A,(BC), A should be loaded with value at memory address BC (0x0013)
}

TEST_CASE("LD (BC), A Instruction", "[LD (BC),A]") {

    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = LD_BCmem_A; // LD (BC),A
    cpu.A = 0x42;
    memory[cpu.BC] = 0x69; // Set memory at address BC (0x0013) to 0x69 for testing LD (BC),A
    cpu.executeInstruction(opcycles[LD_BCmem_A], memory);
    REQUIRE(memory[cpu.BC] == 0x42); // After LD (BC),A, memory at address BC should be loaded with value of A (0x42)
}

TEST_CASE("ADC A, B Instruction", "[ADC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = ADC_A_B; // ADC A, B
    cpu.A = 0x90;
    cpu.B = 0x80;
    cpu.F = 0b00010000; // Set carry flag to 1 for testing ADC
    Byte expectedResult = static_cast<Byte>(0x90 + 0x80 + 1); // 0x81
    cpu.executeInstruction(opcycles[ADC_A_B], memory);
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
    cpu.F = 0b00010000; // Set carry flag to 1 for testing SBC
    Byte expectedResult = static_cast<Byte>(0x90 - 0x80 - 1); // 0x0F
    cpu.executeInstruction(opcycles[SBC_A_B], memory);
    REQUIRE(cpu.A == expectedResult);
    REQUIRE(cpu.F == 0b01100000); // Check that carry flag is set
}

TEST_CASE("DAA Instruction", "[DAA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = DAA; // DAA
    cpu.A = 0x45; // Example value that would require adjustment
    cpu.F = 0b00100000; // Clear flags for testing DAA
    cpu.executeInstruction(opcycles[DAA], memory);
    REQUIRE(cpu.A == 0x4B); // After DAA, A should be adjusted to a valid BCD representation (in this case, it remains the same)

    memory[cpu.PC] = DAA; // DAA
    cpu.A = 0x9A; // Example value that would require adjustment
    cpu.F = 0b00010000; // Set carry flag to 1 for testing DAA
    cpu.executeInstruction(opcycles[DAA], memory);
    REQUIRE(cpu.A == 0xFA);
}

TEST_CASE("SCF Instruction", "[SCF]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = SCF; // SCF
    cpu.F = 0b11000000; // Clear all flags for testing SCF
    cpu.executeInstruction(opcycles[SCF], memory);
    REQUIRE(cpu.F == 0b10010000); // After SCF, C flag should be set, N and H flags should be reset, Z flag should be unaffected
}

TEST_CASE("CCF Instruction", "[CCF]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CCF; // CCF
    cpu.F = 0b10010000; // Set C flag to 1 for testing CCF
    cpu.executeInstruction(opcycles[CCF], memory);
    REQUIRE(cpu.F == 0b10000000);

    memory[cpu.PC] = CCF; // CCF
    cpu.F = 0b00000000; // Set C flag to 0 for testing CCF
    cpu.executeInstruction(opcycles[CCF], memory);
    REQUIRE(cpu.F == 0b00010000);
}

TEST_CASE("CPL Instruction", "[CPL]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = CPL; // CPL
    cpu.A = 0b10101010; // Example value for testing CPL
    cpu.F = 0b00000000; // Clear flags for testing CPL
    cpu.executeInstruction(opcycles[CPL], memory);
    REQUIRE(cpu.A == 0b01010101); // After CPL, A should be bitwise complemented
    REQUIRE(cpu.F == 0b01100000); // After CPL, N and H flags should be set, C flag should be reset, Z flag should be unaffected
}

TEST_CASE("POP BC Instruction", "[POP BC]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[cpu.PC] = POP_BC; // POP BC
    cpu.SP = 0xFFFE; // Set stack pointer to top of stack
    memory[cpu.SP] = 0x34; // Set value at SP to 0x34 (C)
    memory[cpu.SP + 1] = 0x12; // Set value at SP+1 to 0x12 (B)
    cpu.executeInstruction(opcycles[POP_BC], memory);
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
    cpu.executeInstruction(opcycles[RET], memory);
    REQUIRE(cpu.PC == 0x1234); // After RET, PC should be loaded with return address from stack (0x1234)
    REQUIRE(cpu.SP == 0x0000); // After RET, SP should be incremented by 2 (from 0xFFFE to 0x0000)
}

// need to test the jump/calls to get the different opcycles

//