#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdint.h>
#include <cstdint>
#include "cpu/cpu.hpp"
#include "utils/palette.hpp"
#include "utils/types.hpp"

#ifndef PPU_HPP
#define PPU_HPP

class Ppu {
    public:
        Palette palette;
        
        // bool running;
        bool paused = false; // used to pause the emulator when debugging
        int scanlineCounter;
        
        Ppu();
        ~Ppu();
        void reset();

        uint winScale = 2; // used to change the size of the gameboy screen
        uint EMULATOR_SCREEN_WIDTH() const { return GAMEBOY_WIDTH * winScale; };
        uint EMULATOR_SCREEN_HEIGHT() const { return GAMEBOY_HEIGHT * winScale; };
        uint EMULATOR_TILEDATA_WIDTH() const { return TILEDATA_WIDTH * winScale; };
        uint EMULATOR_TILEDATA_HEIGHT() const { return TILEDATA_HEIGHT * winScale; };
        bool DEBUG = true; // used to change between view with(out) registers and memory
        static const uint GAMEBOY_HEIGHT = 144;
        static const uint GAMEBOY_WIDTH = 160;
        static const uint MEMORY_SECTION_WIDTH = 400;
        static const uint TILEDATA_HEIGHT = 192;
        static const uint TILEDATA_WIDTH = 128;

        
        // 3 bytes per pixel for background
        static const uint bgFrameBufferSize = GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 3;
        std::array<Byte, bgFrameBufferSize> frameBuffer;
        // 4 bytes per pixel for sprites (transparency being the 4th)
        std::array<Byte, GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 4> frameBufferObj;
        std::array<Byte, TILEDATA_HEIGHT * TILEDATA_WIDTH * 3> tileData;
        void updateGraphics(Cpu &cpu, Mmu &memory, uint cycles);
        void loadTileData(Mmu &memory);
    private:
        void loadScanline(Mmu &memory, Byte currentLine);
        void LCDStatus(Mmu &memory);
};

#endif