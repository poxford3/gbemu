#include "cpu.hpp"
#include "types.hpp"

#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

struct Gameboy {
    public:
        Gameboy();
        Gameboy(const std::vector<Byte>& program);
        void start();
        void stop();
        void tick();
        bool checksum();

    private:
        Mem memory;
        Cpu cpu;
        // std::vector<Byte> program;
        bool checksumPassed = false;

};

#endif