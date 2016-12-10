#ifndef OPCODE_H
#define OPCODE_H
#include "../types.h"
#include "../cpu.h"

extern int execute(cpu_state * cpu);

//structure to store list of opcodes and associated info
struct opcode {
	char* name;
	uint8 cycles;
    uint8 cyclesMax;
    uint8 bytes;
} extern const opcodes[256];

//structure to store list of CB-prefix opcodes
struct cbOpcode {
    char* name;
    uint8 cycles;
} extern const cbOpcodes[256];

#endif /* OPCODE_H */
