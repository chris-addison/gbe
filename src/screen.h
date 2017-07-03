#ifndef SCREEN_H
#define SCREEN_H

#include "cpu.h"

extern void updateScreen(cpu_state *cpu);

// Screen constants
#define SCREEN_HEIGHT       144
#define SCREEN_WIDTH        160
#define SCREEN_LINES        154

// LCDC modes
#define H_BLANK             0b000
#define V_BLANK             0b001
#define OAM                 0b010
#define VRAM                0b011
#define STAT_MODE_MASK      0b011
#define CONINCIDENCE_FLAG   0b100

// Screen interrupt flags
#define STAT_INTR_H_BLANK   0b1000
#define STAT_INTR_V_BLANK   0b10000
#define STAT_INTR_OAM       0b100000
#define STAT_INTR_LYC       0b1000000

// LCDC mode lengths in cycles
#define LCDC_H_BLANK_CYCLES 204
#define LCDC_OAM_CYCLES     80
#define LCDC_VRAM_CYCLES    172
#define LCDC_LINE_CYCLES    456
#define LCDC_SCREEN_CYCLES  70224

#endif /* SCREEN_H */
