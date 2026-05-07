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
}

void Gameboy::start() {
    cpu.reset(memory);
};

void Gameboy::stop() {
    // todo
}

void Gameboy::tick() {
    if (checksumPassed) {

        
        Byte opcode = cpu.loadByte(memory);

        std::cout << "opcode: (0x" << std::hex << static_cast<int>(opcode) << "), AF: 0x" << std::hex << static_cast<int>(cpu.AF) << std::dec;
        std::printf(" PC: 0x%04X", cpu.PC);
        std::printf(" SP: 0x%04X\n", cpu.SP);

        if (cpu.PC == 0xC001) {
            printf("JP operand bytes: %02X %02X\n", 
                cpu.readByte(memory, 0xC002), cpu.readByte(memory, 0xC003));
        }

        cpu.executeInstructions(100, opcode, memory);
    }
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