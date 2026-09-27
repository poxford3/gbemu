#include <string>
#include <fstream>
#include "gameboy.hpp"
#include "utils/bit.hpp"
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
    ppu.reset();
};

void Gameboy::reset() {
    cpu.reset();
    mmu.reset();
    ppu.reset();
    checksumPassed = checksum();
}


Gameboy::~Gameboy() {}


void Gameboy::runFrame() {
    if (checksumPassed) {
        uint frameCycles = 0;
        while (frameCycles <= CYCLES_PER_FRAME) { // 70224 cycles per frame
            uint cycles = tickCpu();
            updateTimer(cycles);
            updateGraphics(cycles);
            handleInterrupts();
            frameCycles += cycles;
        }
    } else {
        printf("checksum failed, game will not run.\n");
    }
}


void Gameboy::joyPadProcessor(Byte input, Byte type, bool keyUp) {
    std::function<Byte(Byte, Byte)> bitFunc = keyUp 
    ? static_cast<Byte(*)(Byte, Byte)>(setBit) // if keyUp, set the bit to 1 to mark it as inactive
    : static_cast<Byte(*)(Byte, Byte)>(resetBit);

    if (type == BUTTON_SELECT) {
        mmu.buttons = bitFunc(mmu.buttons, input);
    } else if (type == DPAD_SELECT) {
        mmu.dpad = bitFunc(mmu.dpad, input);
    }
}


void Gameboy::handleInput(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) { // reset bit to mark it as active
            case SDL_SCANCODE_UP:
                joyPadProcessor(SELECT_UP, DPAD_SELECT, false); // set bit 2 to 0 for up and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_DOWN:
                joyPadProcessor(START_DOWN, DPAD_SELECT, false); // set bit 3 to 0 for down and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_LEFT:
                joyPadProcessor(B_LEFT, DPAD_SELECT, false); // set bit 1 to 0 for left and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_RIGHT:
                joyPadProcessor(A_RIGHT, DPAD_SELECT, false); // set bit 0 to 0 for right and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_Z: // A
                joyPadProcessor(A_RIGHT, BUTTON_SELECT, false); // set bit 0 to 0 for select and bit 5 to 0 for buttons
                break;
            case SDL_SCANCODE_X: // B
                joyPadProcessor(B_LEFT, BUTTON_SELECT, false); // set bit 1 to 0 for select and bit 5 to 0 for buttons
                break;
            case SDL_SCANCODE_RETURN: // Start
                joyPadProcessor(START_DOWN, BUTTON_SELECT, false); // set bit 3 to 0 for select and bit 5 to 0 for buttons
                break;
            case SDL_SCANCODE_RSHIFT: // Select
                joyPadProcessor(SELECT_UP, BUTTON_SELECT, false); // set bit 2 to 0 for select and bit 5 to 0 for buttons
                break;
            default:
                break;
        }
    }
        if (event.type == SDL_KEYUP) { // up means off
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                joyPadProcessor(SELECT_UP, DPAD_SELECT, true); // set bit 2 to 1 for up and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_DOWN:
                joyPadProcessor(START_DOWN, DPAD_SELECT, true); // set bit 3 to 1 for down and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_LEFT:
                joyPadProcessor(B_LEFT, DPAD_SELECT, true); // set bit 1 to 1 for left and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_RIGHT:
                joyPadProcessor(A_RIGHT, DPAD_SELECT, true); // set bit 0 to 1 for right and bit 4 to 1 for dpad
                break;
            case SDL_SCANCODE_Z: // A
                joyPadProcessor(A_RIGHT, BUTTON_SELECT, true); // set bit 0 to 1 for A and bit 5 to 1 for buttons
                break;
            case SDL_SCANCODE_X: // B
                joyPadProcessor(B_LEFT, BUTTON_SELECT, true); // set bit 1 to 1 for B and bit 5 to 1 for buttons
                break;
            case SDL_SCANCODE_RETURN: // Start
                joyPadProcessor(START_DOWN, BUTTON_SELECT, true); // set bit 3 to 1 for start and bit 5 to 1 for buttons
                break;
            case SDL_SCANCODE_RSHIFT: // Select
                joyPadProcessor(SELECT_UP, BUTTON_SELECT, true); // set bit 2 to 1 for select and bit 5 to 1 for buttons
                break;
            default:
                break;
        }
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

    // if ((mmu.readByte(mmu.TAC) >> 2) & 1) {
    if (getBit(mmu.readByte(mmu.TAC), 2)) {
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
    ppu.updateGraphics(mmu, cycles);
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
                case Cpu::VBLANK:    cpu.PC = 0x0040; break; // VBlank
                case Cpu::LCD_STAT:  cpu.PC = 0x0048; break; // LCD
                case Cpu::TIMER:     cpu.PC = 0x0050; break; // Timer
                case Cpu::SERIAL:    cpu.PC = 0x0058; break; // Serial
                case Cpu::JOYPAD:    cpu.PC = 0x0060; break; // Joypad
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
 * @deprecated
 */
[[deprecated("This was used in early testing, just look at things on screen or run CPU tests")]]
void Gameboy::testWithJson(std::string path) {
    std::ifstream f(path);

    if (f) {
        json opcodeTestData = json::parse(f);

        int failCount = 0;
        bool singleFile = true;

        for (int i = 0; i < opcodeTestData.size(); i++) {
            cpu.reset();
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