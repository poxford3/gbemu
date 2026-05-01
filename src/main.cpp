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
    // end test

    cpu.executeInstruction(4, memory);
    
    // TODO: implement file uploading and relative paths
    // using absolute path for testing, need to implement file uploading and relative paths
    // FileHandler fileHandler("/Users/poxford3/Documents/coding/gbemu/assets/pokemon_blue.gb");
    // std::vector<unsigned char> buffer = fileHandler.readFile();
    
    // std::cout << "File size: " << buffer.size() << " bytes" << std::endl;
    // fileHandler.readRandomValues(buffer, 256, 32); // Read first 32 bytes starting at $0100
    // fileHandler.readNthByte(buffer, 0x0100); // Read byte at index $0100
    
    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}