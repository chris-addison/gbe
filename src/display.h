#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "cpu.h"

#define DISPLAY_HEIGHT 144
#define DISPLAY_WIDTH 160

extern void updateBackgroundColour(uint8 value);
extern void updateSpritePalette(uint8 palette, uint8 value);
extern void resetWindowLine();
extern void loadTiles(Cpu *cpu);
extern void loadScanline(Cpu *cpu);
extern void draw(Cpu *cpu);

#endif /* DISPLAY_H */
