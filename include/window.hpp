#include <SFML/Graphics.hpp>

#ifndef WINDOW_HPP
#define WINDOW_HPP

class EmuWindow {
    public:
        EmuWindow();
        void run();
    private:
        sf::Window window;
};

#endif // WINDOW_HPP