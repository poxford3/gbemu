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

    palette = BlackWhite;
    // palette = GameboyGreen;
 
    running = true;
}


void Ppu::stop() {
    running = false;
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


Ppu::~Ppu() {
    if (renderer != nullptr) {
        stop();
    }
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
    // http://www.codeslinger.co.uk/pages/projects/gameboy/lcd.html
    Byte lcdStat = memory.readByte(Mmu::STAT);
    Byte lcdc = memory.readByte(Mmu::LCDC);
    if (!(getBit(lcdc, 7))) { // if the 7th bit of LCDC (LCD Enable) if false
        scanlineCounter = 456;
        memory.writeByte(Mmu::LY, 0);
        lcdStat &= 0xFC; // 0 out the bottom 2 bits
        setBit(lcdStat, 0);
        memory.writeByte(Mmu::STAT, lcdStat);
        return;
    }

    Byte currentLine = memory.readByte(Mmu::LY);
    Byte currentMode = lcdStat & 0x3; // PPU mode (oam, drawing, hblank, etc)

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
            intReq = getBit(lcdStat, 5); // interrupt request equal to 5th bit of STAT (Mode 2 select)
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
            intReq = getBit(lcdStat, 5); // interrupt request equal to 5th bit of STAT (Mode 2 select)
        }
        
        // if new mode, interrupt flag set
        if (intReq && (mode != currentMode)) {
            Byte IFreg = memory.readByte(Mmu::IF);
            IFreg = setBit(IFreg, 1); // set the 1th bit to 1 (LCD bit)
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

    
    
    for (int col = 0; col < GAMEBOY_WIDTH; col++) {
        Byte lcdc = memory.readByte(Mmu::LCDC); // LCD control
        Byte scrollx = memory.readByte(Mmu::SCX);
        Byte scrolly = memory.readByte(Mmu::SCY);
        Word bgPalette = memory.readByte(Mmu::BGP);
        Word tileMapStart = (getBit(lcdc, 3) == 1) ? 0x9c00 : 0x9800;
        Word tileDataStart = (getBit(lcdc, 4) == 1) ? 0x8000 : 0x8800;
    
        Byte currentTileRow = ((currentLine + scrolly) / 8) % 32; // wraps back around at the end of the 32x32 block
        Byte currentTileCol = ((col + scrollx) / 8) % 32; // gets the location in the window

        Word tileMapAddress = tileMapStart + (currentTileRow * 32) + currentTileCol; // tile map address from the given row and col, offset by the tile map start
        Byte tileId = memory.readByte(tileMapAddress);
        Byte lo = memory.readByte(tileId); // low byte of the tile id
        Byte hi = memory.readByte(tileId + 1); // high byte of the tile id


        // formula used to determine color value from bg palette
        // shifting over 2 bits at a time (change the 2 to the number of bits shifted)
        Byte colorVal = bgPalette << (0b11 * 2) & 0b11;

        std::bitset<16> rowVal;
        // from thethiefmaster on emu discord
        for (int i = 0; i < 8; i++) {
            rowVal.set(i*2, getBit(lo, i));
            rowVal.set(i*2+1, getBit(hi, i));
        }
        uint rowIdx = (currentLine * GAMEBOY_WIDTH + col) * 3; 

        Byte colorIndex = rowVal[7 - (col % 8)] | (rowVal[7 - (col % 8) + 1] << 1);
        SDL_Color c;
        switch (colorIndex) {
            case 0:
                c = palette.getColor(WHITE);
                break;
            case 1:
                c = palette.getColor(LIGHT_GRAY);
                break;
            case 2:
                c = palette.getColor(DARK_GRAY);
                break;
            case 3:
                c = palette.getColor(BLACK);
                break;
            default:
                c = palette.getColor(WHITE);
                break;
        }
        frameBuffer[rowIdx + 0] = c.r; // r
        frameBuffer[rowIdx + 1] = c.g; // g
        frameBuffer[rowIdx + 2] = c.b; // b
    }


    // for (int x = 0; x < GAMEBOY_WIDTH; x++) {
    //     uint index = (currentLine * GAMEBOY_WIDTH + x) * 3; 
    //     if (x % 2 == 0 && currentLine % 2 == 0) {
    //         frameBuffer[index + 0] = 0xFF; // r
    //         frameBuffer[index + 1] = 0x00; // g
    //         frameBuffer[index + 2] = 0xFF; // b purple
    //     } else {
    //         frameBuffer[index + 0] = 0x00; // r
    //         frameBuffer[index + 1] = 0x00; // g
    //         frameBuffer[index + 2] = 0x00; // b black
    //     }
    // }
    
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
            // stop();
        }
    }
    if (!paused) {

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

    } else {
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }
    handleInput();

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
    } else if (state[SDL_SCANCODE_P]) {
        palette.selectedPalette = PaletteOptions::BlackWhite;
    } else if (state[SDL_SCANCODE_G]) {
        palette.selectedPalette = PaletteOptions::GameboyGreen;
    } else if (state[SDL_SCANCODE_TAB]) {
        DEBUG = !DEBUG;
        resize();
    } else if (state[SDL_SCANCODE_BACKSPACE]) {
        printf("pausing\n");
        paused = !paused;
        SDL_Delay(100); // todo, change this so it applies to the gameboy, not the ppu only
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

