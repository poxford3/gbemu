#include "cpu.hpp"
#include "ppu.hpp"
#include "types.hpp"

#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

class Gameboy {
    public:
        Mem memory;
        bool checksumPassed = false;
        int divCycles = 0;   // counts cycles until DIV increments (every 256 cycles)
        int timaCycles = 0;  // counts cycles until TIMA increments (based on TAC speed)

        Gameboy();
        Gameboy(const std::vector<Byte>& program);
        void start();
        void stop();
        uint tick();
        bool checksum();
        void printMemory();
        void testWithJson(std::string path);
        void updateTimer(uint cycles);
        Cpu cpu;
        Ppu ppu;

    private:
        // std::vector<Byte> program;

};

#endif