#include <iostream>
#include <fstream>
#include <vector>
#include "window.hpp"
#include "file.hpp"


int main() {
    
    // TODO: implement file uploading and relative paths
    // using absolute path for testing, need to implement file uploading and relative paths
    FileHandler fileHandler("/Users/poxford3/Documents/coding/gbemu/assets/pokemon_blue.gb");
    std::vector<unsigned char> buffer = fileHandler.readFile();
    
    std::cout << "File size: " << buffer.size() << " bytes" << std::endl;
    fileHandler.readRandomValues(buffer, 0, 32); // Read first 32 bytes
    fileHandler.readNthByte(buffer, 0x0100); // Read byte at index $0100
    
    EmuWindow emuWindow;
    emuWindow.run();


    return 0;
}