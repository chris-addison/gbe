#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"
#include "cpu.h"

extern bool updateIME(Cpu *cpu);
extern void setInterruptFlag(uint8 flag, Cpu *cpu);
extern void clearInterruptFlag(uint8 flag, Cpu *cpu);
extern uint8 availableInterrupts(Cpu *cpu);
extern void handleInterrupts(Cpu *cpu, bool active_ime, uint8 interrupts);

//interrupt bit offsets
#define INTR_V_BLANK 0b1
#define INTR_STAT 0b10
#define INTR_TIMER 0b100
#define INTR_SERIAL 0b1000
#define INTR_JOYPAD 0b10000

#endif /* INTERRUPTS_H */
