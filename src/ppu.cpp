#include <nfd.h>
#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/bit.hpp"

Ppu::Ppu() {
    window = nullptr;
    renderer = nullptr;
    font = nullptr;
    gbTexture = nullptr;
}


void Ppu::init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be intiialized, SDL error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("gbemu",
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    (EMULATOR_SCREEN_WIDTH() + (DEBUG ? MEMORY_SECTION_WIDTH : 0)), // allow extra room to render parts of the CPU/MMU
                    EMULATOR_SCREEN_HEIGHT(),
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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
                        GAMEBOY_WIDTH,
                        GAMEBOY_HEIGHT
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

    // palette = BlackWhite;
    palette = GameboyGreen;
 
    running = true;
}


Ppu::~Ppu() {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    SDL_Quit();
}


SDL_Renderer* Ppu::GetRenderer() {
    return renderer;
}


void Ppu::resize() {
    SDL_SetWindowSize(window, (EMULATOR_SCREEN_WIDTH() + (DEBUG ? MEMORY_SECTION_WIDTH : 0)), EMULATOR_SCREEN_HEIGHT());
}


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


void Ppu::LCDStatus(Mmu &memory) {
    Byte lcdStat = memory.readByte(Mmu::STAT);
    Byte lcdc = memory.readByte(Mmu::LCDC);
    if (!(getBit(lcdc, 7))) { // if the 7th bit of LCDC (LCD Enable) if false
        scanlineCounter = 456;
        memory.writeByte(Mmu::LY, 0);
        lcdStat &= 252;
        setBit(lcdStat, 0);
        memory.writeByte(Mmu::STAT, lcdStat);
        return;
    }

    Byte currentLine = memory.readByte(Mmu::LY);
    Byte currentMode = lcdStat & 0x3;

    Byte mode = 0;
    bool intReq = false;

    if (currentLine >= 144) {
        mode = 1;
        lcdStat = setBit(lcdStat, 0); // set the 0th bit to 1
        lcdStat = resetBit(lcdStat, 1); // set the 1th bit to 0
        intReq = getBit(lcdStat, 4); // interrupt request equal to 4th bit of STAT
    } else {
        int mode2bounds = 456-80;
        int mode3bounds = mode2bounds - 172;

        // mode 2
        if (scanlineCounter >= mode2bounds) {
            mode = 2;
            lcdStat = setBit(lcdStat, 1); // set the 1th bit to 1
            lcdStat = resetBit(lcdStat, 0); // set the 0th bit to 0
            intReq = getBit(lcdStat, 5); // interrupt request equal to 5th bit of STAT
        } else if (scanlineCounter >= mode3bounds) {
            // mode 3
            mode = 3;
            lcdStat = setBit(lcdStat, 0); // set the 0th bit to 1
            lcdStat = setBit(lcdStat, 1); // set the 1th bit to 1
        } else {
            // mode 0
            mode = 0;
            lcdStat = resetBit(lcdStat, 0); // set the 0th bit to 0
            lcdStat = resetBit(lcdStat, 1); // set the 1th bit to 0
            intReq = getBit(lcdStat, 5); // interrupt request equal to 5th bit of STAT
        }
        
        // if new mode, interrupt flag set
        if (intReq && (mode != currentMode)) {
            Byte IFreg = memory.readByte(Mmu::IF);
            IFreg = setBit(IFreg, 1); // set the 1th bit to 1
            memory.writeByte(Mmu::IF, IFreg);
        }

        Byte currLY = memory.readByte(Mmu::LY);
        Byte currLYC = memory.readByte(Mmu::LYC);
        if (currLY == currLYC) {
            lcdStat = setBit(lcdStat, 6); // set the 6th bit to 1
            if (getBit(lcdStat, 6)) { // check 6th bit of STAT
                Byte IFreg = memory.readByte(Mmu::IF);
                IFreg = setBit(IFreg, 1); // set the 1th bit to 1
                memory.writeByte(Mmu::IF, IFreg);
            } else {
                lcdStat = resetBit(lcdStat, 2);  // set the 2th bit to 0
            }
        }
        memory.writeByte(Mmu::STAT, lcdStat);
    }
}


