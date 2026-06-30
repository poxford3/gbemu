#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/file.hpp"
#include "utils/bit.hpp"


int main() {

    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/02-interrupts.gb";
    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/03-op sp,hl.gb";
    // std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";
    std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/cpu_instrs.gb";
    
    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/small_rom.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());

    if (gameboy.checksumPassed) {
        gameboy.run();
    }

    // Byte test = 0xF2;
    // for (int i = 0; i <= 7; i++) {
    //     printf("bit %d: %d\n", i, getBit(test, i));
    // }

    // printf("size of int: %lu", sizeof(int));
    
    return 0;
}
