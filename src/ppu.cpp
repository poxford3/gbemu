#include <nfd.h>
#include "gameboy.hpp"
#include "ppu.hpp"

Ppu::Ppu() {
    window = nullptr;
    renderer = nullptr;
    font = nullptr;
}

void Ppu::init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be intiialized, SDL error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("gbemu",
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    (EMULATOR_SCREEN_WIDTH + MEMORY_SECTION_WIDTH), // allow extra room to render parts of the CPU
                    EMULATOR_SCREEN_HEIGHT,
                    SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("error initializing window. SDL error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("error initializing renderer. SDL error: %s\n", SDL_GetError());
    }

    gbTexture = SDL_CreateTexture(
                        renderer,
                        SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        EMULATOR_SCREEN_WIDTH,
                        EMULATOR_SCREEN_HEIGHT
                    );

    if (gbTexture == NULL) {
        printf("error initializing texture, SDL error %s\n", SDL_GetError());
    }

    if (TTF_Init()) {
        printf("error initializing font, SDL error: %s\n", TTF_GetError());
    }

    font = TTF_OpenFont("/Users/poxford3/Documents/coding/cpp/gbemu/assets/arial/ARIAL.TTF", 24);
    if (font == NULL) {
        printf("error loading font: %s\n", TTF_GetError());
    }

    palette = BlackWhite;
    // palette = GameboyGreen;
 
    running = true;
}

SDL_Renderer* Ppu::GetRenderer() {
    return renderer;
}

void Ppu::close() {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    SDL_Quit();
}

void Ppu::run() {}

void Ppu::drawText(const std::string& text, int x, int y) {
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {x, y, w, h};
    
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Ppu::drawFrame(Cpu cpu, Mmu memory) {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
            close();
        }
    }

    // functioning as the background at the moment
    SDL_Color c = palette.getColor(DARK_GRAY);

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer);

    char buf[80];
    uint lineHeight = 30;
    uint x = EMULATOR_SCREEN_WIDTH + 5; // +5 for a bit of left padding
    snprintf(buf, sizeof(buf), "A: 0x%02X", cpu.A); drawText(buf, x, lineHeight * 0);
    snprintf(buf, sizeof(buf), "F: 0x%02X", cpu.F); drawText(buf, x, lineHeight * 1);
    snprintf(buf, sizeof(buf), "B: 0x%02X", cpu.B); drawText(buf, x, lineHeight * 2);
    snprintf(buf, sizeof(buf), "C: 0x%02X", cpu.C); drawText(buf, x, lineHeight * 3);
    snprintf(buf, sizeof(buf), "D: 0x%02X", cpu.D); drawText(buf, x, lineHeight * 4);
    snprintf(buf, sizeof(buf), "E: 0x%02X", cpu.E); drawText(buf, x, lineHeight * 5);
    snprintf(buf, sizeof(buf), "H: 0x%02X", cpu.H); drawText(buf, x, lineHeight * 6);
    snprintf(buf, sizeof(buf), "L: 0x%02X", cpu.L); drawText(buf, x, lineHeight * 7);
    snprintf(buf, sizeof(buf), "PC: 0x%02X", cpu.PC); drawText(buf, x, lineHeight * 8);
    snprintf(buf, sizeof(buf), "SP: 0x%02X", cpu.SP); drawText(buf, x, lineHeight * 9);
    SDL_Rect separator = {EMULATOR_SCREEN_WIDTH, 0, 1, EMULATOR_SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderFillRect(renderer, &separator);

    // tile data location: $8000-$97FF
    // Byte tileMapStart = (((memory[cpu.LCDC] >> 3) & 1) == 1) ? 0x9c00 : 0x9800;
    // Byte tileDataStart = (((memory[cpu.LCDC] >> 4) & 1) == 1) ? 0x8000 : 0x8800;
    // // for (Word i = 0; i < 0x17FF; i++) {
    // //     // frameBuffer[i] = memory[0x8000 + i];
    // //     // TODO set it up so you get the hi and lo bits of the memory to render to the screen
    // //     for (Byte j = 0; j < 0; j++) {
    // //         frameBuffer[j] = memory[0x8000 + 1] >> j;
    // //     }
    // // }
    // for (Word i = 0; i < 0x17FF; i++) {
    //     Byte hi = memory[tileDataStart + i];
    //     Byte lo = memory[tileDataStart + i + 1];

    // }

    // SDL_UpdateTexture(gbTexture, NULL, frameBuffer, EMULATOR_SCREEN_WIDTH);
    // SDL_RenderCopy(renderer, gbTexture, NULL, NULL);


    // present the frame
    SDL_RenderPresent(renderer);

    if (1==2) {
        FileHandler fileH = getFileFromUser();
        std::vector<Byte> rom = fileH.readFile();

        if (rom.size() > 1) {
            Gameboy gameboy(rom);
        }
    }

    // if (SDL_PollEvent(&event) == SDL_MouseButtonEvent(&event)) {
    //     // todo add in clicking to open NFD
    // }
}

void Ppu::displayMemory(Mmu &memory) {

}

FileHandler Ppu::getFileFromUser() {
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