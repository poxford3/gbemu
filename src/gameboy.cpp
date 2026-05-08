#include "gameboy.hpp"
#include "types.hpp"

Gameboy::Gameboy() {
    start();
}

Gameboy::Gameboy(const std::vector<Byte>& program) {
    start();

    // std::cout << "num bytes of prog: " << program.size() << std::endl;
    uint numBytes = program.size();
    
    std::cout << "before memory at 0x0101: " << std::hex << (int)memory[0x0101] << std::dec << std::endl;
    cpu.loadProgram(program, numBytes, memory);
    std::cout << "after memory at 0x0101: " << std::hex << (int)memory[0x0101] << std::dec << std::endl;

    checksumPassed = checksum();
    std::cout << (checksumPassed ? "checksum passed" : "checksum failed") << std::endl;
}

void Gameboy::start() {
    cpu.reset(memory);
};

void Gameboy::stop() {
    // todo
}

void Gameboy::tick() {
    cpu.handleInterrupt(memory);
    
    if (cpu.halted) {
        if (memory[cpu.IE] & memory[cpu.IF]) {
            cpu.halted = false;
        }
        return;
    }
    
    Byte opcode = cpu.loadByte(memory);
    cpu.executeInstructions(100, opcode, memory);
}

bool Gameboy::checksum() {
    // https://gbdev.io/pandocs/The_Cartridge_Header.html?highlight=checksum#014d--header-checksum
    Byte checksum = 0;
    for (Word address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - memory[address] - 1;
    }
    // if the memory address at 0x014D matches bottom 8 bits of checksum, header passes
    return memory[0x014D] == (checksum & 0xFF) ? true : false;
}

void Gameboy::printMemory() {
    // bricks script, JUST for testing
    for (uint i = 0x0; i <= 0x10000; i++) {
        std::cout << "memory[" << i << "] = " << std::hex << (int)memory[i] << std::endl;
    }
}