#include <iostream>
#include <vector>
#include <fstream>

#ifndef FILE_HPP
#define FILE_HPP

/**
 * @brief Handles file operations for reading binary files.
 * @param filePath Path to the binary file/ROM.
 */
class FileHandler {
public:
    FileHandler(const std::string &filePath);
    std::vector<unsigned char> readFile();
    void readRandomValues(const std::vector<unsigned char>& buffer, int start, int length);
    void readNthByte(const std::vector<unsigned char>& buffer, int n);

private:
    std::string m_filePath;
};

#endif // FILE_HPP