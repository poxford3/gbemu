#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdint.h>
#include <cstdint>
#include "cpu.hpp"
#include "palette.hpp"
#include "file.hpp"
#include "types.hpp"

#ifndef PPU_HPP
#define PPU_HPP

class Ppu {
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        TTF_Font* font;
        Palette palette;
        
        SDL_Renderer* GetRenderer();
        void close();
        bool running;
        
        Ppu();
        void init();
        void run();
        FileHandler getFileFromUser();
        void displayMemory(Mem &memory);
        void drawText(const std::string& text, int x, int y);
        void drawFrame(Cpu cpu);
    private:
        static const uint GAMEBOY_HEIGHT = 144;
        static const uint GAMEBOY_WIDTH = 160;
        uint winScale = 3;
        // 3 bytes per pixel
        Byte frameBuffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 3];
};

#endif