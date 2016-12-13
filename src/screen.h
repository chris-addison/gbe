#ifndef SCREEN_H
#define SCREEN_H

extern void updateScreen(cpu_state *cpu);

//screen constants
#define H_BLANK 0b00
#define V_BLANK 0b01
#define OAM     0b10
#define VRAM    0b11

#endif /* SCREEN_H */
