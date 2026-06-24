#include <string>
#include <fstream>
#include "gameboy.hpp"
#include "utils/types.hpp"
#include "cpu/opcodeNames.hpp"

Gameboy::Gameboy() {
    start();
}

Gameboy::Gameboy(const std::vector<Byte>& program) {
    start();
    mmu.loadRom(program);
    checksumPassed = checksum();
}

void Gameboy::start() {
    cpu.reset();
    mmu.reset();
    ppu.init();
};


void Gameboy::stop() {
    // todo
}


void Gameboy::run() {
    while (ppu.running) {
        uint cycles = tickCpu();
        updateTimer(cycles);
        updateGraphics(cycles);
        handleInterrupts();
    }
}


uint Gameboy::tickCpu() {

    if (!cpu.paused) {

        if (cpu.pendingIME) {
            cpu.IME = true;
            cpu.pendingIME = false;
        }

        if (cpu.halted) {
            if (mmu.interruptEnableRegister & mmu.readByte(mmu.IF)) {
                    cpu.halted = false;
                } else {
                    return 4;
            }
        }

        Byte opcode = cpu.loadByte(mmu);
        // printf("opcode %02X, PC %02X\n", opcode, cpu.PC);
        uint cycles = cpu.executeInstructions(opcode, mmu);
        return cycles;
    }
    return -1;
}


void Gameboy::updateTimer(uint cycles) {
    // research this functionality
    divCycles += cycles;
    if (divCycles >= 256) {
        divCycles = 0;
        mmu.ioRegisters[mmu.DIV - 0xFF00]++; // cannot directly write to DIV but must still increment
    }

    if ((mmu.readByte(mmu.TAC) >> 2) & 1) {
        timaCycles += cycles * 4;

        int freq = 4096; // Hz
        switch (mmu.readByte(mmu.TAC) & 0x03) {
            case 0: freq = 4096; break;
            case 1: freq = 262144; break;
            case 2: freq = 65536; break;
            case 3: freq = 16384; break;

        }

        // increment tima based on synced gameboy freq (4.19 MHz)
        while (timaCycles >= (4194304 / freq)) {
            // increment TIMA
            mmu.writeByte(mmu.TIMA, mmu.readByte(mmu.TIMA) + 1);
            // if TIMA overflows
            if (mmu.readByte(mmu.TIMA) == 0x00) {
                // set timer interrupt request
                mmu.writeByte(mmu.IF, mmu.readByte(mmu.IF) | 4);
                // reset timer to timer modulo
                mmu.writeByte(mmu.TIMA, mmu.readByte(mmu.TMA));
            }
            timaCycles -= (4194304 / freq);
        }
    }
}


void Gameboy::updateGraphics(uint cycles) {
    ppu.updateGraphics(cpu, mmu, cycles);
}


void Gameboy::handleInterrupts() {
    Byte pending = mmu.interruptEnableRegister & mmu.readByte(Mmu::IF);
    if (pending == 0) return;
    
    cpu.halted = false;
    if (!cpu.IME) {
        // printf("interrupt pending but IME=0, IE=%02X IF=%02X\n", memory.interruptEnableRegister, memory.ioRegisters[Mmu::IF]);
        return;
    } // if the master interrupt says there are no interrupts, then we just move on
    cpu.IME = false;

    for (int i = 0; i < 5; i++) {
        if (pending & (1 << i)) {
            // printf("handling interrupt %d, PC=0x%04X IME=%d\n", i, PC, IME);
            mmu.writeByte(Mmu::IF, mmu.readByte(Mmu::IF) & ~(1 << i)); // reset the interrupt request bit for this interrupt
            cpu.pushRegToStack(cpu.PC, mmu);

            switch (i) {
                case 0: cpu.PC = 0x0040; break; // VBlank
                case 1: cpu.PC = 0x0048; break; // LCD
                case 2: cpu.PC = 0x0050; break; // Timer
                case 3: cpu.PC = 0x0058; break; // Serial
                case 4: cpu.PC = 0x0060; break; // Joypad
            }
            // cycles -= 20; // implemented once the clock is done
            return;
        }
    }
}


bool Gameboy::checksum() {
    // https://gbdev.io/pandocs/The_Cartridge_Header.html?highlight=checksum#014d--header-checksum
    Byte checksum = 0;
    for (Word address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - mmu.readByte(address) - 1;
    }
    // if the memory address at 0x014D matches bottom 8 bits of checksum, header passes
    return mmu.readByte(0x014D) == (checksum & 0xFF) ? true : false;
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
            cpu.reset();
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
            mmu.writeByte(mmu.interruptEnableRegister, opcodeTestData[i]["initial"]["ie"].get<Byte>());
            // load in the opcode to memory, along with necessary values
            for (int j = 0; j < opcodeTestData[i]["initial"]["ram"].size(); j++) {
                mmu.writeByte(opcodeTestData[i]["initial"]["ram"][j][0], opcodeTestData[i]["initial"]["ram"][j][1]);
            }

            cpu.TEST_showAllRegValuesDecimal();

            // tick();
            Byte opcode = cpu.loadByte(mmu);
            uint cycles = cpu.executeInstructions(opcode, mmu);


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
                // || mmu.readByte(opcodeTestData[i]["final"]["ram"][0][0]) != opcodeTestData[i]["final"]["ram"][0][1]
            ) {
                failCount++;
                singleFile && std::cout << "error executing operation out mem, " << opcodeTestData[i]["name"] << std::endl;
                bool memoryError = false;
                for (int j = 0; j < opcodeTestData[i]["final"]["ram"].size(); j++) {
                    if (mmu.readByte(opcodeTestData[i]["final"]["ram"][j][0]) != opcodeTestData[i]["final"]["ram"][j][1]) {
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
        std::cout << "error opening file, json" << std::endl;
    }

}