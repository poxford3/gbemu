#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdint.h>
#include <cstdint>
#include "cpu/cpu.hpp"
#include "utils/palette.hpp"
#include "utils/file.hpp"
#include "utils/types.hpp"

#ifndef PPU_HPP
#define PPU_HPP

class Ppu {
    public:
        static const uint winScale = 3;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* gbTexture;
        TTF_Font* font;
        Palette palette;
        
        bool running;
        
        Ppu();
        void init();
        void run();
        void close();
        SDL_Renderer* GetRenderer();
        void displayMemory(Mmu &memory);
        void drawText(const std::string& text, int x, int y);
        void drawFrame(Cpu cpu, Mmu memory);
        FileHandler getFileFromUser();
    private:
        static const uint GAMEBOY_HEIGHT = 144;
        static const uint GAMEBOY_WIDTH = 160;
        static const uint MEMORY_SECTION_WIDTH = 400;
        static const uint EMULATOR_SCREEN_WIDTH = GAMEBOY_WIDTH * winScale;
        static const uint EMULATOR_SCREEN_HEIGHT = GAMEBOY_HEIGHT * winScale;

        // 3 bytes per pixel
        Byte frameBuffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 3];
        Byte tileMap[GAMEBOY_HEIGHT * GAMEBOY_WIDTH];
};

#endif