#include "types.h"
#include "memory.h"
#include "display.h"
#include "window.h"
#include "stdio.h"

const uint8 COLOURS[] = {0xFF, 0xC0, 0x60, 0x00};
uint8 background_colour_offset[] = {0, 1, 2, 3};
uint8 sprite_palette_zero[] = {0, 1, 2, 3};
uint8 sprite_palette_one[] = {0, 1, 2, 3};

// Store the un-offset colour of the current scanline.
// Used to decided whether sprites will draw if they don't have priority
// Without this, sprites will be invisible on some games
static uint8 line_buffer[DISPLAY_WIDTH];
static uint8 frame_buffer[4 * DISPLAY_WIDTH * DISPLAY_HEIGHT];
static uint8 tiles[384][8][8];

// Update colour palette for the background
void updateBackgroundColour(uint8 value) {
    for (uint8 i = 0; i < 4; i++) {
        // Mask the two bits to get the background colour set
        background_colour_offset[i] = (value >> (i * 2)) & 0b11;
    }
}

// Update colour palette for the sprites
void updateSpritePalette(uint8 palette, uint8 value) {
    for (uint8 i = 0; i < 4; i++) {
        // Mask the two bits to get the background colour set
        if (palette) { // palette 1
            sprite_palette_one[i] = (value >> (i * 2)) & 0b11;
        } else { // pallete 0
            sprite_palette_zero[i] = (value >> (i * 2)) & 0b11;
        }
    }
}

// Load all tiles. 384 in total as set 0 overlaps set 1 by 128 tiles.
void loadTiles(cpu_state *cpu) {
    uint8 *vram = cpu->MEM + 0x8000;
    for (int tile_num = 0; tile_num < 384; tile_num++) {
        for (int y = 0; y < 8; y++) {
            uint8 byte_low = *(vram + 2*y + tile_num*16);
            uint8 byte_high = *(vram + 2*y + 1 + tile_num*16);
            for (int x = 0; x < 8; x++) {
                tiles[tile_num][x][y] = ((byte_low & 0x80) >> 7) | ((byte_high & 0x80) >> 6);
                byte_low <<= 1;
                byte_high <<= 1;
            }
        }
    }
}

// Load Background into frame buffer
static void loadBackgroundLine(uint8 scanline, bool tile_set, cpu_state *cpu) {
    // Check if background enabled
    if (readBit(0, &cpu->MEM[LCDC])) {
        // Select map location
        uint16 map_location = (readBit(3, &cpu->MEM[LCDC])) ? 0x9C00 : 0x9800;
        // Draw tileset onto the window
        // Wrap the y offset at 32 to wrap the background vertically. Map is 32x32. Same for offset_x below.
        uint16 offset_y = (((scanline + cpu->MEM[SCROLL_Y]) >> 3) % 32) << 5;
        uint16 offset_x = (cpu->MEM[SCROLL_X] >> 3) % 32;
        short x = cpu->MEM[SCROLL_X] % 8;
        short y = (scanline + cpu->MEM[SCROLL_Y]) % 8;
        uint16 tile = cpu->MEM[map_location + offset_x + offset_y];
        // Tile set 0 is numbered -128 to 128. In tiles array it takes index 128 to 383.
        if (!tile_set) {
            tile = ((int8) tile) + 256;
        }
        // Pixel offset for the current scanline
        uint16 draw_offset = DISPLAY_WIDTH * scanline;
        // Put data in frame buffer
        for (uint16 i = 0; i < DISPLAY_WIDTH; i++) {
            uint8 colour = tiles[tile][x][y];
            frame_buffer[(draw_offset + i)*4 + 0] = COLOURS[background_colour_offset[colour]];
            frame_buffer[(draw_offset + i)*4 + 1] = COLOURS[background_colour_offset[colour]];
            frame_buffer[(draw_offset + i)*4 + 2] = COLOURS[background_colour_offset[colour]];
            frame_buffer[(draw_offset + i)*4 + 3] = 0xFF;
            line_buffer[i] = colour;
            x++;
            // Tile completed, draw next one.
            if (x == 8) {
                x = 0;
                offset_x = (offset_x + 1) % 32; // Wrap the x offset at 32 to wrap background horizonally. See above.
                tile = cpu->MEM[map_location + offset_x + offset_y];
                // Tile set 0 is numbered -128 to 128. In tiles array it takes index 128 to 383.
                if (!tile_set) {
                    tile = ((int8) tile) + 256;
                }
            }
        }
    }
}

// Clear the current line
void loadClearLine(uint8 scanline, cpu_state *cpu) {
    short draw_offset = DISPLAY_WIDTH * scanline;
    for (uint16 i = 0; i < DISPLAY_WIDTH; i++) {
        frame_buffer[(draw_offset + i)*4 + 0] = 0xFF;
        frame_buffer[(draw_offset + i)*4 + 1] = 0xFF;
        frame_buffer[(draw_offset + i)*4 + 2] = 0xFF;
        frame_buffer[(draw_offset + i)*4 + 3] = 0xFF;
        line_buffer[i] = 0;
    }
}

// Set window line to 0.
void resetWindowLine(cpu_state *cpu) {
    //printf("reset line!\n");
    cpu->window_line = 0;
}

