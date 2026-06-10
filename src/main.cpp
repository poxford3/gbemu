#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "ppu.hpp"
#include "gameboy.hpp"
#include "file.hpp"


int main() {

    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/01-special.gb";
    std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/small_rom.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());
    

    // PPU not ready, need other things to be working first
    // Ppu ppu;
    // ppu.init();

    // if (gameboy.checksumPassed) {
    //     while (ppu.running) {
    //             uint frameCycles = 0;
    //             while (frameCycles < 70224) { // 70224 = 154 * 456 (154 scanlines, 456 "dots"/cycles per scanline)
    //                 uint cycles = gameboy.tick();
    //                 frameCycles += cycles;
    //             }
    //             ppu.drawFrame(gameboy.cpu, gameboy.memory);
    //             SDL_Delay(16); // 16 ms = 60 fps (1/60)
    //         }
    // }

    // ppu.close();
    
    return 0;
}
