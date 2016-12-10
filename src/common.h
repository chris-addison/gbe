#ifndef COMMON_H
#define COMMON_H

#include "types.h"
#include "cpu.h"
#include <stdio.h>

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
extern void printByte(uint8 byte);
extern void printByteSigned(int8 byte);
extern void printShort(uint16 twoBytes);
extern void printInstruction(bool showPC, uint16 PC, cpu_state *cpu);
extern void printInstructionToFile(uint16 PC, FILE *file, cpu_state *cpu);

#endif /* COMMON_H */