// Load window into frame buffer
static void loadWindowLine(uint8 scanline, bool tile_set, cpu_state *cpu) {
    // Check if window is enabled
    if (readBit(5, &cpu->MEM[LCDC])) {
        int16 window_x = cpu->MEM[WINDOW_X] - 7;
        uint8 window_y = cpu->MEM[WINDOW_Y];
        // Skip line if the window is below, or off screen
        if (scanline < window_y || window_x > 159 || window_y > 143) {
            return;
        }
        uint16 map_location = (readBit(6, &cpu->MEM[LCDC])) ? 0x9C00 : 0x9800;
        // Draw tileset onto the window
        uint16 offset_x = window_x >> 3;
        uint16 offset_y = (cpu->window_line >> 3) << 5;
        short x = 0;
        short y = cpu->window_line % 8;
        uint16 tile = cpu->MEM[map_location + offset_x + offset_y];
        // Tile set 0 is numbered -128 to 128. In tiles array it takes index 128 to 383.
        if (!tile_set) {
            tile = ((int8) tile) + 256;
        }
        short draw_offset = DISPLAY_WIDTH * scanline;
        for (int16 i = window_x; i < DISPLAY_WIDTH; i++) {
            if (i >= 0) {
                uint8 colour = tiles[tile][x][y];
                frame_buffer[(draw_offset + i)*4 + 0] = COLOURS[background_colour_offset[colour]];
                frame_buffer[(draw_offset + i)*4 + 1] = COLOURS[background_colour_offset[colour]];
                frame_buffer[(draw_offset + i)*4 + 2] = COLOURS[background_colour_offset[colour]];
                frame_buffer[(draw_offset + i)*4 + 3] = 0xFF;
                line_buffer[i] = colour;
            }
            x++;
            if (x == 8) {
                x = 0;
                offset_x += 1;
                tile = cpu->MEM[map_location + offset_x + offset_y];
                // Tile set 0 is numbered -128 to 128. In tiles array it takes index 128 to 383.
                if (!tile_set) {
                    tile = ((int8) tile) + 256;
                }
            }
        }
        cpu->window_line++;
    }
}

// Add spites onto the current scanline in frame buffer
static void loadSpriteLine(uint8 scanline, cpu_state *cpu) {
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

            //fprintf(stdout, "x: 0x%X y: 0x%X tile: 0x%X attrubutes: 0x%X\n", spriteX, spriteY, tile, attributes);
            //fprintf(stdout, "C: 0x%X\n", cpu->registers.C);

            // Check if sprite is on this line number
            if ((scanline >= spriteY) && (scanline < (spriteY + height))) {
                short draw_offset = DISPLAY_WIDTH * scanline;
                uint8 y = scanline - spriteY;
                // Correct for 8x16 sprites. When not mirrored on y, move to next tile if y > 7. When mirrored move to next tile for y <= 7.
                // This corrects for mirroring the sprite in halfs. Eg 1234 becomes 4321 not 2143.
                if (sprite8x16 && ((flipY && y <= 7) || (!flipY && y > 7))) {
                    tile++;
                }
                y %= 8;
                //printf("x: %d y: %d \n", spriteX, y);
                // Iterate over the length of the title
                for (uint8 x = 0; x < 8; x++) {
                    //printf("POS: %d x: %d sx: %d\n", x + spriteX, x, spriteX);
                    // Skip pixel if off screen
                    if (x + spriteX < 0 || x + spriteX > 159) {
                        //printf("POS: %d x: %d sx: %d\n", x + spriteX, x, spriteX);
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
                    colour = (palette) ?  sprite_palette_one[tiles[tile][drawX][drawY]] : sprite_palette_zero[tiles[tile][drawX][drawY]];
                    if (!priority || !line_buffer[x + spriteX]) {
                        frame_buffer[(draw_offset + x + spriteX)*4 + 0] = COLOURS[colour];
                        frame_buffer[(draw_offset + x + spriteX)*4 + 1] = COLOURS[colour];
                        frame_buffer[(draw_offset + x + spriteX)*4 + 2] = COLOURS[colour];
                        frame_buffer[(draw_offset + x + spriteX)*4 + 3] = 0xFF;
                    }
                }
            } else {
                //fprintf(stdout, "x: 0x%X y: 0x%X tile: 0x%X attrubutes: 0x%X\n", spriteX, spriteY, tile, attributes);
            }
        }
    }
}

// Load scanline into the frame buffer
void loadScanline(cpu_state *cpu) {
    // Load info to display
    uint8 scanline = cpu->MEM[SCANLINE];
    bool tile_set = readBit(4, &cpu->MEM[LCDC]);

    // If display enabled, draw to screen
    // Else clear the line
    if (readBit(7, &cpu->MEM[LCDC])) {
        //printf("Line: %d\n", cpu->window_line);
        loadTiles(cpu);
        // Display each line in depth order
        loadBackgroundLine(scanline, tile_set, cpu);
        loadWindowLine(scanline, tile_set, cpu);
        loadSpriteLine(scanline, cpu);
    } else {
        // Set the line white
        loadClearLine(scanline, cpu);
    }
}

// Draw frame buffer to screen
void draw(cpu_state *cpu) {
    //loadTiles(cpu);
    //for (uint8 i = 0; i < DISPLAY_HEIGHT; i++) {
        //loadScanline(i, cpu);
    //}
    displayOnWindow(frame_buffer);
}
