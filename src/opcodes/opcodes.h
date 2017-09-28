#ifndef OPCODE_H
#define OPCODE_H
#include "../types.h"
#include "../cpu.h"

typedef struct {
	char* name;
	uint8 cycles;
    uint8 cyclesMax;
    uint8 bytes;
} opcode;

typedef struct {
    char* name;
    uint8 cycles;
} cb_opcode;

extern opcode get_opcode(uint8 value);
extern cb_opcode get_cb_opcode(uint8 value);

extern int executeNextInstruction(Cpu * cpu);

#endif /* OPCODE_H */
