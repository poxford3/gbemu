#include <iostream>
#include <fstream>
#include <vector>
#include "window.hpp"
#include "file.hpp"
#include "cpu.hpp"


int main() {


    uint exampleOpcodes[4] = {0x00, 0x01, 0x34, 0x12}; // NOP, LD BC, 0x1234

    Mem memory;
    Cpu cpu;
    cpu.reset(memory);

    // test
    memory[0x0100] = Cpu::NOP; // NOP
    memory[0x0101] = Cpu::LD_BC_d16; // LD BC,d16
    memory[0x0102] = 0x34; // low byte of d16
    memory[0x0103] = 0x12; // high byte of d16

    cpu.executeInstruction(4, memory);
    cpu.showAllRegisterValues();
    printf("\n\n");

    memory[0x0104] = Cpu::LD_B_C; // LD B,C
    
    cpu.executeInstruction(1, memory);
    cpu.showAllRegisterValues();
    printf("\n\n");
    
    // memory[0x0105] = Cpu::LD_BCmem_A;
    // cpu.A = 0x42;
    // memory[0x3434] = 0x69; // Set memory at address 0x3434 to 0x69 for testing LD (BC),A
    // cpu.executeInstruction(2, memory);
    // std::cout << "Value at memory address 0x3434 after LD (BC),A: 0x" << std::hex << static_cast<int>(memory[0x3434]) << std::dec << std::endl;
    // cpu.showAllRegisterValues();
    // printf("\n\n");
    
    // cpu.reset(memory);
    memory[0x0100] = Cpu::LD_A_BCmem; // LD A,(BC)
    cpu.A = 0x69;
    memory[0x0013] = 0x42; // default BC is 0x0013
    cpu.executeInstruction(2, memory);
    std::cout << "Value at memory address 0x0013 after LD A,(BC): 0x" << std::hex << static_cast<int>(memory[0x0013]) << std::dec << std::endl;
    cpu.showAllRegisterValues();
    // end test


    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}