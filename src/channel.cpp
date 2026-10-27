#include "apu/apu.hpp"
#include "apu/channel.hpp"
#include "utils/types.hpp"
#include "utils/bit.hpp"


// Channel 1
#pragma region
/*
float Channel1::newSweepFrequency(Byte &sweepShift) {
    float newFrequency = shadowFrequency / (1 << sweepShift); // new freq = shadowFrequency / (2^sweepShift)
    if (isSweepDecreasing) {
        newFrequency -= shadowFrequency;
    } else {
        newFrequency += shadowFrequency;
    }

    if (newFrequency > 2047) {
        channelEnabled = false;
    }

    return newFrequency;
}


float Channel1::sweep() {
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

                // for overflow check
                newSweepFrequency(sweepShift);
            }
        }
    }
}

*/


#pragma endregion


// Channel 2
#pragma region

Channel2::Channel2() : Channel() {}


void Channel2::reset() {

}


void Channel2::tick(Mmu &memory) {

    channelEnabled = getBit(memory.getIoRegister(Mmu::NR52), Apu::NR52::Ch2On);

    NRx1 = memory.getIoRegister(Mmu::NR21);
    NRx2 = memory.getIoRegister(Mmu::NR22);
    NRx3 = memory.getIoRegister(Mmu::NR23);
    NRx4 = memory.getIoRegister(Mmu::NR24);

    // initialize values from registers
    initialVolume = (NRx2 >> 4) & 0xF; // bits 7-4
    isEnvelopeIncreasing = getBit(NRx2, 3);
    selectedDutyCycle = (NRx1 >> 6) & 0x3; // top 2 bits
    lengthTimer = 64 - (NRx1 & 0x3F); // bottom 6 bits (256 - x in the case of channel 3, all others are 64 - x)
    if (lengthTimer == 0) {
        // channel disabled until next trigger event
        setEnabled(false, memory);
    }

    setPeriod();

    if (getBit(NRx4, 7)) trigger(memory);

    if (!DACenabled) {
        setEnabled(false, memory);
    }

    if (channelEnabled) {
        periodTimer--;
        if (periodTimer <= 0) {
            periodTimer = (PERIOD_BOUNDS - period) * 4;
            // dutyBit = (dutyBit + 1) % 8;
            ++dutyBit %= 8; // add 1 then mod 8
        }
        amplitude = getBit(dutyTable[selectedDutyCycle], dutyBit);
        amplitude *= memory.ch2EnvelopeCurrentVolume;
    } else if (DACenabled) {
        amplitude = 0xF;
    } else {
        amplitude = 0x0;
    }
}


/*
protected void trigger() {
    if (this.getDacEnable()) {
        this.setEnabled(true);
    }
    if (this.lengthTimer == 0) {
        this.lengthTimer = this.getMaxLengthTimer();
        if (this.getLengthEnable() && !isLengthClockStep()) {
            this.clockLength();
        }
    }
}

protected void clockLength() {
    if (!this.getLengthEnable()) {
        return;
    }
    if (this.lengthTimer <= 0) {
        return;
    }
    this.lengthTimer--;
    if (this.lengthTimer <= 0) {
        this.setEnabled(false);
    }
}
*/

void Channel2::trigger(Mmu &memory) {
    if (DACenabled) setEnabled(true, memory);

    if (memory.ch2LengthTimer == 0) {
        memory.ch2LengthTimer = 64;
    }

    // each M cycle (cycle provided to components from CPU): period timer decremented by 4
    periodTimer = (PERIOD_BOUNDS - period) * 4;
    envelopePeriodTimer = memory.ch2stepPeriodPace;
    memory.ch2EnvelopeCurrentVolume = initialVolume;
    dutyBit = 0;
    memory.ch2EnvelopeUpdating = false;
}


void Channel2::tickEnvelopeVolume(Mmu &memory) {
    if (!channelEnabled) return;

    if (period != 0) {
        if (periodTimer > 0) {
            periodTimer--;
        }

        if (periodTimer == 0) {
            periodTimer = period;

            if (((envelopeCurrentVolume < 0xF) && isEnvelopeIncreasing) || 
                (envelopeCurrentVolume > 0x0) && !isEnvelopeIncreasing) {
                    envelopeCurrentVolume += isEnvelopeIncreasing ? 1 : -1;
            } else {
                memory.ch2EnvelopeUpdating = false;
            }
        }
    }
}


void Channel2::tickLength(Mmu &memory) {
    if (memory.ch2LengthTimer <= 0) return;
    memory.ch2LengthTimer--;
    if (memory.ch2LengthTimer <= 0) setEnabled(false, memory);
}


void Channel2::setEnabled(bool enabled, Mmu &memory) {
    if (enabled) {
        channelEnabled = enabled;
        // setIoRegister of NR52, setting the ChannelOn bit
        memory.setIoRegister(Mmu::NR52, setBit(memory.getIoRegister(Mmu::NR52), Apu::NR52::Ch2On));
    } else {
        channelEnabled = enabled;
        memory.setIoRegister(Mmu::NR52, resetBit(memory.getIoRegister(Mmu::NR52), Apu::NR52::Ch2On));
    }
}


void Channel2::setDacEnabled() {
    DACenabled = (NRx2 & 0xF8) != 0; // if any of the bits 3-7 are on, dac enabled
}


void Channel2::setPeriod() {
    // takes top 3 bits from NRx4 and makes an 11 bit period with NRx3
    period = ((NRx4 & 0x7) << 8) | NRx3;
}


#pragma endregion

