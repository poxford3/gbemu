#include "ppu.hpp"

Ppu::Ppu() {}

void Ppu::init() {
    uint windowHeight = GAMEBOY_HEIGHT * winScale;
    uint windowWidth = GAMEBOY_WIDTH * winScale;

    window.create(
        sf::VideoMode({windowWidth, windowHeight}),
        "GB Emulator",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    std::cout << "Running window loop..." << std::endl;
}

void Ppu::run() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }

    window.clear();
    window.display();
}

void Ppu::displayMemory(Mem &memory) {

}