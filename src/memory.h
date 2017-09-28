#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "cpu.h"

extern uint8 readByte(uint16 address, Cpu *cpu);
extern uint8 readNextByte(Cpu *cpu);
extern void writeByte(uint16 address, uint8 value, Cpu *cpu);
extern uint16 readShort(uint16 address, Cpu *cpu);
extern void writeShort(uint16 address, uint16 value, Cpu *cpu);
extern void writeShortToStack(uint16 value, Cpu *cpu);
extern uint16 readShortFromStack(Cpu *cpu);
extern void setFlag(uint8 flag, Cpu *cpu);
extern void clearFlag(uint8 flag, Cpu *cpu);
extern bool readFlag(uint8 flag, Cpu *cpu);
extern bool readBit(uint8 bit, uint8 *reg);
extern bool readBitMem(uint8 bit, Cpu *cpu);

#endif /* MEMORY_H */
