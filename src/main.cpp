#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/file.hpp"
#include "utils/bit.hpp"


int main() {

    std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/cpu_instrs.gb";
        // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/small_rom.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());

    if (gameboy.checksumPassed) {
        gameboy.run();
    }

    
    return 0;
}
