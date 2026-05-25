#include <iostream>
#include <fstream>
#include <vector>
#include "ppu.hpp"
// #include "window.hpp"
#include "gameboy.hpp"
#include "file.hpp"


int main() {

    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/cpu_instrs.gb";
    std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/01-special.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());
    
    Ppu ppu;
    ppu.init();
    
    // if (gameboy.checksumPassed) {
    //         uint frameCycles = 0;
    //         while (frameCycles < 70224) {
    //             uint cycles = gameboy.tick();
    //             // ppu.step(cycles);
    //             frameCycles += cycles;
    //         }
    // }


    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}
