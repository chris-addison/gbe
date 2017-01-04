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
#define INTERRUPT_FLAGS 0xFF0F
#define TAC 0xFF07
#define TMA 0xFF06
#define TIMA 0xFF05
#define DIV 0xFF04
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
