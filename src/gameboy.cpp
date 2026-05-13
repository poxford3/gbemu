#include <fstream>
#include "gameboy.hpp"
#include "types.hpp"

Gameboy::Gameboy() {
    start();
}

Gameboy::Gameboy(const std::vector<Byte>& program) {
    start();

    // std::cout << "num bytes of prog: " << program.size() << std::endl;
    uint numBytes = program.size();
    
    cpu.loadProgram(program, numBytes, memory);

    checksumPassed = checksum();
    // std::cout << (checksumPassed ? "checksum passed" : "checksum failed") << std::endl;
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
    // std::cout << "opcode: " << std::hex << static_cast<int>(opcode) << std::dec << "\tPC: 0x" << std::hex << (cpu.PC) << std::dec << std::dec <<
    // "\tF: 0x" << std::hex << (int)(cpu.F) << std::dec <<
    // "\tDE: 0x" << std::hex << (cpu.DE) << std::dec <<
    // "\tHL: 0x" << std::hex << (cpu.HL) << std::dec <<
    // "\tA: 0x" << std::hex << int(cpu.A) << std::dec <<
    // "\tmem[c000]: 0x" << std::hex << (int)(memory[0xC000]) << std::dec << std::endl;
    // printf(", C000-4 %02X %02X %02X %02X %02X\n", memory[0xC000], memory[0xC001], memory[0xC002], memory[0xC003], memory[0xC004]);
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

void Gameboy::testWithJson(std::string path) {
    std::ifstream f(path);

    if (f) {
        json opcodeTestData = json::parse(f);

        int failCount = 0;
        bool singleFile = true;

        for (int i = 0; i < opcodeTestData.size(); i++) {
            // std::cout << i << std::endl;
            // set initial values
            cpu.PC = opcodeTestData[i]["initial"]["pc"].get<Word>();
            cpu.SP = opcodeTestData[i]["initial"]["sp"].get<Word>();
            cpu.A = opcodeTestData[i]["initial"]["a"].get<Byte>();
            cpu.B = opcodeTestData[i]["initial"]["b"].get<Byte>();
            cpu.C = opcodeTestData[i]["initial"]["c"].get<Byte>();
            cpu.D = opcodeTestData[i]["initial"]["d"].get<Byte>();
            cpu.E = opcodeTestData[i]["initial"]["e"].get<Byte>();
            cpu.F = opcodeTestData[i]["initial"]["f"].get<Byte>();
            cpu.H = opcodeTestData[i]["initial"]["h"].get<Byte>();
            cpu.L = opcodeTestData[i]["initial"]["l"].get<Byte>();
            cpu.IME = opcodeTestData[i]["initial"]["ime"].get<int>() != 0; // if false, return false
            memory[cpu.IE] = opcodeTestData[i]["initial"]["ie"];
            // load in the opcode to memory, along with necessary values
            for (int j = 0; j < opcodeTestData[i]["initial"]["ram"].size(); j++) {
                memory[opcodeTestData[i]["initial"]["ram"][j][0]] = opcodeTestData[i]["initial"]["ram"][j][1];
            }

            // cpu.TEST_showAllRegValuesDecimal();

            tick();

            if (
                cpu.PC != opcodeTestData[i]["final"]["pc"] ||
                cpu.SP != opcodeTestData[i]["final"]["sp"] ||
                cpu.A != opcodeTestData[i]["final"]["a"] ||
                cpu.B != opcodeTestData[i]["final"]["b"] ||
                cpu.C != opcodeTestData[i]["final"]["c"] ||
                cpu.D != opcodeTestData[i]["final"]["d"] ||
                cpu.E != opcodeTestData[i]["final"]["e"] ||
                cpu.F != opcodeTestData[i]["final"]["f"] ||
                cpu.H != opcodeTestData[i]["final"]["h"] ||
                cpu.L != opcodeTestData[i]["final"]["l"] ||
                cpu.IME != (opcodeTestData[i]["final"]["ime"].get<int>() != 0)
                // || memory[opcodeTestData[i]["final"]["ram"][0][0]] != opcodeTestData[i]["final"]["ram"][0][1]
            ) {
                failCount++;
                singleFile && std::cout << "error executing operation out mem, " << opcodeTestData[i]["name"] << std::endl;
                bool memoryError = false;
                for (int j = 0; j < opcodeTestData[i]["final"]["ram"].size(); j++) {
                    if (memory[opcodeTestData[i]["final"]["ram"][j][0]] != opcodeTestData[i]["final"]["ram"][j][1]) {
                        memoryError = true;
                    }
                }
                if (memoryError) {
                    singleFile && std::cout << "error executing operation in mem, " << opcodeTestData[i]["name"] << std::endl;
                }
                // cpu.showAllRegisterValues();
                cpu.TEST_showAllRegValuesDecimal();
            } else {
                // std::cout << "pass, op: " << opcodeTestData[i]["name"] << std::endl;
            }
        }
        std::cout << "finished testing :D, " << opcodeTestData[0]["name"] << ", " << failCount << std::endl;
    } else {
        std::cout << "error opening file" << std::endl;
    }

}