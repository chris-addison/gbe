#ifndef SCREEN_H
#define SCREEN_H

extern void updateScreen(cpu_state *cpu);

//screen constants
#define H_BLANK 0x0
#define V_BLANK 0x1
#define OAM 0x2
#define VRAM 0x3

#endif /* SCREEN_H */
