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
                    GAMEBOY_WIDTH * winScale, 
                    GAMEBOY_HEIGHT * winScale,
                    SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("error initializing window. SDL error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("error initializing renderer. SDL error: %s\n", SDL_GetError());
    }

    if (TTF_Init()) {
        printf("error initializing font, SDL error: %s\n", TTF_GetError());
    }

    font = TTF_OpenFont("/Users/poxford3/Documents/coding/cpp/gbemu/assets/arial/ARIAL.TTF", 36);
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
    SDL_Color textColor = {0, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {x, y, w, h};
    
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Ppu::drawFrame(Cpu cpu) {
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

    char buf[32];
    snprintf(buf, sizeof(buf), "A: 0x%02X", cpu.A); drawText(buf, 0, 0);
    snprintf(buf, sizeof(buf), "F: 0x%02X", cpu.F); drawText(buf, 0, 50);
    snprintf(buf, sizeof(buf), "B: 0x%02X", cpu.B); drawText(buf, 0, 100);
    snprintf(buf, sizeof(buf), "C: 0x%02X", cpu.C); drawText(buf, 0, 150);
    snprintf(buf, sizeof(buf), "OBP0: 0x%02X", cpu.OBP0); drawText(buf, 0, 200);


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

void Ppu::displayMemory(Mem &memory) {

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