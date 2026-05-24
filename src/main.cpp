#include <iostream>
#include <fstream>
#include <vector>
#include "ppu.hpp"
#include "window.hpp"
#include "gameboy.hpp"
#include "file.hpp"


int main() {

    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/01-special.gb";
    std::string path = "/Users/poxford3/Downloads/cpu_instrs/cpu_instrs.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());
    
    Ppu ppu;
    ppu.init();
    
    if (gameboy.checksumPassed) {
        while (ppu.window.isOpen()) {
            gameboy.tick();
            ppu.run();
        }
    }


    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}
