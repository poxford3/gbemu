#include "iostream"
#include "mmu.hpp"
#include "instrument.hpp"
#include "square.hpp"
#include "utils/types.hpp"
#include "utils/bit.hpp"

#ifndef APU_HPP
#define APU_HPP

class Apu {
    public:
        void updateSound(Mmu &memory, uint &cycles);
        void reset();
        Apu(): apuCounters{0,0,0,0} {}
        Apu(Mmu &memory): apuCounters{0,0,0,0} {}
        ~Apu();

    private:
        std::array<Byte, 4> dutyTable = {
            0b00000001, // 12.5%
            0b00000011, // 25%
            0b00001111, // 50%
            0b11111100, // 75%
        };

        const int GAMEBOY_FREQUENCY = 1'048'576;
        const Word period_bounds = 2048;

        int currentVolume;
        Square channel1{true};
        Square channel2;
        bool isCgb2xSpeed = false;

        // used to track frequencies of other values from APU Counters
        std::array<Byte, 4> apuCounters;

        enum APUCounters {
            FS                  = 0, // 512 Hz
            LENGTH_CTRL         = 1, // 256 Hz
            SWEEP               = 2, // 128 Hz
            VOLUME_ENVELOPE     = 3  // 64 Hz 
        };

        Word getPeriod(Byte &NRx4, Byte &NRx3);
        float getSampleRate(Word period);
        void frameSequencer(Byte &div);
        int envelopeVolume(Word &period, Square &channel);
        float newSweepFrequency(Byte &sweepShift);
        float sweep(Byte &sweepShift, bool &sweepEnabled);
        float mixer();
        


};

#endif // APU_HPP