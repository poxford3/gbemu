#include <iostream>
#include <fstream>
#include <vector>


int main() {
    // std::cout << "Hello, World!" << std::endl;

    std::ifstream file("../assets/pokemon_blue.gb", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::vector<unsigned char> buffer;
    char byte;

    while (file.read(reinterpret_cast<char*>(&byte), 1)) {
        buffer.push_back(byte);
    }

    std::cout << "File size: " << buffer.size() << " bytes" << std::endl;


    // unsigned char byteList[32];
    int i = 0;
    while (file.get(byte)) {
        // Process each raw byte
        std::cout << std::hex << (int)(unsigned char)byte << " ";
        buffer[i] = (unsigned char)byte;
        i++;
    }

    unsigned char test = buffer[0];

    printf("First byte: %02x\n", test);

    int incr = 100;
    int length = 32;
    int final_pos = incr + length;
    for (int i = incr; i < final_pos; ++i) {
        std::cout << std::hex << (int)buffer[i] << " ";
    }

    file.close();

    return 0;
}