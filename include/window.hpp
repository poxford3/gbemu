#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "file.hpp"

#ifndef WINDOW_HPP
#define WINDOW_HPP

class EmuWindow {
    public:
        EmuWindow();
        void run();
        FileHandler getFileFromUser();
    private:
        sf::Window window;
};

#endif // WINDOW_HPP