#include "cpu.hpp"
#include "types.hpp"

#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

struct Gameboy {
    public:
        bool checksumPassed = false;
        int divCycles = 0;   // counts cycles until DIV increments (every 256 cycles)
        int timaCycles = 0;  // counts cycles until TIMA increments (based on TAC speed)

        Gameboy();
        Gameboy(const std::vector<Byte>& program);
        void start();
        void stop();
        void tick();
        bool checksum();
        void printMemory();
        void testWithJson(std::string path);
        void updateTimer(uint cycles);

    private:
        Mem memory;
        Cpu cpu;
        // std::vector<Byte> program;

};

#endif