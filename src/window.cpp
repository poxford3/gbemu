#include <iostream>
#include "window.hpp"

EmuWindow::EmuWindow() {
    window.create(sf::VideoMode({800,600}), "GB Emulator");
}

void EmuWindow::run() {
    std::cout << "Running window loop..." << std::endl;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }
}   