#include "mmu.hpp"

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

class Channel {

    public:

        const int GAMEBOY_FREQUENCY = 1'048'576;
        const Word PERIOD_BOUNDS = 2048;
        float amplitude = 0.f;

        virtual void tick(Mmu &memory) = 0;
        virtual void reset() = 0;
        virtual void tickLength(Mmu &memory) = 0;
        virtual void setEnabled(bool enabled, Mmu &memory) = 0;
        virtual void trigger(Mmu &memory) = 0;

        virtual ~Channel() = default;

        Byte NRx1;
        Byte NRx2;
        Byte NRx3;
        Byte NRx4;

        bool channelEnabled;
        Byte lengthTimer;
        bool left, right;
        bool DACenabled;

        Byte dutyBit;
        Byte selectedDutyCycle;
        std::array<Byte, 4> dutyTable = {
            0b00000001, // 12.5%
            0b00000011, // 25%
            0b00001111, // 50%
            0b11111100, // 75%
        };
};

#endif


#ifndef CHANNEL1_HPP
#define CHANNEL1_HPP

class Channel1 : public Channel {

    public:

        Channel1();
        void tick(Mmu &memory);
        void reset();
        void tickLength(Mmu &memory);
        void setEnabled(bool enabled, Mmu &memory);
        void trigger(Mmu &memory);
        

    private:
        bool isEnvelopeIncreasing;
        Word envelopePeriodTimer;
        int envelopeCurrentVolume;
        bool envelopeUpdating;
        // envelope sweep pace
        // Byte stepPeriodPace; // amount that the envelope changes per cycle (?)
        Byte initialVolume;
        Word period;
        int periodTimer;

        Byte NR10;
        float sweep();
        float newSweepFrequency(Byte &sweepShift);
        int envelopeVolume(int &currentVolume);
        float setSampleRateAndFrequency();


};

#endif


#ifndef CHANNEL2_HPP
#define CHANNEL2_HPP

class Channel2 : public Channel {

    public:

        Channel2();
        void tick(Mmu &memory);
        void reset();
        void tickLength(Mmu &memory);
        void tickEnvelopeVolume(Mmu &memory);
        void setEnabled(bool enabled, Mmu &memory);

    private:
        bool isEnvelopeIncreasing;
        Word envelopePeriodTimer;
        int envelopeCurrentVolume;
        bool envelopeUpdating;
        // envelope sweep pace
        // Byte stepPeriodPace; // amount that the envelope changes per cycle (?)
        Byte initialVolume;
        Word period;
        int periodTimer;


        void trigger(Mmu &memory);
        void setDacEnabled();
        void setPeriod();
};

#endif


#ifndef CHANNEL3_HPP
#define CHANNEL3_HPP

class Channel3 : public Channel {
    public:
        Channel3();
        void tick(Mmu &memory);
        void reset();
        void tickLength(Mmu &memory);
        void setEnabled(bool enabled, Mmu &memory);
        void trigger(Mmu &memory);
    private:
};

#endif


#ifndef CHANNEL4_HPP
#define CHANNEL4_HPP

class Channel4 : public Channel {
    public:
        Channel4();
        void tick(Mmu &memory);
        void reset();
        void tickLength(Mmu &memory);
        void setEnabled(bool enabled, Mmu &memory);
        void trigger(Mmu &memory);
    private:
};

#endif