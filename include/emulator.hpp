#include <nfd.h>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
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
        bool showDebugMenu;
        bool showTileData;
        int emulatorScreenWidth = 300;
        int emulatorScreenHeight = 150;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* gbTexture;
        SDL_Texture* tileDataTexture;
        TTF_Font* font;
        SDL_Renderer* GetRenderer();
        std::optional<Gameboy> gameboy;

        void handleInput(SDL_Event &event);
        void createGameboyTextures();
        void renderMenuBar();

};

#endif