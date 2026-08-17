#include "cpu/cpu.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "mmu.hpp"
#include "utils/types.hpp"

#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

class Gameboy {
    public:
        bool checksumPassed = false;
        int divCycles = 0;   // counts cycles until DIV increments (every 256 cycles)
        int timaCycles = 0;  // counts cycles until TIMA increments (based on TAC speed)

        Gameboy();
        Gameboy(const std::vector<Byte>& program);
        ~Gameboy();
        void start();
        void reset();
        void runFrame();
        void handleInput(SDL_Event &event);
        Cpu cpu;
        Mmu mmu;
        Ppu ppu;
        Apu apu;

        
        enum JoypadInput {
            BUTTON_SELECT = 5,
            DPAD_SELECT = 4,
            START_DOWN = 3,
            SELECT_UP = 2,
            B_LEFT = 1,
            A_RIGHT = 0
        };
        
    private:
        uint tickCpu();
        void updateGraphics(uint cycles);
        void updateTimer(uint cycles);
        void handleInterrupts();

        void joyPadProcessor(Byte input, Byte type, bool keyUp); 

        bool checksum();
        void testWithJson(std::string path);
};

#endif