#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "gameboy.hpp"

#ifndef FILE_HPP
#define FILE_HPP

/**
 * @brief Handles file operations for reading binary files.
 * @param filePath Path to the binary file/ROM.
 */
class FileHandler {
    public:
        bool isDmg;
        bool isCgb;
        FileHandler();
        FileHandler(const std::string &filePath);
        std::vector<unsigned char> readFile();
        void readRandomValues(const std::vector<unsigned char>& buffer, int start, int length);
        void readNthByte(const std::vector<unsigned char>& buffer, int n);

        void createSaveFile(Gameboy &gameboy);
        void loadSaveFile(Gameboy &gameboy);
        
    private:
        std::string m_filePath;
        std::string m_folderPath;
        std::string m_fileName;
};

#endif // FILE_HPP