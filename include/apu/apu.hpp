#include <iostream>
#include "mmu.hpp"
#include "channel.hpp"
#include "utils/types.hpp"
#include "utils/bit.hpp"

#ifndef APU_HPP
#define APU_HPP

class Apu {
    public:
        void updateSound(Mmu &memory, uint &cycles);
        void reset();
        Apu(): apuCounters{0,0,0,0} {}
        ~Apu();

    private:
        std::array<Byte, 4> dutyTable = {
            0b00000001, // 12.5%
            0b00000011, // 25%
            0b00001111, // 50%
            0b11111100, // 75%
        };

        const int GAMEBOY_FREQUENCY = 1'048'576;
        const Word PERIOD_BOUNDS = 2048;

        int currentVolume;
        Channel1 channel1;
        Channel2 channel2;
        Channel3 channel3;
        Channel4 channel4;
        bool isCgb2xSpeed = false;

        // used to track frequencies of other values from APU Counters
        std::array<Byte, 4> apuCounters;

        enum APUCounters {
            FS                  = 0, // 512 Hz
            LENGTH_CTRL         = 1, // 256 Hz
            SWEEP               = 2, // 128 Hz
            VOLUME_ENVELOPE     = 3  // 64 Hz 
        };

        void frameSequencer(Byte &div);
        float mixer();
        


};

#endif // APU_HPP