#include <iostream>
#include "utils/file.hpp"
#include "utils/types.hpp"

FileHandler::FileHandler() {};


FileHandler::FileHandler(const std::string& filePath) : m_filePath{filePath} {
    // Convert string to std::filesystem::path
    std::filesystem::path p(filePath);
    
    // Get the parent directory path
    std::filesystem::path folderPathObj = p.parent_path();
    m_folderPath = folderPathObj.string();
    m_fileName = p.filename().stem().string();

    if (filePath.length() > 0) {
        if (filePath.substr(filePath.length() - 3) == ".gb") {
            isDmg = true;
        } else if (filePath.substr(filePath.length() - 4) == ".gbc") {
            isCgb = true;
        }
    }
}

/**
 * readFile()
 * @return `std::vector<Byte>` array of data from binary file
 */
std::vector<Byte> FileHandler::readFile() {
    std::vector<Byte> buffer;
    std::ifstream file(m_filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return buffer;
    }

    Byte byte;
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        buffer.push_back(byte);
    }
    file.close();
    return buffer;
}


void FileHandler::readRandomValues(const std::vector<Byte>& buffer, int start, int length) {
    int final_pos = start + length;
    for (int i = start; i < final_pos && i < buffer.size(); ++i) {
        std::cout << std::hex << (int)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal
}


void FileHandler::readNthByte(const std::vector<Byte>& buffer, int n) {
    // gameboy rom game starts at $0100, an example to read from
    if (n < buffer.size()) {
        std::cout << "Byte " << n << ": " << std::hex << (int)buffer[n] << std::dec << std::endl;
    } else {
        std::cerr << "Index out of bounds!" << std::endl;
    }
}


void FileHandler::createSaveFile(Gameboy &gameboy) {
    if (gameboy.mmu.cartridge.hasRAM) {
        std::ofstream saveFile(m_folderPath + "/" + m_fileName + ".sav", std::ios::binary);
        if (!saveFile) {
            std::cerr << "Error creating save file!" << std::endl;
            return;
        }
        saveFile.write(reinterpret_cast<const char*>(gameboy.mmu.externalRam.data()), sizeof(gameboy.mmu.externalRam)); // first 8k

        saveFile.close();
    }

}


void FileHandler::loadSaveFile(Gameboy &gameboy) {
     if (gameboy.mmu.cartridge.hasRAM) {
        std::ifstream saveFile(m_folderPath + "/" + m_fileName + ".sav", std::ios::binary | std::ios::ate);
        if (!saveFile) {
            std::cerr << "Error loading save file!" << std::endl;
            return;
        }

        // Get total size and seek back to the beginning to read
        std::streamsize fileSize = saveFile.tellg();
        saveFile.seekg(0, std::ios::beg);
        std::streamsize bytesToRead = std::min(fileSize, static_cast<std::streamsize>(sizeof(gameboy.mmu.externalRam)));

        saveFile.read(reinterpret_cast<char*>(gameboy.mmu.externalRam.data()), bytesToRead);
        saveFile.close();
    }
}