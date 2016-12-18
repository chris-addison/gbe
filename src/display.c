#include "types.h"
#include "memory.h"
#include "display.h"
#include "window.h"
#include "stdio.h"

const uint8 COLOURS[] = {0xFF, 0xC0, 0x60, 0x00};
uint8 backgroundColourOffset[] = {0, 1, 2, 3};
uint8 spritePaletteZero[] = {0, 1, 2, 3};
uint8 spritePaletteOne[] = {0, 1, 2, 3};

uint8 frameBuffer[4 * DISPLAY_WIDTH * DISPLAY_HEIGHT];
uint8 tiles[384][8][8];

// Update colour palette for the background
void updateBackgroundColour(uint8 value) {
    for (uint8 i = 0; i < 4; i++) {
        // Mask the two bits to get the background colour set
        backgroundColourOffset[i] = (value >> (i * 2)) & 0b11;
    }
}

// Update colour palette for the sprites
void updateSpritePalette(uint8 palette, uint8 value) {
    for (uint8 i = 0; i < 4; i++) {
        // Mask the two bits to get the background colour set
        if (palette) { // palette 1
            spritePaletteOne[i] = (value >> (i * 2)) & 0b11;
        } else { // pallete 0
            spritePaletteZero[i] = (value >> (i * 2)) & 0b11;
        }
    }
}

// Load all tiles. 384 in total as set 0 overlaps set 1 by 128 tiles.
void loadTiles(cpu_state *cpu) {
    uint8 *vram = cpu->MEM + 0x8000;
    for (int tileNum = 0; tileNum < 384; tileNum++) {
        for (int y = 0; y < 8; y++) {
            uint8 byteLow = *(vram + 2*y + tileNum*16);
            uint8 byteHigh = *(vram + 2*y + 1 + tileNum*16);
            for (int x = 0; x < 8; x++) {
                tiles[tileNum][x][y] = ((byteLow & 0x80) >> 7) | ((byteHigh & 0x80) >> 6);
                byteLow <<= 1;
                byteHigh <<= 1;
            }
        }
    }
}

//bool backgroundEnabled = true;
// Load Background into framebuffer
static void loadBackgroundLine(uint8 scanLine, bool tileSet, cpu_state *cpu) {
    //if (scanLine == 0 || scanLine > 143) {
        //backgroundEnabled = readBit(0, &cpu->MEM[LCDC]);
    //}
    // Check if background enabled
    if (readBit(0, &cpu->MEM[LCDC])) {
        int mapLocation = (readBit(3, &cpu->MEM[LCDC])) ? 0x9C00 : 0x9800;
        // Draw tileset onto the window
        mapLocation += ((scanLine + cpu->MEM[SCROLL_Y]) >> 3) << 5;
        short lineOffset = cpu->MEM[SCROLL_X] >> 3;
        short x = cpu->MEM[SCROLL_X] & 7;
        short y = (scanLine + cpu->MEM[SCROLL_Y]) & 7;
        short tile = cpu->MEM[mapLocation + lineOffset];
        // Tile set 0 is numbered -128 to 128
        if (!tileSet) {
            tile = ((int8) tile) + 256;
        }
        short drawOffset = DISPLAY_WIDTH * scanLine;
        for (int i = 0; i < DISPLAY_WIDTH; i++) {
            frameBuffer[(drawOffset + i)*4 + 0] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
            frameBuffer[(drawOffset + i)*4 + 1] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
            frameBuffer[(drawOffset + i)*4 + 2] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
            frameBuffer[(drawOffset + i)*4 + 3] = 0xFF;
            x++;
            if (x == 8) {
                x = 0;
                lineOffset = (lineOffset + 1) & 31;
                tile = cpu->MEM[mapLocation + lineOffset];
                // Tile set 0 is numbered -128 to 128
                if (!tileSet) {
                    tile = ((int8) tile) + 256;
                }
            }
        }
    }
}

//uint8 nextPosition = 0;
// Load window into frameBuffer
static void loadWindowLine(uint8 scanLine, bool tileSet, cpu_state *cpu) {
    // Check if window is enabled
    if (readBit(5, &cpu->MEM[LCDC])) {
        int16 windowX = cpu->MEM[WINDOW_X] - 7;
        uint8 windowY = cpu->MEM[WINDOW_Y];
        //printf("Window enabled! RAW_X: %d X: %d Y: %d\n", cpu->MEM[WINDOW_X], windowX, windowY);
        // Skip line if the window is below
        if (scanLine < windowY) {
            return;
        }
        int mapLocation = (readBit(6, &cpu->MEM[LCDC])) ? 0x9C00 : 0x9800;
        // Draw tileset onto the window
        mapLocation += ((scanLine + cpu->MEM[WINDOW_Y]) >> 3) << 5;
        short lineOffset = cpu->MEM[WINDOW_X] >> 3;
        short x = (cpu->MEM[WINDOW_X] - 7) & 7;
        short y = (scanLine + cpu->MEM[WINDOW_Y]) & 7;
        //nextPosition++;
        short tile = cpu->MEM[mapLocation + lineOffset];
        // Tile set 0 is numbered -128 to 128
        if (!tileSet) {
            tile = ((int8) tile) + 256;
        }
        short drawOffset = DISPLAY_WIDTH * scanLine;
        for (int i = windowX; i < DISPLAY_WIDTH; i++) {
            if (i >= 0) {
                frameBuffer[(drawOffset + i)*4 + 0] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
                frameBuffer[(drawOffset + i)*4 + 1] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
                frameBuffer[(drawOffset + i)*4 + 2] = COLOURS[backgroundColourOffset[tiles[tile][x][y]]];
                frameBuffer[(drawOffset + i)*4 + 3] = 0xFF;
            }
            x++;
            if (x == 8) {
                x = 0;
                lineOffset = (lineOffset + 1) & 31;
                tile = cpu->MEM[mapLocation + lineOffset];
                // Tile set 0 is numbered -128 to 128
                if (!tileSet) {
                    tile = ((int8) tile) + 256;
                }
            }
        }
    }
}

