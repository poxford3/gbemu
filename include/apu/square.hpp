#include "mmu.hpp"
#include "utils/types.hpp"

#ifndef SQUARE_HPP
#define SQUARE_HPP

class Square {
    public:
        float amplitude;
        int periodTimer;
        bool isEnvelopeIncreasing;
        float frequency;
        float frequencyTimer;
        float shadowFrequency;
        Byte dutyBits;
        Byte dutyCycle;
        bool hasSweep;
        Byte sweepTimer;
        Byte sweepPeriod;
        bool isSweepDecreasing;

        Byte NRx0;
        Byte NRx1;
        Byte NRx2;
        Byte NRx3;
        Byte NRx4;

        Square() : hasSweep(false) {}
        Square(bool hasSweep) : hasSweep(hasSweep) {}

        void update(Mmu &memory, uint &cycles);

    private:

        float newSweepFrequency(); // sweepShift is just NR10 & 0x7

};

#endif