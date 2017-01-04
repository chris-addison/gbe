#ifndef SCREEN_H
#define SCREEN_H

#include "cpu.h"

extern void updateScreen(cpu_state *cpu);

// Screen constants
#define H_BLANK             0b000
#define V_BLANK             0b001
#define OAM                 0b010
#define VRAM                0b011
#define CONINCIDENCE_FLAG   0b100

// Screen interrupt flags
#define SCREEN_INTER_H_BLANK    0b1000
#define SCREEN_INTER_V_BLANK    0b10000
#define SCREEN_INTER_OAM        0b100000
#define SCREEN_INTER_LYC        0b1000000

#endif /* SCREEN_H */
