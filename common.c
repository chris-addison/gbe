/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef COMMON_C
#define COMMON_C
#include "main.h"

//read a byte from a given memory address
static uint8 readByte(uint16 address, cpu_state *cpu) {
    return cpu->MEM[address];
}

//read next byte from the instruction pointer
static uint8 readNextByte(cpu_state *cpu) {
    uint8 byte = readByte(cpu->PC, cpu);
    cpu->PC++;
    return byte;
}

//write a byte to the given memory address
static void writeByte(uint16 address, uint8 value, cpu_state *cpu) {
    //handle the echo of internal memory at 0xC000->0xDDFF and 0xE000->0xFDFF
    if (address >= 0xC000 && address < 0xDE00) {
        cpu->MEM[address + 0x2000] = value;
    } else if (address >= 0xE000 && address < 0xFE00) {
        cpu->MEM[address - 0x2000] = value;
    }
    cpu->MEM[address] = value;
}

//read a short from a given memory address
static uint16 readShort(uint16 address, cpu_state *cpu) {
    return (((uint16) readByte(address + 1, cpu)) << 8) + readByte(address, cpu);
}

//write a short to the given memory address
static void writeShort(uint16 address, uint16 value, cpu_state *cpu) {
    writeByte(address, value & 0xFF, cpu); //Least significant byte
    writeByte(address + 1, (value >> 8) & 0xFF, cpu); //Most significant byte
}

//move the stack pointer and save a short to the stack
static void writeShortToStack(uint16 value, cpu_state *cpu) {
    cpu->SP -= 2;
    writeShort(cpu->SP, value, cpu);
}

//read a short from the stack and move the stack pointer
static uint16 readShortFromStack(cpu_state *cpu) {
    uint16 value = readShort(cpu->SP, cpu);
    cpu->SP += 2;
    return value;
}

//set a flag
static void setFlag(uint8 flag, cpu_state *cpu) {
    //create a mask for the flag, then OR it to set the flag to one
    cpu->registers.F |= (0b1 << flag);
}

//clear a flag
static void clearFlag(uint8 flag, cpu_state *cpu) {
    /* Reset Flag */
    //create a mask for the flag, inverse it, then AND it to set the flag to zero
    cpu->registers.F &= ~(0b1 << flag);
}

//return true if a flag is set. Return false otherwise.
static bool readFlag(uint8 flag, cpu_state *cpu) {
    //get flags register, shift down to correct flag offset and compare
    return ((cpu->registers.F >> flag) & 0b1) == 0b1;
}

//return true if a bit in a register is set. Return false otherwise.
static bool readBit(uint8 bit, uint8 *reg) {
    //get register, shift down to correct flag offset and compare
    return ((*reg >> bit) & 0b1) == 0b1;
}

//return true if a bit in a byte at the memory address stored in HL is set. Return false otherwise.
static bool readBitMem(uint8 bit, cpu_state *cpu) {
    //get register, shift down to correct flag offset and compare
    return ((readByte(cpu->registers.HL, cpu) >> bit) & 0b1) == 0b1;
}

//print unsigned byte to standard input
static void printByte(uint8 byte) {
    printf("0x%02" PRIX8, byte);
}

//print signed byte to standard input
static void printByteSigned(int8 byte) {
    printf("0x%02" PRIi8, byte);
}

//print 2 bytes to standard input
static void printShort(uint16 twoBytes) {
    printf("0x%04X", twoBytes);
}

//print given instruction
static void printInstruction(bool showPC, uint16 PC, cpu_state *cpu) {
    if (showPC) {
        fprintf(stderr, "0x%04X:  ", PC);
    }
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        fprintf(stderr, "%s\n", cbOpcodes[readByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        fprintf(stderr, "%s\n", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        fprintf(stderr, opcodes[opcode].name, readByte(PC + 1, cpu));
        fprintf(stderr, "\n");
    } else { //print three byte instruction
        fprintf(stderr, opcodes[opcode].name, readShort(PC + 1, cpu));
        fprintf(stderr, "\n");
    }
}

//print given instruction to file
static void printInstructionToFile(uint16 PC, FILE *file, cpu_state *cpu) {
    //skip the wait instructions
	if (PC >= 0x36C && PC <= 0x36F) {
		return;
	}
    fprintf(file, "0x%04X:  ", PC);
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        fprintf(file, "%s\n", cbOpcodes[readByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        fprintf(file, "%s\n", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        fprintf(file, opcodes[opcode].name, readByte(PC + 1, cpu));
        fprintf(file, "\n");
    } else { //print three byte instruction
        fprintf(file, opcodes[opcode].name, readShort(PC + 1, cpu));
        fprintf(file, "\n");
    }
}

#endif /* COMMON_C */
