#include <iostream>
#include <fstream>
#include <vector>
#include "window.hpp"
#include "gameboy.hpp"
#include "file.hpp"


int main() {

    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/01-special.gb";
    std::string path = "/Users/poxford3/Downloads/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";
    FileHandler fileH(path);
    Gameboy gameboy (fileH.readFile());
    
    if (gameboy.checksumPassed) {
        while (1) {
            gameboy.tick();
        }
    }

    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}
