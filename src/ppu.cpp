#include "ppu.hpp"
#include "gameboy.hpp"
#include "utils/bit.hpp"

Ppu::Ppu() {}


void Ppu::reset() {
    tileData.fill(0);
    for (int i = 0; i < frameBuffer.size(); i++) {
        if (i % 2 == 0) {
            frameBuffer[i] = 1;
        } else {
            frameBuffer[i] = 0;
        }
    }
    palette = BlackWhite;
    // palette = GameboyGreen;
}


Ppu::~Ppu() {

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
            intReq = getBit(lcdStat, 3); // interrupt request equal to 3th bit of STAT (Mode 2 select)
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
            lcdStat = setBit(lcdStat, 2); // set the 2th bit to 1
            if (getBit(lcdStat, 6)) { // check 6th bit of STAT
                Byte IFreg = memory.readByte(Mmu::IF);
                IFreg = setBit(IFreg, 1); // set the 1th bit to 1
                memory.writeByte(Mmu::IF, IFreg);
            } else {
                lcdStat = resetBit(lcdStat, 1);  // set the 1th bit to 0
            }
        }
        memory.writeByte(Mmu::STAT, lcdStat);
    }
}


void Ppu::loadOamToFrameBuffer(Mmu &memory, Byte currentLine) {
    // https://gbdev.io/pandocs/OAM.html
    Byte lcdc = memory.readByte(Mmu::LCDC);
    for (int i = 0; i < oamSize; i+=4) {

        bool mode = getBit(lcdc, 2); // 0 = 8x8, 1 = 8x16
        int yPos = memory.readByte(oamStart + i) - 16; // the data is given as yPos + 16
        int xPos = memory.readByte(oamStart + i + 1) - 8; // the data is given as xPos + 8
        Byte tileId = memory.readByte(oamStart + i + 2);
        Byte attFlags = memory.readByte(oamStart + i + 3); // Attributes/Flags
        bool priority = getBit(attFlags, 7);
        bool yFlip = getBit(attFlags, 6);
        bool xFlip = getBit(attFlags, 5);
        bool dmgPalette = getBit(attFlags, 4);
        Byte objPalette = memory.readByte(dmgPalette ? Mmu::OBP1 : Mmu::OBP0);
        Byte ySize = mode ? 16 : 8;
        // CGB only
        // bool bank = getBit(attFlags, 3);
        // Byte cgbPalette = attFlags & 0x7; // gets the last 3 bits (0b0111)

        if ((currentLine >= yPos) && (currentLine < yPos + ySize)) { // does the LY contain the sprite?
            int rowUsed = currentLine - yPos;
            if (yFlip) {
                rowUsed -= ySize;
                rowUsed *= -1;
            }
            Word tileAddress = 0x8000 + (tileId * 16) + (rowUsed * 2); // sprites always read starting from 0x8000
            Byte lo = memory.readByte(tileAddress);
            Byte hi = memory.readByte(tileAddress + 1);
            for (int col = 7; col >= 0; --col) {
                if (xPos < 0 || yPos < 0) continue;
                int colUsed = col;
                if (xFlip) {
                    colUsed -= 7;
                    colUsed *= -1;
                }

                Byte paletteId = getBit(lo, 7 - colUsed) | (getBit(hi, 7 - colUsed) << 1);

                Byte colorIndex = objPalette >> (paletteId * 2) & 0b11;
                if (colorIndex == 0) continue; // white is transparent, so no need to render it
                SDL_Color c;
                switch (colorIndex) {
                    case 1: c = palette.getColor(LIGHT_GRAY);   break;
                    case 2: c = palette.getColor(DARK_GRAY);    break;
                    case 3: c = palette.getColor(BLACK);        break;
                    default: c = palette.getColor(WHITE);       break;
                }

                int xPixel = 0 - colUsed;
                xPixel += 7;
                // int index = xPos + xPixel;
                int index = ((currentLine * GAMEBOY_WIDTH) + xPos + xPixel) * 3;
                if (priority && frameBuffer[index] > 0) continue;

                frameBuffer[index] = c.r;
                frameBuffer[index + 1] = c.g;
                frameBuffer[index + 2] = c.b;
            }
        }
    }
}

void Ppu::loadScanline(Mmu &memory, Byte currentLine) {
    if (currentLine >= GAMEBOY_HEIGHT) {
        printf("loadScanline out of bounds: %d\n", currentLine);
        return;
    }

    Byte lcdc = memory.readByte(Mmu::LCDC); // LCD control
    Byte scrollx = memory.readByte(Mmu::SCX);
    Byte scrolly = memory.readByte(Mmu::SCY);
    Byte winX = memory.readByte(Mmu::WX);
    Byte winY = memory.readByte(Mmu::WY);
    Word bgPalette = memory.readByte(Mmu::BGP);
    Word tileMapStart = (getBit(lcdc, 3) == 1) ? 0x9c00 : 0x9800;
    Word tileDataStart = (getBit(lcdc, 4) == 1) ? 0x8000 : 0x9000;
    Byte currentTileRow = ((currentLine + scrolly) / 8) % 32; // wraps back around at the end of the 32x32 block

    for (int col = 0; col < GAMEBOY_WIDTH; col++) {

        Byte currentTileCol = ((col + scrollx) / 8) % 32; // gets the location in the window

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
        Byte tileRow = (currentLine + scrolly) % 8;
        Byte lo = memory.readByte(tileAddress + (tileRow * 2));     // low byte of the tile to show
        Byte hi = memory.readByte(tileAddress + (tileRow * 2) + 1); // high byte of the tile

        Byte bitToShift = (7 - (col + scrollx) % 8);
        Byte paletteId = getBit(lo, bitToShift) | (getBit(hi, bitToShift) << 1);
        // formula used to determine color value from bg palette
        // shifting over 2 bits at a time (change the 2 to the number of bits shifted)
        Byte colorIndex = bgPalette >> (paletteId * 2) & 0b11;
        SDL_Color c;
        switch (colorIndex) {
            case 0: c = palette.getColor(WHITE);        break;
            case 1: c = palette.getColor(LIGHT_GRAY);   break;
            case 2: c = palette.getColor(DARK_GRAY);    break;
            case 3: c = palette.getColor(BLACK);        break;
            default: c = palette.getColor(WHITE);       break;
        }

        int index = ((currentLine * 160) + col) * 3;
        frameBuffer[index + 0] = c.r; // r
        frameBuffer[index + 1] = c.g; // g
        frameBuffer[index + 2] = c.b; // b
    }    
}


void Ppu::updateGraphics(Cpu &cpu, Mmu &memory, uint cycles) {
    
    LCDStatus(memory);

    bool isLcdEnabled = memory.readByte(Mmu::LCDC) >> 7;
    if (!isLcdEnabled) {
        return;
    }
    
    scanlineCounter -= cycles;

    if (scanlineCounter <= 0) {
        Byte currentLine = memory.readByte(Mmu::LY);
        
        scanlineCounter = 456;
        
        if (currentLine == 144) {
            // set bit 0 of IF to request vblank interrupt
            memory.writeByte(memory.IF, memory.readByte(memory.IF) | 0x01);
        } else if (currentLine > 153) {
            memory.writeByte(Mmu::LY, 0);
        } else if (currentLine < 144) {
            // printf("rendering scanline: %d\n", currentLine);
            loadScanline(memory, currentLine);
            loadOamToFrameBuffer(memory, currentLine);
        }
        memory.writeByte(Mmu::LY, memory.readByte(Mmu::LY) + 1);
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
                    default: c = palette.getColor(WHITE);       break;
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
