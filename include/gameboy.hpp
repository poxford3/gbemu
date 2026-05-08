#include "cpu.hpp"
#include "types.hpp"

#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

struct Gameboy {
    public:
        bool checksumPassed = false;

        Gameboy();
        Gameboy(const std::vector<Byte>& program);
        void start();
        void stop();
        void tick();
        bool checksum();
        void printMemory();

    private:
        Mem memory;
        Cpu cpu;
        // std::vector<Byte> program;

};

#endif