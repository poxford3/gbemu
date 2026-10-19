#include "square.hpp"

void Square::update(Mmu &memory, uint &cycles, int &currentVolume) {
    if (hasSweep) {
        NR10 = memory.getIoRegister(Mmu::NR10);
        NRx1 = memory.getIoRegister(Mmu::NR11);
        NRx2 = memory.getIoRegister(Mmu::NR12);
        NRx3 = memory.getIoRegister(Mmu::NR13);
        NRx4 = memory.getIoRegister(Mmu::NR14);

        sweepPeriod = (NR10 >> 4) & 0x7; // bits 6-4
        isSweepDecreasing = getBit(NR10, 3);
        sweep();
    } else {
        NRx1 = memory.getIoRegister(Mmu::NR21);
        NRx2 = memory.getIoRegister(Mmu::NR22);
        NRx3 = memory.getIoRegister(Mmu::NR23);
        NRx4 = memory.getIoRegister(Mmu::NR24);
    }

    dutyBits = ((NRx1 >> 6) & 0x3); // top 2 bits
    // dutyCycle = dutyTable[dutyBits];
    // int pos = 4;
    // amplitude = getBit(dutyCycle, pos);

    // each M cycle (cycle provided to components from CPU): Freq timer decremented by 4
    frequencyTimer = (PERIOD_BOUNDS - frequency) * 4;

    // envelope
    Byte initialVolume = (NRx2 >> 4) & 0xF; // bits 7-4
    isEnvelopeIncreasing = getBit(NRx2, 3);
    Byte stepPeriod = NRx2 & 0x7;
    bool triggerEvent = getBit(NRx4, 7);

    currentVolume += envelopeVolume(currentVolume);

    // duty cycle output * currentVolume
    float dacInput;

    float dacOutput = (dacInput / 7.5) - 1.f;
    // Length
    // whenever apuCounters[LENGTH_CTRL] goes up AND NRx4.6 -> length timer is decremented by 1
    Byte lengthTimer = 64 - (NRx1 & 0x3F); // bottom 6 bits (256 - x in the case of channel 3, all others are 64 - x)
    if (lengthTimer == 0) {
        // channel disabled until next trigger event
        channel_enabled = false;
    }
}


float Square::newSweepFrequency(Byte &sweepShift) {
    float newFrequency = shadowFrequency / (1 << sweepShift); // new freq = shadowFrequency / (2^sweepShift)
    if (isSweepDecreasing) {
        newFrequency -= shadowFrequency;
    } else {
        newFrequency += shadowFrequency;
    }

    if (newFrequency > 2047) {
        channel_enabled = false;
    }

    return newFrequency;
}


float Square::sweep() {
    Byte sweepShift = NR10 & 0x7; // bottom 3 bits
    if (sweepTimer > 0) sweepTimer -= 1;

    if (sweepTimer == 0) {
        if (sweepPeriod > 0) {
            sweepTimer = sweepPeriod;
        } else {
            sweepTimer = 0;
        }

        if (sweepEnabled && sweepPeriod > 0) {
            float newFrequency = newSweepFrequency(sweepShift);

            if (newFrequency <= 2047 && sweepShift > 0) {
                frequency = newFrequency;
                shadowFrequency = newFrequency;

                /* for overflow check */
                newSweepFrequency(sweepShift);
            }
        }
    }
}


Word Square::setPeriod(Byte &NRx4, Byte &NRx3) {
    // takes top 3 bits from NRx4 and makes an 11 bit period with NRx3
    period = ((NRx4 & 0x7) << 8) | NRx3;
}


float Square::getSampleRateAndFrequency() {
    // https://gbdev.io/pandocs/Audio_Registers.html#ff13--nr13-channel-1-period-low-write-only
    sampleRate = (float)GAMEBOY_FREQUENCY / (PERIOD_BOUNDS - period);
    frequency = sampleRate / 8;
}


int Square::envelopeVolume(int &currentVolume) {
    if (period != 0) {
        if (periodTimer > 0) {
            periodTimer--;
        }

        if (periodTimer == 0) {
            periodTimer = period;

            if (((currentVolume < 0xF) && isEnvelopeIncreasing) || 
                (currentVolume > 0x0) && !isEnvelopeIncreasing) {
                    return isEnvelopeIncreasing ? 1 : -1;
            }
        }
    }
    return 0;
}
