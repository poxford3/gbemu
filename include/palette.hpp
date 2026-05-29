#include <SDL2/SDL.h>
#include "types.hpp"


#ifndef PALETTE_HPP
#define PALETTE_HPP


enum GameboyColors {
    BLACK = 0x11,
    DARK_GRAY = 0x10,
    LIGHT_GRAY = 0x01,
    WHITE = 0x00
};

enum PaletteOptions {
    BlackWhite = 0,
    GameboyGreen = 1
};

class Palette {
    public:
        PaletteOptions selectedPalette;
        

        SDL_Color BlackWhiteColors[4] = {
            {0, 0, 0, 255},         // black
            {85, 85, 85, 255},      // dark gray (85 = 1/3 * 255)
            {170, 170, 170, 255},   // light gray (170 = 2/3 * 255)
            {255, 255, 255, 255},   // white
        };

        SDL_Color GameboyGreenColors[4] = {
            {15, 56, 15, 255},      // black
            {48, 98, 48, 255},      // dark gray
            {139, 172, 15, 255},    // light gray
            {155, 188, 15, 255}     // white
        };

        Palette(PaletteOptions palette = BlackWhite) : selectedPalette(palette) {}

        SDL_Color getColor(GameboyColors color) {
            SDL_Color* palette = (selectedPalette == BlackWhite) 
                ? BlackWhiteColors 
                : GameboyGreenColors;

            switch (color) {
                case BLACK:      return palette[0];
                case DARK_GRAY:  return palette[1];
                case LIGHT_GRAY: return palette[2];
                case WHITE:      return palette[3];
                default:         return palette[0];
            }
        }

};

#endif