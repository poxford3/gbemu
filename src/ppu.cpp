#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/bit.hpp"

Ppu::Ppu() {}


void Ppu::reset() {
    tileData.fill(0);
    oldIntCheck = false;
    for (size_t i = 0; i < frameBuffer.size(); i++) frameBuffer[i] = 0;
    palette.selectedPalette = BlackWhite;
}


Ppu::~Ppu() {}


void Ppu::LCDStatus(Mmu &memory, uint &cycles, Byte &lcdc) {
    Byte lcdStat = memory.readByte(Mmu::STAT);
    Byte LY = memory.readByte(Mmu::LY); // current line
    Byte LYC = memory.readByte(Mmu::LYC); // current line compare
    if (!(getBit(lcdc, LCD_PPU_ENABLE))) { // if the 7th bit of LCDC (LCD Enable) if false
        scanlineCounter = 456;
        memory.writeByte(Mmu::LY, 0);
        lcdStat &= 0xFC; // 0 out the bottom 2 bits
        lcdStat |= HBLANK; // set the PPU Mode to HBlank (bottom 2 bits)
        memory.ioRegisters[Mmu::STAT - 0xFF00] = lcdStat;
        return;
    }

    Byte newMode;
    bool modeInt;
    if (LY >= GAMEBOY_HEIGHT) {
        newMode = VBLANK; // mode 1
        modeInt = getBit(lcdStat, MODE1_INT);
    } else if (scanlineCounter >= 456 - MODE2LEN) {
        newMode = OAM; // mode 2
        modeInt = getBit(lcdStat, MODE2_INT);
    } else if (scanlineCounter >= 456 - (MODE2LEN + MODE3LEN)) { // TODO maybe make this into one combined value (mode 3 bounds)
        newMode = DRAWING; // mode 3
    } else {
        newMode = HBLANK; // mode 0
        modeInt = getBit(lcdStat, MODE0_INT);
    }

    // set the lcd stat bits (bottom 2 bits)
    Byte newLcdStat = lcdStat & 0xFC;
    newLcdStat |= newMode;

    bool LYeqLYC = (LY == LYC);
    newLcdStat = LYeqLYC ? setBit(newLcdStat, LYC_FLAG) : resetBit(newLcdStat, LYC_FLAG);

    bool newIntCheck = ((LYeqLYC && getBit(newLcdStat, LYC_INT)) ||
        ((newMode == OAM) && getBit(newLcdStat, MODE2_INT)) ||
        ((newMode == HBLANK) && getBit(newLcdStat, MODE0_INT)) ||
        ((newMode == VBLANK) && getBit(newLcdStat, MODE1_INT)));


    // todo optimize this to only run checks as they happen, not every check every time
    if (!oldIntCheck && newIntCheck) { // if it goes from 0->1
        // check if any of the LCD interrupt conditions are met
        Byte IFreg = memory.readByte(Mmu::IF);
        IFreg = setBit(IFreg, Cpu::Interrupt::LCD_STAT); // set the 1th bit to 1
        memory.writeByte(Mmu::IF, IFreg);
    }

    oldIntCheck = newIntCheck;

    memory.ioRegisters[Mmu::STAT - 0xFF00] = newLcdStat;
}


