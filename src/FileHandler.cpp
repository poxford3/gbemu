#include <iostream>
#include "file.hpp"

FileHandler::FileHandler(const std::string& filePath) : m_filePath{filePath} {}

std::vector<unsigned char> FileHandler::readFile() {
    std::vector<unsigned char> buffer;
    std::ifstream file(m_filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return buffer;
    }

    char byte;
    while (file.read(reinterpret_cast<char*>(&byte), 1)) {
        buffer.push_back(byte);
    }
    file.close();
    return buffer;
}

void FileHandler::readRandomValues(const std::vector<unsigned char>& buffer, int start, int length) {
    int final_pos = start + length;
    for (int i = start; i < final_pos && i < buffer.size(); ++i) {
        std::cout << std::hex << (int)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal
}

void FileHandler::readNthByte(const std::vector<unsigned char>& buffer, int n) {
    // gameboy rom game starts at $0100, an example to read from
    if (n < buffer.size()) {
        std::cout << "Byte " << n << ": " << std::hex << (int)buffer[n] << std::dec << std::endl;
    } else {
        std::cerr << "Index out of bounds!" << std::endl;
    }
}