static void loadSpriteLine(uint8 scanLine, cpu_state *cpu) {
    // Check if sprites enabled
    if (readBit(1, &cpu->MEM[LCDC])) {
        // Check if 8 or 16 pixels high
        bool sprite8x16 = (readBit(2, &cpu->MEM[LCDC]));
        // Iterate through all sprites data in OAM
        for (uint8 sprite = 0; sprite < 40; sprite++) {
            // 4 bytes per sprite data.
            // First byte is y, second is x, third is title number, fourth is attributes
            uint16 spriteOffset = 0xFE00 + (sprite * 4);

            // y coord. Offset by -16
            int16 spriteY = cpu->MEM[spriteOffset] - 16;
            // x coord. Offset by -8
            int16 spriteX = cpu->MEM[spriteOffset + 1] - 8;
            // Tile number
            uint8 tile = cpu->MEM[spriteOffset + 2];
            // Attributes
            uint8 attributes = cpu->MEM[spriteOffset + 3];

            // Get attributes
            bool flipX = readBit(5, &attributes);
            bool flipY = readBit(6, &attributes);
            bool priority = readBit(7, &attributes);
            bool palette = readBit(4, &attributes);

            // Set sprite height based on the LCDC bit
            uint8 height = (sprite8x16) ? 16 : 8;

            fprintf(stdout, "x: %d y: %d tile: %d attrubutes: %X height: %d scanline: %d\n", spriteX, spriteY, tile, attributes, height, scanLine);

            // Check if sprite is on this line number
            if ((scanLine >= spriteY) && (scanLine < (spriteY + height))) {
                short drawOffset = DISPLAY_WIDTH * scanLine;
                uint8 y = scanLine - spriteY;
                // Correct for 8x16 sprites. When not mirrored on y, move to next tile if y > 7. When mirrored move to next tile for y <= 7.
                // This corrects for mirroring the sprite in halfs. Eg 1234 becomes 4321 not 2143.
                if (sprite8x16 && ((flipY && y <= 7) || (!flipY && y > 7))) {
                    tile++;
                }
                y %= 8;
                //printf("x: %d y: %d \n", spriteX, y);
                // Iterate over the length of the title
                for (uint8 x = 0; x < 8; x++) {
                    // Skip pixel if off screen
                    if (x + spriteX < 0) {
                        continue;
                    }
                    // Switch tile for 8x16 sprite
                    uint8 drawX = (flipX) ? 7 - x : x;
                    uint8 drawY = (flipY) ? 7 - y : y;

                    uint8 colour = tiles[tile][drawX][drawY];
                    // Skip clear pixels
                    if (!colour) {
                        continue;
                    }
                    colour = (palette) ?  spritePaletteOne[tiles[tile][drawX][drawY]] : spritePaletteZero[tiles[tile][drawX][drawY]];
                    if (!priority || frameBuffer[(drawOffset + x + spriteX)*4 + 0] == 0xFF) {
                        frameBuffer[(drawOffset + x + spriteX)*4 + 0] = COLOURS[colour];
                        frameBuffer[(drawOffset + x + spriteX)*4 + 1] = COLOURS[colour];
                        frameBuffer[(drawOffset + x + spriteX)*4 + 2] = COLOURS[colour];
                        frameBuffer[(drawOffset + x + spriteX)*4 + 3] = 0xFF;
                    }
                }
            }
        }
    }
}

// Load scanline into the frame buffer
void loadScanline(cpu_state *cpu) {
    uint8 scanLine = cpu->MEM[SCANLINE];
    bool tileSet = readBit(4, &cpu->MEM[LCDC]);
    loadBackgroundLine(scanLine, tileSet, cpu);
    loadWindowLine(scanLine, tileSet, cpu);
    loadSpriteLine(scanLine, cpu);
}

// Draw framebuffer to screen
void draw(cpu_state *cpu) {
    //loadTiles(cpu);
    //for (uint8 i = 0; i < DISPLAY_HEIGHT; i++) {
        //loadScanline(i, cpu);
    //}
    displayOnWindow(frameBuffer);
}