void Ppu::loadOamToFrameBuffer(Mmu &memory, Byte &currentLine, Byte &lcdc) {

    Byte oamCounter = 0;

    // https://gbdev.io/pandocs/OAM.html
    for (int i = 0; i < oamSize; i+=4) {

        bool mode = getBit(lcdc, OBJ_SIZE); // 0 = 8x8, 1 = 8x16
        int yPos = memory.readByte(oamStart + i) - 16; // the data is given as yPos + 16
        int xPos = memory.readByte(oamStart + i + 1) - 8; // the data is given as xPos + 8
        Byte ySize = mode ? 16 : 8;

        if ((currentLine >= yPos) && (currentLine < yPos + ySize)) { // does the LY contain the sprite?
            // printf("rendering sprite at LY=%d, xPos=%d, yPos=%d\n", currentLine, xPos, yPos);

            oamCounter++;
            if (oamCounter > 10) return;

            Byte tileId = memory.readByte(oamStart + i + 2);
            Byte attFlags = memory.readByte(oamStart + i + 3); // Attributes/Flags
            bool priority = getBit(attFlags, 7);
            bool yFlip = getBit(attFlags, 6);
            bool xFlip = getBit(attFlags, 5);
            bool dmgPalette = getBit(attFlags, 4);
            Byte objPalette = memory.readByte(dmgPalette ? Mmu::OBP1 : Mmu::OBP0);
            
            // CGB only
            // bool bank = getBit(attFlags, 3);
            // Byte cgbPalette = attFlags & 0x7; // gets the last 3 bits (0b0111)

            int rowUsed = currentLine - yPos;
            if (yFlip) {
                rowUsed -= (ySize - 1);
                rowUsed *= -1;
            }
            Byte tileIndex = mode ? (tileId & 0xFE) : tileId;
            Word tileAddress = 0x8000 + (tileIndex * 16) + (rowUsed * 2); // sprites always read starting from 0x8000
            Byte lo = memory.readByte(tileAddress);
            Byte hi = memory.readByte(tileAddress + 1);
            for (int col = 7; col >= 0; --col) {
                if (xPos + col < 0 || xPos + col >= GAMEBOY_WIDTH) continue;
                int colUsed = col;
                // if (xFlip) {
                //     colUsed -= 7;
                //     colUsed *= -1;
                // }

                int tileX = xFlip ? 7 - col : col;

                // Byte paletteId = getBit(lo, 7 - colUsed) | (getBit(hi, 7 - colUsed) << 1);
                Byte paletteId = getBit(lo, 7 - tileX) | (getBit(hi, 7 - tileX) << 1);

                Byte colorIndex = objPalette >> (paletteId * 2) & 0b11;
                SDL_Color c;
                switch (colorIndex) {
                    case 0: c = palette.getColor(WHITE);        break;
                    case 1: c = palette.getColor(LIGHT_GRAY);   break;
                    case 2: c = palette.getColor(DARK_GRAY);    break;
                    case 3: c = palette.getColor(BLACK);        break;
                }

                if (paletteId > 0) { // if the color is not transparent, draw it
                    int index = ((currentLine * GAMEBOY_WIDTH) + xPos + col) * 3;
                    // if (priority && frameBuffer[index] > 0) continue;

                    frameBuffer[index + 0] = c.r;
                    frameBuffer[index + 1] = c.g;
                    frameBuffer[index + 2] = c.b;
                }
            }
        }
    }
}


void Ppu::loadWinToFrameBuffer(Mmu &memory, Byte &currentLine, Byte &lcdc) {
    if (currentLine >= GAMEBOY_HEIGHT) {
        printf("loadScanline out of bounds: %d\n", currentLine);
        return;
    }
    int winX = memory.readByte(Mmu::WX) - 7; // window x pos is offset by 7, subtracting 7 to get actual pos
    int winY = memory.readByte(Mmu::WY);
    Word winPalette = memory.readByte(Mmu::BGP); // window shares the palette with the bg
    windowLineCounter++; // every time a window pixel is rendered, add to windowLineCounter
    Word tileMapStart = (getBit(lcdc, WIN_TILE_MAP_SELECT) == 1) ? 0x9c00 : 0x9800;
    Word tileDataStart = (getBit(lcdc, BG_WIN_TILE_DATA_SELECT) == 1) ? 0x8000 : 0x9000;

    for (int col = 0; col < GAMEBOY_WIDTH; col++) {

        if (col < winX) continue; // if the current column is less than the window x pos, skip to the next column
        if (currentLine < winY) continue; // if the current line is less than the window y pos, skip to the next column
        Byte currentTileRow = (windowLineCounter - 1) / 8; // windowLineCounter is incremented every time a window pixel is rendered, so subtracting 1 to get the correct row
        Byte currentTileCol = (col - winX) / 8;

        Word tileMapAddress = tileMapStart + (currentTileRow * 32) + currentTileCol; // tile map address from the given row and col, offset by the tile map start
        Byte tileId = memory.readByte(tileMapAddress);
        Word tileAddress;
        if (tileDataStart == 0x8000)
        {
            tileAddress = tileDataStart + tileId * 16;
        }
        else {
            tileAddress = tileDataStart + (int8_t)tileId * 16;
        }
        Byte tileRow = (currentLine - winY) % 8;
        Byte lo = memory.readByte(tileAddress + (tileRow * 2));     // low byte of the tile to show
        Byte hi = memory.readByte(tileAddress + (tileRow * 2) + 1); // high byte of the tile

        Byte bitToShift = (7 - (col - winX) % 8);
        Byte paletteId = getBit(lo, bitToShift) | (getBit(hi, bitToShift) << 1);
        Byte colorIndex = winPalette >> (paletteId * 2) & 0b11;
        SDL_Color c;
        switch (colorIndex) {
            case 0: c = palette.getColor(WHITE);        break;
            case 1: c = palette.getColor(LIGHT_GRAY);   break;
            case 2: c = palette.getColor(DARK_GRAY);    break;
            case 3: c = palette.getColor(BLACK);        break;
        }

        int index = ((currentLine * GAMEBOY_WIDTH) + col) * 3;
        frameBuffer[index + 0] = c.r; // r
        frameBuffer[index + 1] = c.g; // g
        frameBuffer[index + 2] = c.b; // b
    }
}


