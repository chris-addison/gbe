#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "cpu.h"

#define DISPLAY_HEIGHT 144
#define DISPLAY_WIDTH 160

extern void updateBackgroundColour(uint8 value);
extern void updateSpritePalette(uint8 palette, uint8 value);
extern void resetWindowLine(cpu_state *cpu);
extern void loadTiles(cpu_state *cpu);
extern void loadScanline(cpu_state *cpu);
extern void draw(cpu_state *cpu);

#endif /* DISPLAY_H */