void Ppu::loadScanline(Mmu &memory, Byte currentLine) {
    if (currentLine < 0 || currentLine >= GAMEBOY_HEIGHT) {
        printf("loadScanline out of bounds: %d\n", currentLine);
        return;
    }

    Byte lcdc = memory.readByte(Mmu::LCDC); // LCD control
    Byte scrollx = memory.readByte(Mmu::SCX);
    Byte scrolly = memory.readByte(Mmu::SCY);
    Word tileMapStart = (getBit(lcdc, 3) == 1) ? 0x9c00 : 0x9800;
    Word tileDataStart = (getBit(lcdc, 4) == 1) ? 0x8000 : 0x8800;
    Byte currentTileRow = ((currentLine + scrolly) / 8) % 32; // wraps back around at the end of the 32x32 block
    Byte currentTileCol = ((currentLine + scrollx));

    Word tileMapAddress = tileMapStart + (currentTileRow * 32) + currentTileCol; // 
    Byte tileId = memory.readByte(tileMapAddress);

    Word bgPalette = memory.readByte(Mmu::BGP);
    Byte pal00 = bgPalette & 0b11;
    Byte pal01 = (bgPalette >> 2) & 0b11;
    Byte pal10 = (bgPalette >> 4) & 0b11;
    Byte pal11 = (bgPalette >> 6) & 0b11;

    // formula used to determine color value from bg palette
    // shifting over 2 bits at a time
    Byte colorVal = bgPalette << (0b11 * 2) & 0b11;


    Byte lo = memory.readByte(tileDataStart + currentLine); // low byte of the tile map data
    Byte hi = memory.readByte(tileDataStart + currentLine + 1); // high byte
    // printf("hi 0x%02X lo 0x%02X, ", hi, lo);

    std::bitset<16> rowVal;
    // from thethiefmaster on emu discord
    for (int i = 0; i < 8; i++) {
        rowVal.set(i*2, getBit(lo, i));
        rowVal.set(i*2+1, getBit(hi, i));
    }
    // printf("rowval 0x%02X\n", rowVal);


    for (int x = 0; x < GAMEBOY_WIDTH; x++) {
        uint index = (currentLine * GAMEBOY_WIDTH + x) * 3; 
        if (x % 2 == 0 && currentLine % 2 == 0) {
            frameBuffer[index + 0] = 0xFF; // r
            frameBuffer[index + 1] = 0x00; // g
            frameBuffer[index + 2] = 0xFF; // b purple
        } else {
            frameBuffer[index + 0] = 0x00; // r
            frameBuffer[index + 1] = 0x00; // g
            frameBuffer[index + 2] = 0x00; // b black
        }
    }
}


void Ppu::updateGraphics(Cpu &cpu, Mmu &memory, uint cycles) {
    
    LCDStatus(memory);

    bool isLcdEnabled = memory.readByte(Mmu::LCDC) >> 7;
    if (isLcdEnabled) {
        scanlineCounter -= cycles;
    } else {
        return;
    }

    if (scanlineCounter <= 0) {
        memory.writeByte(Mmu::LY, memory.readByte(Mmu::LY) + 1);
        Byte currentLine = memory.readByte(Mmu::LY);

        scanlineCounter = 456;

        if (currentLine == 144) {
            // set bit 0 of IF to request vblank interrupt
            memory.writeByte(memory.IF, memory.readByte(memory.IF) | 0x01);
            drawFrame(cpu, memory);
            SDL_Delay(16); // 16 ms = 60 fps (1/60)
            // SDL_Delay(1000);
        } else if (currentLine > 153) {
            memory.writeByte(Mmu::LY, 0);
        } else if (currentLine < 144) {
            loadScanline(memory, currentLine);
        }
    }
}


void Ppu::drawFrame(Cpu &cpu, Mmu &memory) {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }

    // functioning as the background at the moment
    SDL_Color c = palette.getColor(DARK_GRAY);

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer);

    if (DEBUG) {
        displayMemory(cpu, memory);
        SDL_Rect separator = {static_cast<int>(EMULATOR_SCREEN_WIDTH()), 0, 1, static_cast<int>(EMULATOR_SCREEN_HEIGHT())};
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderFillRect(renderer, &separator);
    }


    SDL_Rect gameboySection = {0, 0, static_cast<int>(EMULATOR_SCREEN_WIDTH()), static_cast<int>(EMULATOR_SCREEN_HEIGHT())};
    SDL_UpdateTexture(gbTexture, NULL, frameBuffer, GAMEBOY_WIDTH * 3);
    SDL_RenderCopy(renderer, gbTexture, NULL, &gameboySection);


    // present the frame
    SDL_RenderPresent(renderer);

    if (1==2) {
        FileHandler fileH = getFileFromUser();
        std::vector<Byte> rom = fileH.readFile();

        if (rom.size() > 1) {
            Gameboy gameboy(rom);
        }
    }

    handleInput();

    // if (SDL_PollEvent(&event) == SDL_MouseButtonEvent(&event)) {
    //     // todo add in clicking to open NFD
    // }
}


void Ppu::handleInput() {
    const Byte* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_1]) {
        winScale = 1;
        resize();
    } else if (state[SDL_SCANCODE_2]) {
        winScale = 2;
        resize();
    } else if (state[SDL_SCANCODE_3]) {
        winScale = 3;
        resize();
    }

}


void Ppu::displayMemory(Cpu &cpu, Mmu &memory) {
    char buf[108];
    uint lineHeight = 30;
    uint x = EMULATOR_SCREEN_WIDTH() + 5; // +5 for a bit of left padding
    Byte lcdcBin = memory.readByte(Mmu::LCDC);
    Byte statBin = memory.readByte(Mmu::STAT);
    Byte ly = memory.readByte(Mmu::LY);
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
    snprintf(buf, sizeof(buf), "LCDC: 0b%s", std::bitset<8>(lcdcBin).to_string().c_str()); drawText(buf, x, lineHeight * 10); // draw the bits out here since I want to see each part's effect
    snprintf(buf, sizeof(buf), "STAT: 0b%s", std::bitset<8>(statBin).to_string().c_str()); drawText(buf, x, lineHeight * 11); // draw the bits out here since I want to see each part's effect
    snprintf(buf, sizeof(buf), "LY: 0x%02X", ly); drawText(buf, x, lineHeight * 12);

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