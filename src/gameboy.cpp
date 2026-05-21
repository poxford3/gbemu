#include <string>
#include <fstream>
#include "gameboy.hpp"
#include "types.hpp"
#include "opcodeNames.hpp"

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

    if (!cpu.paused) {
        uint cycles = 100;
        updateTimer(cycles);
        
        if (cpu.halted) {
            if (memory[cpu.IE] & memory[cpu.IF]) {
                cpu.halted = false;
            }
            return;
        }
        
        Byte opcode = cpu.loadByte(memory);
        cpu.executeInstructions(cycles, opcode, memory);
        cpu.handleInterrupt(memory);
    }
    
}

void Gameboy::updateTimer(uint cycles) {
    divCycles += cycles;
    if (divCycles >= 256) {
        divCycles -= 256;
        memory[cpu.DIV]++;
    }

    if ((memory[cpu.TAC] >> 2) & 1) {
        timaCycles += cycles * 4;

        int freq = 4096; // Hz
        if ((memory[cpu.IF] & 3) == 1) { // mask last 2 bits
            freq = 262144;
        } else if ((memory[cpu.IF] & 3) == 2) {
            freq = 65536;
        } else if ((memory[cpu.IF] & 3) == 3) {
            freq = 16384;
        }

        // increment tima based on synced gameboy freq (4.19 MHz)
        while (timaCycles >= (4194304 / freq)) {
            // increment TIMA
            memory[cpu.TIMA]++;
            // if TIMA overflows
            if (memory[cpu.TIMA] == 0x00) {
                // set timer interrupt request
                memory[cpu.IF] = memory[cpu.IF] | 4;
                // reset timer to timer modulo
                memory[cpu.TIMA] = memory[cpu.TMA];
            }
            timaCycles -= (4194304 / freq);
        }
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

void Gameboy::printMemory() {
    // bricks script, JUST for testing
    for (uint i = 0x0; i <= 0x10000; i++) {
        std::cout << "memory[" << i << "] = " << std::hex << (int)memory[i] << std::endl;
    }
}

/**
 * this was used to test the SST functions, which requires looping through a json file
 */
void Gameboy::testWithJson(std::string path) {
    std::ifstream f(path);

    if (f) {
        json opcodeTestData = json::parse(f);

        int failCount = 0;
        bool singleFile = true;

        for (int i = 0; i < opcodeTestData.size(); i++) {
            cpu.reset(memory);
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
            memory[cpu.IE] = opcodeTestData[i]["initial"]["ie"].get<Byte>();
            // load in the opcode to memory, along with necessary values
            for (int j = 0; j < opcodeTestData[i]["initial"]["ram"].size(); j++) {
                memory[opcodeTestData[i]["initial"]["ram"][j][0]] = opcodeTestData[i]["initial"]["ram"][j][1];
            }

            cpu.TEST_showAllRegValuesDecimal();

            // tick();
            uint cycles = 100;
            Byte opcode = cpu.loadByte(memory);
            cpu.executeInstructions(cycles, opcode, memory);


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
        // std::string name = opcodeTestData[0]["name"];
        // Byte opcodeNum = (Byte)std::stoi(name.substr(0, 2), nullptr, 16);
        // std::string opcodePrint = opcodeNames[opcodeNum];
        // std::cout << "finished testing :D, " << failCount << ", " << opcodePrint << " (" << name.substr(0, 2) << ")" << std::endl;
        std::cout << "finished testing :D, " << opcodeTestData[0]["name"] << ", " << failCount << std::endl;
    } else {
        std::cout << "error opening file" << std::endl;
    }

}