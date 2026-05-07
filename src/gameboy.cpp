#include "gameboy.hpp"
#include "types.hpp"

Gameboy::Gameboy() {
    start();
}

Gameboy::Gameboy(const std::vector<Byte>& program) {
    start();

    std::cout << "num bytes of prog: " << program.size() << std::endl;
    
    cpu.loadProgram(program, program.size(), memory);
    if (checksum()) {
        std::cout << "passed checksum" << std::endl;
        // clock start
        // execute commands etc
    } else {
        // todo
        std::cout << "passed checksum" << std::endl;
    }
}

void Gameboy::start() {
    cpu.reset(memory);
};

void Gameboy::stop() {
    // todo
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