#include "mmu.hpp"
#include "utils/types.hpp"

#ifndef SQUARE_HPP
#define SQUARE_HPP

/*
    Square Wave
*/
class Square {
    public:
        Square(const int &GAMEBOY_FREQUENCY, const Word &PERIOD_BOUNDS) : 
            hasSweep(false),
            GAMEBOY_FREQUENCY(GAMEBOY_FREQUENCY),
            PERIOD_BOUNDS(PERIOD_BOUNDS)
        {}
        Square(bool hasSweep, const int &GAMEBOY_FREQUENCY, const Word &PERIOD_BOUNDS) :
            hasSweep(hasSweep),
            GAMEBOY_FREQUENCY(GAMEBOY_FREQUENCY),
            PERIOD_BOUNDS(PERIOD_BOUNDS)
        {}

        float amplitude;
        void update(Mmu &memory, uint &cycles, int &currentVolume);

    private:
        bool channel_enabled;
        bool isEnvelopeIncreasing;
        float frequency;
        float frequencyTimer;
        float shadowFrequency;
        Word period;
        int periodTimer;
        float sampleRate;
        float frequency;
        Byte dutyBits;
        Byte dutyCycle;
        bool hasSweep;
        Byte sweepTimer;
        Byte sweepPeriod;
        bool sweepEnabled;
        bool isSweepDecreasing;

        const int GAMEBOY_FREQUENCY;
        const Word PERIOD_BOUNDS;

        Byte NR10;
        Byte NRx1;
        Byte NRx2;
        Byte NRx3;
        Byte NRx4;
    
        float sweep();
        float newSweepFrequency(Byte &sweepShift);

        Word setPeriod(Byte &NRx4, Byte &NRx3);
        float getSampleRateAndFrequency();
        int envelopeVolume(int &currentVolume);

};

#endif