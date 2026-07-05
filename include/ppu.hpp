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
        uint winScale = 3; // used to change the size of the gameboy screen
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* gbTexture;
        TTF_Font* font;
        Palette palette;
        
        bool running;
        int scanlineCounter;
        
        Ppu();
        ~Ppu();
        void init();
        SDL_Renderer* GetRenderer();
        void resize();
        // module rendering
        void displayMemory(Cpu &cpu, Mmu &memory);
        void drawText(const std::string& text, int x, int y);
        // gameboy rendering
        void loadScanline(Mmu &memory, Byte currentLine);
        void LCDStatus(Mmu &memory);
        void updateGraphics(Cpu &cpu, Mmu &memory, uint cycles);
        void drawFrame(Cpu &cpu, Mmu &memory);
        // files
        FileHandler getFileFromUser();
        // inputs
        void handleInput();
    private:
        bool DEBUG = true; // used to change between view with(out) registers and memory
        static const uint GAMEBOY_HEIGHT = 144;
        static const uint GAMEBOY_WIDTH = 160;
        static const uint MEMORY_SECTION_WIDTH = 400;
        uint EMULATOR_SCREEN_WIDTH() const { return GAMEBOY_WIDTH * winScale; };
        uint EMULATOR_SCREEN_HEIGHT() const { return GAMEBOY_HEIGHT * winScale; };

        // 3 bytes per pixel for background
        Byte frameBuffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 3];
        // 4 bytes per pixel for sprites (transparency being the 4th)
        Byte frameBufferObj[GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 4];
        Byte tileMap[GAMEBOY_HEIGHT * GAMEBOY_WIDTH];
};

#endif