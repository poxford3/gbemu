#include "emulator.hpp"

Emulator::Emulator() {
    window = nullptr;
    renderer = nullptr;
    font = nullptr;
    gbTexture = nullptr;

    init();
}


Emulator::~Emulator() {
    TTF_CloseFont(font);    
    TTF_Quit();
    SDL_DestroyTexture(gbTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    gbTexture = NULL;
    window = NULL;
    renderer = NULL;

    SDL_Quit();
}


void Emulator::init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be intiialized, SDL error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(emulatorTitle,
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    emulatorScreenWidth,
                    emulatorScreenHeight,
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        printf("error initializing window. SDL error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        printf("error initializing renderer. SDL error: %s\n", SDL_GetError());
    }


    if (TTF_Init()) {
        printf("error initializing font, SDL error: %s\n", TTF_GetError());
    }

    font = TTF_OpenFont("/Users/poxford3/Documents/coding/cpp/gbemu/assets/arial/ARIAL.TTF", 24);
    if (font == NULL) {
        printf("error loading font: %s\n", TTF_GetError());
    }

    running = true;
}


SDL_Renderer* Emulator::GetRenderer() {
    return renderer;
}


void Emulator::createGameboyTextures() {
    if (gameboy.has_value()) {
        gbTexture = SDL_CreateTexture(
                            renderer,
                            SDL_PIXELFORMAT_RGB24,
                            SDL_TEXTUREACCESS_STREAMING,
                            gameboy->ppu.GAMEBOY_WIDTH,
                            gameboy->ppu.GAMEBOY_HEIGHT
                        );

        if (gbTexture == NULL) {
            printf("error initializing texture, SDL error %s\n", SDL_GetError());
        }

        tileDataTexture = SDL_CreateTexture(
                            renderer,
                            SDL_PIXELFORMAT_RGB24,
                            SDL_TEXTUREACCESS_STREAMING,
                            gameboy->ppu.TILEDATA_WIDTH,
                            gameboy->ppu.TILEDATA_HEIGHT
                        );

        if (tileDataTexture == NULL) {
            printf("error initializing texture, SDL error %s\n", SDL_GetError());
        }
    }
}

int frameCounter = 0;
void Emulator::run() {
    while (running) {
        SDL_Event event;
    
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_F) {
                            FileHandler file = getFileFromUser();
                            if (sizeof(file) > 0) { // todo, check what happens when a user hits cancel, and if file isn't the right type (extension)
                                printf("no file selected, please select a file to run the emulator\n");
                                gameboy.emplace(file.readFile());
                                createGameboyTextures();
                            }
                    } else if (event.key.keysym.scancode == SDL_SCANCODE_P) {
                        if (gameboy.has_value()) {
                            paused = !paused;
                        }
                    } else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                        if (gameboy.has_value()) {
                            if (gameboy->ppu.palette.selectedPalette == PaletteOptions::GameboyGreen) {
                                gameboy->ppu.palette.selectedPalette = PaletteOptions::BlackWhite;
                            } else {
                                gameboy->ppu.palette.selectedPalette = PaletteOptions::GameboyGreen;
                            }
                        }
                    }
                }
            }
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        if (gameboy.has_value()) {
            if (!paused) {
                gameboy->runFrame();
                gameboy->ppu.loadTileData(gameboy->mmu);
                frameCounter++;
            }

            // if (frameCounter == 60) {
            //     printf("VRAM Dump:\n");
            //     for (int i = 0; i < 0x1800; i++) {
            //         printf("%02X ", gameboy->mmu.VRam[i]);
            //         if ((i + 1) % 16 == 0) printf("\n");
            //     }
            // }


            uint widthForMemory = gameboy->ppu.EMULATOR_TILEDATA_WIDTH();
            uint heightFormemory = gameboy->ppu.EMULATOR_TILEDATA_HEIGHT();
            // uint widthForMemory = gameboy->ppu.EMULATOR_SCREEN_WIDTH();
            // uint heightFormemory = gameboy->ppu.EMULATOR_SCREEN_HEIGHT();

            if (true) { // TODO REPLACE WITH DEBUG SOMEWHERE
                displayMemory(gameboy->cpu, gameboy->mmu, widthForMemory);
                SDL_Rect separator = {static_cast<int>(widthForMemory), 0, 1, static_cast<int>(heightFormemory)};
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                SDL_RenderFillRect(renderer, &separator);
            }

            // SDL_Rect gameboySection = {0, 0, static_cast<int>(gameboy->ppu.EMULATOR_SCREEN_WIDTH()), static_cast<int>(gameboy->ppu.EMULATOR_SCREEN_HEIGHT())};
            // SDL_UpdateTexture(gbTexture, NULL, gameboy->ppu.frameBuffer.data(), gameboy->ppu.GAMEBOY_WIDTH * 3);
            // SDL_RenderCopy(renderer, gbTexture, NULL, &gameboySection);

            SDL_Rect tileDataSection = {0, 0, static_cast<int>(gameboy->ppu.EMULATOR_TILEDATA_WIDTH()), static_cast<int>(gameboy->ppu.EMULATOR_TILEDATA_HEIGHT())};
            SDL_UpdateTexture(tileDataTexture, NULL, gameboy->ppu.tileData.data(), gameboy->ppu.TILEDATA_WIDTH * 3);
            SDL_RenderCopy(renderer, tileDataTexture, NULL, &tileDataSection);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 16 ms = 60 fps
    }
}


