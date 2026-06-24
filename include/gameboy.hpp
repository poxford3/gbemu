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
        void start();
        void stop();
        void run();
        
    private:
        Cpu cpu;
        Mmu mmu;
        Ppu ppu;
        Apu apu;

        uint tickCpu();
        void updateGraphics(uint cycles);
        void updateTimer(uint cycles);
        void handleInterrupts();

        bool checksum();
        void testWithJson(std::string path);

};

#endif