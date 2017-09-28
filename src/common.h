#ifndef COMMON_H
#define COMMON_H

#include "types.h"
#include "cpu.h"
#include <stdio.h>

extern void printByte(uint8 byte);
extern void printByteSigned(int8 byte);
extern void printShort(uint16 twoBytes);
extern void printInstruction(bool showPC, uint16 PC, Cpu *cpu);
extern void printInstructionToFile(uint16 PC, FILE *file, Cpu *cpu);

#endif /* COMMON_H */
