#include "apu/apu.hpp"

Apu::~Apu() {}


void Apu::reset() {
    audioCycles = 0;
    channel2.reset();
}


void Apu::tickLength(Mmu &memory) {
    // channel1.tickLength(memory);
    channel2.tickLength(memory);
    // channel3.tickLength(memory);
    // channel4.tickLength(memory);
}


void Apu::tickEnvelopeVolume(Mmu &memory) {
    // channel1.tickLength(memory);
    channel2.tickEnvelopeVolume(memory);
    // channel3.tickLength(memory);
    // channel4.tickLength(memory);
}


void Apu::tickSweep(Mmu &memory) {
    // channel1.tickSweep();
}


void Apu::tickTrameSequencer(Mmu &memory) {
    Byte div = memory.getIoRegister(Mmu::DIV);
    bool div_counter = isCgb2xSpeed ? getBit(div, 6) : getBit(div, 5);

    if (div_counter) {
        if (frameSeqCounter % 2 == 0) tickLength(memory);
        if (frameSeqCounter % 4 == 0) tickSweep(memory);
        if (frameSeqCounter % 8 == 0) tickEnvelopeVolume(memory);
        frameSeqCounter++;
        frameSeqCounter &= 0x7;
    }
}


void Apu::updateSound(Mmu &memory, uint &cycles) {
    Byte NR52 = memory.getIoRegister(Mmu::NR52); // Audio Master Control
    Byte NR51 = memory.getIoRegister(Mmu::NR51); // Sound Panning
    Byte NR50 = memory.getIoRegister(Mmu::NR50); // Master Volume and VIN panning

    bool audioOn = getBit(NR52, AudioOn);
    // potentially remove below
    bool channel1On = getBit(NR52, Ch1On);
    bool channel2On = getBit(NR52, Ch2On);
    bool channel3On = getBit(NR52, Ch3On);
    bool channel4On = getBit(NR52, Ch4On);

    if (audioOn) {
        audioCycles += cycles;
        if (audioCycles % 2048 == 0) {
            tickTrameSequencer(memory); // every 8192 T-cycles (2048 M-cycles) the FS is ticked
        }
        // if (channel1On) channel1.tick(currentVolume);
        channel2.tick(memory);
        // if (channel3On) channel3.tick(currentVolume);
        // if (channel4On) channel4.tick(currentVolume);
        mixer();
    }
}


float Apu::mixer() {
    // return (channel1.amplitude +
    //         channel2.amplitude +
    //         channel3.amplitude +
    //         channel4.amplitude) / 4.f;
    printf("ch2: %f\n", channel2.amplitude);
    return channel2.amplitude;
    // return 0.f;
}