#include <iostream>
#include <fstream>
#include <vector>
#include "window.hpp"
#include "file.hpp"
#include "cpu.hpp"
#include "opcodes.hpp"
#include "opcycles.hpp"


int main() {

    // Mem memory;
    // Cpu cpu;
    // cpu.reset(memory);

    EmuWindow emuWindow;
    emuWindow.run();


    return 0;
}