#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "cpu.h"

//memory locations
#define INTERRUPTS_ENABLED 0xFFFF
#define WINDOW_X 0xFF4B
#define WINDOW_Y 0xFF4A
#define SP_PALETTE_1 0xFF49
#define SP_PALETTE_0 0xFF48
#define BG_PALETTE 0xFF47
#define DMA 0xFF46
#define SYC 0xFF45
#define SCANLINE 0xFF44
#define SCROLL_X 0xFF43
#define SCROLL_Y 0xFF42
#define STAT 0xFF41
#define LCDC 0xFF40
/* WAVE PATTERNS 0xFF30 to 0xFF3F */
#define NR_52 0xFF26
#define NR_51 0xFF25
#define NR_50 0xFF24
#define NR_44 0xFF23
#define NR_43 0xFF22
#define NR_42 0xFF21
#define NR_41 0xFF20
#define NR_34 0xFF1E
#define NR_33 0xFF1D
#define NR_32 0xFF1C
#define NR_31 0xFF1B
#define NR_30 0xFF1A
#define NR_24 0xFF19
#define NR_23 0xFF18
#define NR_22 0xFF17
#define NR_21 0xFF16
#define NR_14 0xFF14
#define NR_13 0xFF13
#define NR_12 0xFF12
#define NR_11 0xFF11
#define NR_10 0xFF10
#define INTERRUPT_FLAGS 0xFF0F
#define TAC 0xFF07
#define TMA 0xFF06
#define TIMA 0xFF05
#define DIV 0xFF04
#define SC 0xFF02
#define SB 0xFF01
#define JOYPAD 0xFF00

extern uint8 readByte(uint16 address, cpu_state *cpu);
extern uint8 readNextByte(cpu_state *cpu);
extern void writeByte(uint16 address, uint8 value, cpu_state *cpu);
extern uint16 readShort(uint16 address, cpu_state *cpu);
extern void writeShort(uint16 address, uint16 value, cpu_state *cpu);
extern void writeShortToStack(uint16 value, cpu_state *cpu);
extern uint16 readShortFromStack(cpu_state *cpu);
extern void setFlag(uint8 flag, cpu_state *cpu);
extern void clearFlag(uint8 flag, cpu_state *cpu);
extern bool readFlag(uint8 flag, cpu_state *cpu);
extern bool readBit(uint8 bit, uint8 *reg);
extern bool readBitMem(uint8 bit, cpu_state *cpu);

#endif /* MEMORY_H */
