#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdint.h>
#include <cstdint>
#include "cpu.hpp"
#include "types.hpp"

#ifndef PPU_HPP
#define PPU_HPP

struct Ppu {

    static const uint GAMEBOY_HEIGHT = 144;
    static const uint GAMEBOY_WIDTH = 160;
    uint winScale = 3;

    // 3 bytes per pixel
    Byte frameBuffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH * 3];

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;


    Ppu();
    void init();
    void run();
    void drawFrame( Mem& memory);
    void displayMemory(Mem &memory);
};

#endif