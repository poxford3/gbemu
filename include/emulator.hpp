#include <nfd.h>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "gameboy.hpp"
#include "utils/file.hpp"
#include "utils/palette.hpp"

#ifndef EMULATOR_HPP
#define EMULATOR_HPP

class Emulator {
    public:
        Emulator();
        ~Emulator();
        void init();
        void run();
        const char* emulatorTitle = "gbemu";
        FileHandler getFileFromUser();
    private:
        bool running;
        bool paused;
        int emulatorScreenWidth = 800;
        int emulatorScreenHeight = 600;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* gbTexture;
        SDL_Texture* tileDataTexture;
        TTF_Font* font;
        SDL_Renderer* GetRenderer();
        std::optional<Gameboy> gameboy;

        void handleInput();
        void createGameboyTextures();
        void displayMemory(Cpu &cpu, Mmu &memory, uint width);
        void drawText(const std::string& text, int x, int y);

};

#endif