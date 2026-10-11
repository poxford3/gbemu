#include "apu/apu.hpp"

// Apu::Apu() {
//     currentVolume = 0x3;

//     // channel1.updateRegisters()
// }


Apu::~Apu() {}


void Apu::reset() {}


void Apu::frameSequencer(Byte &div) {
    bool div_counter = isCgb2xSpeed ? getBit(div, 6) : getBit(div, 5);

    if (div_counter) {
        apuCounters[FS]++;
        if (apuCounters[FS] % 2 == 0) apuCounters[LENGTH_CTRL]++;
        if (apuCounters[FS] % 4 == 0) apuCounters[SWEEP]++;
        if (apuCounters[FS] % 8 == 0) apuCounters[VOLUME_ENVELOPE]++;
    }
}


int Apu::envelopeVolume(Word &period, Square &channel) {
    if (period != 0) {
        if (channel.periodTimer > 0) {
            channel.periodTimer--;
        }

        if (channel.periodTimer == 0) {
            channel.periodTimer = period;

            if (((currentVolume < 0xF) && channel.isEnvelopeIncreasing) || 
                (currentVolume > 0x0) && !channel.isEnvelopeIncreasing) {
                    return channel.isEnvelopeIncreasing ? 1 : -1;
            }
        }
    }
    return 0;
}


float Apu::newSweepFrequency(Byte &sweepShift) {
    bool channel_enabled = true; // todo get rid of this, will come from other data
    float newFrequency = 0;
    // float newFrequency = shadowFrequency >> sweepShift; // todo how to do bit shift for float
    if (channel1.isSweepDecreasing) {
        newFrequency -= channel1.shadowFrequency;
    } else {
        newFrequency += channel1.shadowFrequency;
    }

    if (newFrequency > 2047) {
        channel_enabled = false;
    }

    return newFrequency;
}


float Apu::sweep(Byte &sweepShift, bool &sweepEnabled) {
    if (channel1.sweepTimer > 0) channel1.sweepTimer -= 1;

    if (channel1.sweepTimer == 0) {
        if (channel1.sweepPeriod > 0) {
            channel1.sweepTimer = channel1.sweepPeriod;
        } else {
            channel1.sweepTimer = 0;
        }

        if (sweepEnabled && channel1.sweepPeriod > 0) {
            float newFrequency = newSweepFrequency(sweepShift);

            if (newFrequency <= 2047 && sweepShift > 0) {
                channel1.frequency = newFrequency;
                channel1.shadowFrequency = newFrequency;

                /* for overflow check */
                newSweepFrequency(sweepShift);
            }
        }
    }
}


Word Apu::getPeriod(Byte &NRx4, Byte &NRx3) {
    return ((NRx4 & 0x7) << 8) | NRx3;
}


float Apu::getSampleRate(Word period) {
    return (float)GAMEBOY_FREQUENCY / (period_bounds - period);
}


void Apu::updateSound(Mmu &memory, uint &cycles) {
    Byte NR52 = memory.readByte(Mmu::NR52); // audio master control
    Byte NR10 = memory.readByte(Mmu::NR10);
    Byte NR11 = memory.readByte(Mmu::NR11);
    Byte NR12 = memory.readByte(Mmu::NR12);
    Byte NR13 = memory.readByte(Mmu::NR13);
    Byte NR14 = memory.readByte(Mmu::NR14);

    bool audioOn = getBit(NR52, 7);
    bool channel1On = getBit(NR52, 0);

    if (audioOn && channel1On) {

        Byte div = memory.getIoRegister(Mmu::DIV);

        frameSequencer(div);
        // channel1.update(memory, cycles);

        Byte chan1DutyBits = ((NR11 >> 6) & 0x3); // top 2 bits
        Byte chan1DutyCycle = dutyTable[chan1DutyBits];
        int pos = 4; // todo, this will be the position in the wave of the duty table
        Byte chan1Amplitude = getBit(chan1DutyCycle, pos);
    
        Word chan1Period = getPeriod(NR14, NR13);
        float chan1SampleRate = getSampleRate(chan1Period);
        float chan1Frequency = chan1SampleRate / 8;
    
        // each M cycle (cycle provided to components from CPU): Freq timer decremented by 4
        float chan1FreqTimer = (period_bounds - chan1Frequency) * 4;
    
        // envelope
        Byte initialVolume = (NR12 >> 4) & 0xF; // bits 7-4
        channel1.isEnvelopeIncreasing = getBit(NR12, 3);
        Byte chan1StepPeriod = NR12 & 0x7;
        channel1.periodTimer = 0;
        int currentVolume = 0x3;
        bool triggerEvent = getBit(NR14, 7);
    
        currentVolume += envelopeVolume(chan1Period, channel1);
    
        // duty cycle output * currentVolume
        float dacInput;
    
        float dacOutput = (dacInput / 7.5) - 1.f;
    
        // Length
        // whenever apuCounters[LENGTH_CTRL] goes up AND NRx4.6 -> length timer is decremented by 1
        Byte lengthTimer = 64 - (NR11 & 0x3F); // bottom 6 bits (256 - x in the case of channel 3, all others are 64 - x)
        if (lengthTimer == 0) {
            // channel disabled until next trigger event
        }
    
        // Sweep
        channel1.sweepPeriod = (NR10 >> 4) & 0x7; // bits 6-4
        channel1.isSweepDecreasing = getBit(NR10, 3);
        Byte sweepShift = NR10 & 0x7; // bits 2-0, the step by step change
        bool sweepEnabled = false;
        sweep(sweepShift, sweepEnabled);


        mixer();
    }
}


float Apu::mixer() {
    return (channel1.amplitude) / 1.f;
}