void Ppu::loadBgToFrameBuffer(Mmu &memory, Byte &currentLine, Byte &lcdc) {
    if (currentLine >= GAMEBOY_HEIGHT) {
        printf("loadScanline out of bounds: %d\n", currentLine);
        return;
    }

    Byte scrollx = memory.readByte(Mmu::SCX);
    Byte scrolly = memory.readByte(Mmu::SCY);
    Word bgPalette = memory.readByte(Mmu::BGP); // window and background share the palette
    Word tileMapStart = (getBit(lcdc, BG_TILE_MAP_SELECT) == 1) ? 0x9c00 : 0x9800;
    Word tileDataStart = (getBit(lcdc, BG_WIN_TILE_DATA_SELECT) == 1) ? 0x8000 : 0x9000;
    uint currentTileRow = ((currentLine + scrolly) / 8) % 32; // wraps back around at the end of the 32x32 block

    for (int col = 0; col < GAMEBOY_WIDTH; col++) {

        uint currentTileCol = ((col + scrollx) / 8) % 32; // gets the location in the windows

        Word tileMapAddress = tileMapStart + (currentTileRow * 32) + currentTileCol; // tile map address from the given row and col, offset by the tile map start
        Byte tileId = memory.readByte(tileMapAddress);
        Word tileAddress;
        if (tileDataStart == 0x8000) {
            tileAddress = tileDataStart + tileId * 16; // 16 is the size of a tile in bits (2 bytes)
        }
        else {
            tileAddress = tileDataStart + (int8_t)tileId * 16;
        }
        uint tileRow = (currentLine + scrolly) % 8;
        Byte lo = memory.readByte(tileAddress + (tileRow * 2));     // low byte of the tile to show
        Byte hi = memory.readByte(tileAddress + (tileRow * 2) + 1); // high byte of the tile

        Byte bitToShift = (7 - (col + scrollx) % 8);
        Byte paletteId = getBit(lo, bitToShift) | (getBit(hi, bitToShift) << 1);
        Byte colorIndex = bgPalette >> (paletteId * 2) & 0b11;
        SDL_Color c;
        switch (colorIndex) {
            case 0: c = palette.getColor(WHITE);        break;
            case 1: c = palette.getColor(LIGHT_GRAY);   break;
            case 2: c = palette.getColor(DARK_GRAY);    break;
            case 3: c = palette.getColor(BLACK);        break;
            default:c = palette.getColor(BLACK);        break;
        }

        int index = ((currentLine * GAMEBOY_WIDTH) + col) * 3;
        frameBuffer[index + 0] = c.r; // r
        frameBuffer[index + 1] = c.g; // g
        frameBuffer[index + 2] = c.b; // b
    }
}


