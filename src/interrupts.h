#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"
#include "cpu.h"

extern bool updateIME(cpu_state *cpu);
extern void setInterruptFlag(uint8 flag, cpu_state *cpu);
extern void clearInterruptFlag(uint8 flag, cpu_state *cpu);
extern uint8 availableInterrupts(cpu_state *cpu);
extern void handleInterrupts(cpu_state *cpu, bool active_ime, uint8 interrupts);

//interrupt bit offsets
#define INTR_V_BLANK 0b1
#define INTR_STAT 0b10
#define INTR_TIMER 0b100
#define INTR_SERIAL 0b1000
#define INTR_JOYPAD 0b10000

#endif /* INTERRUPTS_H */
