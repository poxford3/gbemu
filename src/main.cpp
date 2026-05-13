#include <iostream>
#include <fstream>
#include <vector>
#include "window.hpp"
#include "gameboy.hpp"
#include "file.hpp"


#include <string>
#include <filesystem>

namespace fs = std::filesystem;
void testAllOpcodesEver(Gameboy gb) {

    std::string dir = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/v1/";

    std::vector<std::string> dirs;

    // get all files in the v1 dir
    for (const auto& entry : fs::directory_iterator(dir)) {
        std::string full_path = dir + entry.path().filename().string();
        // std::cout << full_path << std::endl;
        // std::cout << entry.path().filename() << std::endl;
        if (entry.path().filename().string().find("cb ") != 0) {
            dirs.push_back(full_path);
        }
    }
    // sort the files to be in opcode order(ish)
    std::sort(dirs.begin(), dirs.end());

    // for (int i = 0; i < dirs.size(); i++) {
    //     std::cout << dirs[i] << std::endl;
    // }

    // run the test for all opcodes
    for (int i = 0; i < dirs.size(); i++) {
        gb.testWithJson(dirs[i]);
    }
}

int main() {

    // std::string path = "/Users/poxford3/Documents/coding/cpp/gbemu/assets/01-special.gb";
    // FileHandler fileH(path);
    // Gameboy gameboy (fileH.readFile());
    
    // if (gameboy.checksumPassed) {
    //     while (1) {
    //         gameboy.tick();
    //     }
    // }
            
    Gameboy gameboy;
    gameboy.testWithJson("/Users/poxford3/Documents/coding/cpp/gbemu/assets/v1/F1.json");
    // testAllOpcodesEver(gameboy);

    // EmuWindow emuWindow;
    // emuWindow.run();


    return 0;
}
