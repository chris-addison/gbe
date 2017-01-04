/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef COMMON_C
#define COMMON_C
#include "types.h"
#include "common.h"
#include "opcodes/opcodes.h"
#include "cpu.h"
#include "display.h"
#include "memory.h"
#include <stdio.h>


//print unsigned byte to standard input
void printByte(uint8 byte) {
    printf("0x%02" PRIX8, byte);
}

//print signed byte to standard input
void printByteSigned(int8 byte) {
    printf("0x%02" PRIi8, byte);
}

//print 2 bytes to standard input
void printShort(uint16 twoBytes) {
    printf("0x%04X", twoBytes);
}

//print given instruction
void printInstruction(bool showPC, uint16 PC, cpu_state *cpu) {
    if (showPC) {
        printf("0x%04X:  ", PC);
    }
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        printf("%s\n", cbOpcodes[readByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        printf("%s\n", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        printf(opcodes[opcode].name, readByte(PC + 1, cpu));
        printf("\n");
    } else { //print three byte instruction
        printf(opcodes[opcode].name, readShort(PC + 1, cpu));
        printf("\n");
    }
}

//print given instruction to file
void printInstructionToFile(uint16 PC, FILE *file, cpu_state *cpu) {
    //skip the wait instructions
	if (PC >= 0x36C && PC <= 0x36F) {
		return;
	}
    fprintf(file, "0x%04X:  ", PC);
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        fprintf(file, "%s\nAF:\t%X\tBC:\t%X\tDE:\t%X\tHL:\t%X\n", cbOpcodes[readByte(PC + 1, cpu)].name, cpu->registers.AF, cpu->registers.BC, cpu->registers.DE, cpu->registers.HL);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        fprintf(file, "%s\nAF:\t%X\tBC:\t%X\tDE:\t%X\tHL:\t%X\n", opcodes[opcode].name, cpu->registers.AF, cpu->registers.BC, cpu->registers.DE, cpu->registers.HL);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        fprintf(file, opcodes[opcode].name, readByte(PC + 1, cpu));
        fprintf(file, "\nAF:\t%X\tBC:\t%X\tDE:\t%X\tHL:\t%X\n", cpu->registers.AF, cpu->registers.BC, cpu->registers.DE, cpu->registers.HL);
    } else { //print three byte instruction
        fprintf(file, opcodes[opcode].name, readShort(PC + 1, cpu));
        fprintf(file, "\nAF:\t%X\tBC:\t%X\tDE:\t%X\tHL:\t%X\n", cpu->registers.AF, cpu->registers.BC, cpu->registers.DE, cpu->registers.HL);
    }
}

#endif /* COMMON_C */
