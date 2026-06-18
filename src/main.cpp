#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/file.hpp"


int main() {

    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/02-interrupts.gb";
    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/03-op sp,hl.gb";
    std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/small_rom.gb";
    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/cpu_instrs.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());
    

    if (gameboy.checksumPassed) {
        gameboy.run();
    }
    
    return 0;
}