void Emulator::handleInput() {
    const Byte* state = SDL_GetKeyboardState(NULL);
    // needs some work
}


void Emulator::drawText(const std::string& text, int x, int y) {
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


void Emulator::displayMemory(Cpu &cpu, Mmu &memory, uint width) {
    int numItems = 13;
    char buf[numItems * 9];
    uint lineHeight = 30;
    uint x = width + 5; // +5 for a bit of left padding
    Byte lcdcBin = memory.readByte(Mmu::LCDC);
    Byte statBin = memory.readByte(Mmu::STAT);
    Byte ly = memory.readByte(Mmu::LY);
    Byte tile1 = memory.readByte(0x8000);
    Byte tile2 = memory.readByte(0x8001);
    snprintf(buf, sizeof(buf), "A: 0x%02X", cpu.A); drawText(buf, x, lineHeight * 0);
    snprintf(buf, sizeof(buf), "F: 0x%02X", cpu.F); drawText(buf, x, lineHeight * 1);
    snprintf(buf, sizeof(buf), "B: 0x%02X", cpu.B); drawText(buf, x, lineHeight * 2);
    snprintf(buf, sizeof(buf), "C: 0x%02X", cpu.C); drawText(buf, x, lineHeight * 3);
    snprintf(buf, sizeof(buf), "D: 0x%02X", cpu.D); drawText(buf, x, lineHeight * 4);
    snprintf(buf, sizeof(buf), "E: 0x%02X", cpu.E); drawText(buf, x, lineHeight * 5);
    snprintf(buf, sizeof(buf), "H: 0x%02X", cpu.H); drawText(buf, x, lineHeight * 6);
    snprintf(buf, sizeof(buf), "L: 0x%02X", cpu.L); drawText(buf, x, lineHeight * 7);
    snprintf(buf, sizeof(buf), "PC: 0x%04X", cpu.PC); drawText(buf, x, lineHeight * 8);
    snprintf(buf, sizeof(buf), "SP: 0x%04X", cpu.SP); drawText(buf, x, lineHeight * 9);
    snprintf(buf, sizeof(buf), "LCDC: 0b%s", std::bitset<8>(lcdcBin).to_string().c_str()); drawText(buf, x, lineHeight * 10); // draw the bits out here since I want to see each part's effect
    snprintf(buf, sizeof(buf), "STAT: 0b%s", std::bitset<8>(statBin).to_string().c_str()); drawText(buf, x, lineHeight * 11); // draw the bits out here since I want to see each part's effect
    snprintf(buf, sizeof(buf), "LY: 0x%02X", ly); drawText(buf, x, lineHeight * 12);

}


FileHandler Emulator::getFileFromUser() {
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
    return FileHandler("");
}