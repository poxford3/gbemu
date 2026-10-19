#include "apu/apu.hpp"

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


void Apu::updateSound(Mmu &memory, uint &cycles) {
    Byte NR52 = memory.readByte(Mmu::NR52); // audio master control
    Byte NR51 = memory.readByte(Mmu::NR51); // Sound Panning
    Byte NR50 = memory.readByte(Mmu::NR50); // Master Volume and VIN panning

    bool audioOn = getBit(NR52, 7);
    bool channel1On = getBit(NR52, 0);
    bool channel2On = getBit(NR52, 1);
    bool channel3On = getBit(NR52, 2);
    bool channel4On = getBit(NR52, 3);

    if (audioOn) {

        Byte div = memory.getIoRegister(Mmu::DIV);

        frameSequencer(div);
        // if (channel1On) channel1.update(memory, cycles, currentVolume);
        if (channel1On) channel1.tick();
        // if (channel2On) channel2.update(memory, cycles, currentVolume);

        mixer();
    }
}


float Apu::mixer() {
    return (channel1.amplitude) / 1.f;
}