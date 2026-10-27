#include <iostream>
#include "mmu.hpp"
#include "channel.hpp"
#include "utils/types.hpp"
#include "utils/bit.hpp"

#ifndef APU_HPP
#define APU_HPP

class Apu {
    public:
        Apu(): frameSeqCounter{0} {}
        ~Apu();
        void reset();
        void updateSound(Mmu &memory, uint &cycles);

        enum APUCounters {
            FS                  = 0, // 512 Hz
            LENGTH_CTRL         = 1, // 256 Hz
            SWEEP               = 2, // 128 Hz
            VOLUME_ENVELOPE     = 3  // 64 Hz 
        };

        enum NR52 {
            Ch1On = 0,
            Ch2On = 1,
            Ch3On = 2,
            Ch4On = 3,
            AudioOn = 7
        };

    private:
        Word audioCycles; // tracks cycles hit by the APU (not every cycle comes in and is ran)

        int currentVolume;
        // Channel1 channel1;
        Channel2 channel2;
        // Channel3 channel3;
        // Channel4 channel4;
        bool isCgb2xSpeed = false;

        Byte frameSeqCounter;

        void tickTrameSequencer(Mmu &memory);
        void tickLength(Mmu &memory);
        void tickSweep(Mmu &memory);
        void tickEnvelopeVolume(Mmu &memory);
        float mixer();
        


};

#endif // APU_HPP