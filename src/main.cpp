#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "emulator.hpp"
#include "gameboy.hpp"
#include "utils/file.hpp"
#include "utils/bit.hpp"


int main() {

    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/cpu_instrs.gb";
    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/small_rom.gb";
    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/lin_rom.gb";
    // FileHandler fileH(path);
    // Gameboy gameboy (fileH.readFile());

    // gameboy.run();

    Emulator emulator;
    emulator.run();

    // std::string paths[8] = {
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/01-special.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/02-interrupts.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/03-op sp,hl.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/04-op r,imm.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/05-op rp.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/06-ld r,r.gb",
    //     "/Users/poxford3/Downloads/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb",
    // };

    // for (int i = 0; i < 8; i++) {
    //     Gameboy gameboy (FileHandler(paths[i]).readFile());
    //     printf("starting %s\n", paths[i].c_str());
    //     gameboy.run();
    //     SDL_Delay(5000);
    //     gameboy.stop();
    //     printf("stopping %s\n", paths[i].c_str());
    // }

    return 0;
}
