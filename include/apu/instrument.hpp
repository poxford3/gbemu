#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

class Instrument {

    public:
        int periodTimer;
        bool isEnvelopeIncreasing;
        float frequency;
        float shadowFrequency;
        bool hasSweep;
        Byte sweepTimer;
        Byte sweepPeriod;
        bool isSweepDecreasing;

        Instrument() : 
        periodTimer(0),
        isEnvelopeIncreasing(false),
        frequency(0.f),
        shadowFrequency(0.f),
        sweepTimer(0),
        sweepPeriod(0),
        isSweepDecreasing(false),
        hasSweep(false)
        {}


        Instrument(bool hasSweep) : 
        periodTimer(0),
        isEnvelopeIncreasing(false),
        frequency(0.f),
        shadowFrequency(0.f),
        sweepTimer(0),
        sweepPeriod(0),
        isSweepDecreasing(false),
        hasSweep(hasSweep)
        {}

    private:


};

#endif