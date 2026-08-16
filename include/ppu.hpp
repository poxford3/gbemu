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

        int winScale = 2; // used to change the size of the gameboy screen
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
        void updateGraphics(Mmu &memory, uint cycles);
        void loadTileData(Mmu &memory);
    private:
        static const Word oamStart = 0xFE00;
        static const Byte oamSize = 0x9F;

        enum PpuMode {
            HBLANK = 0,
            VBLANK = 1,
            OAM = 2,
            DRAWING = 3
        };

        enum LCDStatus { // bits 3-6 used for interrupts
            LYC_INT = 6,
            MODE2_INT = 5,
            MODE1_INT = 4,
            MODE0_INT = 3,
            LYC_FLAG = 2,
            PPU_MODE_H = 1, // high bit of ppu mode
            PPU_MODE_L = 0  // low bit of ppu mode
        };

        enum LCDControl {
            LCD_PPU_ENABLE = 7,         // 0 = Off; 1 = On
            WIN_TILE_MAP_SELECT = 6,    // 0 = 9800–9BFF; 1 = 9C00–9FFF
            WIN_ENABLE = 5,             // 0 = Off; 1 = On
            BG_WIN_TILE_DATA_SELECT = 4,// 0 = 8800–97FF; 1 = 8000–8FFF
            BG_TILE_MAP_SELECT = 3,     // 0 = 9800–9BFF; 1 = 9C00–9FFF
            OBJ_SIZE = 2,               // 0 = 8x8; 1 = 8x16
            OBJ_ENABLE = 1,             // 0 = Off; 1 = On
            BG_WIN_ENABLE = 0           // 0 = Off; 1 = On (different for CGB)
        };

        void loadOamToFrameBuffer(Mmu &memory, Byte currentLine, Byte lcdc); // load sprites in, can potentially rename
        void loadWinToFrameBuffer(Mmu &memory, Byte currentLine, Byte lcdc);
        void loadBgToFrameBuffer(Mmu &memory, Byte currentLine, Byte lcdc);
        void LCDStatus(Mmu &memory);
};

#endif