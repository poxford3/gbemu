#include <iostream>
#include <nfd.h>
#include "window.hpp"
#include "gameboy.hpp"
#include "types.hpp"

EmuWindow::EmuWindow() {}

void EmuWindow::run() {
    int gbHeight = 160;
    int gbWidth = 144;
    int scale = 3;
    unsigned windowWidth = gbWidth * scale;
    unsigned windowHeight = gbHeight * scale;
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "GB Emulator",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    std::cout << "Running window loop..." << std::endl;

    // button
    sf::RectangleShape button(sf::Vector2f(200, 200));
    button.setFillColor(sf::Color::Cyan);
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(2);

    float x = (window.getSize().x - button.getSize().x) / 2.f;
    float y = (window.getSize().y - button.getSize().y) / 2.f;
    button.setPosition({x, y});


    // text
    sf::Font font("/Users/poxford3/Documents/coding/cpp/gbemu/assets/arial/ARIAL.TTF");
    sf::Text text(font, "open file");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition({x + 50, y + 80});
    
    // to show fps, uncomment
    // sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
            std::cout << "pushin p" << std::endl;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            // todo up
        }

        // Check for mouse button press
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            // Check if the mouse click is within the button's bounds
            if (button.getGlobalBounds().contains({(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y})) {
                std::cout << "Button clicked!" << std::endl;
                FileHandler rom = getFileFromUser();
                std::vector<Byte> buffer = rom.readFile();
                if (buffer.size() > 0) {
                    Gameboy gameboy(buffer);
                    text.setString("file loaded!");
                } else {
                    text.setString("file canceled/error");
                }
            }
        }


        // show fps
        // std::cout << 1000000.0f / clock.getElapsedTime().asMicroseconds() << '\n';
        // clock.restart();

        window.clear();
        window.draw(button);
        window.draw(text);
        window.display();
    }
}

void getInputs() {

}


FileHandler EmuWindow::getFileFromUser() {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(&outPath, NULL, NULL, NULL);

    if (result == NFD_OKAY) {
        // std::string filePath(outPath);
        std::cout << "successfully got file path: " << outPath << std::endl;
        puts(outPath);
        std::string str_filePath(outPath);
        free(outPath);
        return FileHandler(str_filePath);

    } else if (result == NFD_CANCEL) {
        std::cout << "User canceled the dialog." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
    // return FileHandler(filePath);
    return FileHandler("");
}