void Ppu::renderScanline(Mmu &memory, Byte &currentLine, Byte &lcdc) {
    // printf("rendering scanline: %d\n", currentLine);
    if (memory.readByte(Mmu::WY) == currentLine && !winYcondition) winYcondition = true; // stays latched if it's been set. only will be set to false at the beginning of VBlank

    Byte BGandWinEnabled = getBit(lcdc, BG_WIN_ENABLE); // if this is false, the background and window are disabled, and the bg&window are white
    if (!BGandWinEnabled) {
        std::fill(frameBuffer.begin() + (currentLine * GAMEBOY_WIDTH * 3), frameBuffer.begin() + ((currentLine + 1) * GAMEBOY_WIDTH * 3), 255); // make the current line white
    } else {
        loadBgToFrameBuffer(memory, currentLine, lcdc);
        if (
            winYcondition &&
            getBit(lcdc, WIN_ENABLE) &&
            static_cast<int>(memory.readByte(Mmu::WX) - 7) < GAMEBOY_WIDTH
        ) {
            loadWinToFrameBuffer(memory, currentLine, lcdc);
        }
    }

    loadOamToFrameBuffer(memory, currentLine, lcdc);
}


void Ppu::updateGraphics(Mmu &memory, uint cycles) {

    scanlineCounter -= cycles;
    Byte lcdc = memory.readByte(Mmu::LCDC); // LCD control
    LCDStatus(memory, cycles, lcdc);

    bool isLcdEnabled = getBit(lcdc, LCD_PPU_ENABLE);
    if (!isLcdEnabled) {
        return;
    }

    // reorganization thanks to @peter1508 on emudev discord
    if (scanlineCounter <= 0) {
        Byte currentLine = memory.ioRegisters[Mmu::LY - 0xFF00];
        scanlineCounter += 456; // reset scaline counter for the next line
        if (currentLine < 144) {
            renderScanline(memory, currentLine, lcdc);
        }
        // memory.ioRegisters[Mmu::LY - 0xFF00] = currentLine;
        currentLine++;
        if (currentLine > 153) {
            currentLine = 0;
        } else
        if (currentLine == GAMEBOY_HEIGHT) { // if end of line, enter vblank
            winYcondition = false; // window Y condition set to false every VBLank
            windowLineCounter = 0; // resets at the beginning of each VBlank
            // set bit 0 of IF to request vblank interrupt
            Byte IFreg = memory.readByte(Mmu::IF);
            IFreg = setBit(IFreg, Cpu::Interrupt::VBLANK);
            memory.writeByte(Mmu::IF, IFreg);
        }
        memory.ioRegisters[Mmu::LY - 0xFF00] = currentLine;
    }
}


void Ppu::loadTileData(Mmu &memory) {
    Word tileDataStart = 0x8000; // tile block 0 starts at $8000
    const int tilesPerRow = 16;
    Byte bgPalette = memory.readByte(Mmu::BGP);
    // printf("0x%02x\n", bgPalette);
    for (int tile = 0; tile < 384; tile++) { // 384 tiles across the 3 blocks ($8000 - $97FF => 6144 bytes / 16 bytes per tile = 384)
        Word tileAddress = tileDataStart + (tile * 16); // tiles 16 bytes wide
        int tileX = (tile % tilesPerRow) * 8;
        int tileY = (tile / tilesPerRow) * 8;
        for (int row = 0; row < 8; row++) {
            Byte lo = memory.readByte(tileAddress + (row * 2)); // low byte of the tile row
            Byte hi = memory.readByte(tileAddress + (row * 2) + 1); // high byte of the tile row
            for (int col = 0; col < 8; col++) {
                Byte paletteId = getBit(lo, 7 - col) | (getBit(hi, 7 - col) << 1);
                // formula used to determine color value from bg palette
                // shifting over 2 bits at a time (change the 2 to the number of bits shifted)
                Byte colorIndex = bgPalette >> (paletteId * 2) & 0b11;
                SDL_Color c;
                switch (colorIndex) {
                    case 0: c = palette.getColor(WHITE);        break;
                    case 1: c = palette.getColor(LIGHT_GRAY);   break;
                    case 2: c = palette.getColor(DARK_GRAY);    break;
                    case 3: c = palette.getColor(BLACK);        break;
                }
                int px = tileX + col;
                int py = tileY + row;
                int index = (py * tilesPerRow * 8 + px) * 3;

                tileData[index]     = c.r;
                tileData[index + 1] = c.g;
                tileData[index + 2] = c.b;
            }
        }
    }
}
