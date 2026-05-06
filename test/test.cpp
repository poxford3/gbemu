#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>
#include "cpu.hpp"
#include "opcodes.hpp"
#include "opcycles.hpp"

TEST_CASE("AND instruction", "[AND]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[0x0100] = AND_B; // AND B with A
    cpu.B = 0xB1;
    cpu.A = 0xB4;
    cpu.executeInstruction(opcycles[AND_B], memory);
    REQUIRE(cpu.A == (0xB4 & 0xB1));
}

// TEST_CASE("ADD instruction", "[ADD]") {
//     Mem memory;
//     Cpu cpu;
//     cpu.reset(memory);

//     cpu.A = 0x90;
//     cpu.B = 0x80;
//     memory[0x0100] = ADD_A_B; // ADD B to A
//     cpu.executeInstruction(opcycles[ADD_A_B], memory);
//     REQUIRE(cpu.A == (0x90 + 0x80));
// }

TEST_CASE("RRA Instruction", "[RRA]") {
    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    memory[0x0100] = RRA; // Rotate A right
    cpu.A = 0b10010001;
    cpu.F = 0b00010000;
    cpu.executeInstruction(opcycles[RRA], memory);
    REQUIRE(cpu.A == 0b11001000); // After RRA, A should be rotated right and bit 7 should be set to old bit 0
    REQUIRE(cpu.F == 0x10);
}