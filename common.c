/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef COMMON_C
#define COMMON_C
#include "main.h"

//read next byte from the instruction pointer
static uint8 readNextByte(struct cpu_state * cpu) {
    uint8 byte = *(cpu->MEM + cpu->PC);
    cpu->PC++;
    return byte;
}

//peek at byte at given memory address
static uint8 peekByte(uint16 pointer, cpu_state * cpu) {
    return cpu->MEM[pointer];
}

//peek at byte relative to current PC
static uint8 peekByteRelative(uint16 offset, cpu_state * cpu) {
    return peekByte(cpu->PC + offset, cpu);
}

//peek ant short at given memory address
static uint16 peekShort(uint16 pointer, cpu_state * cpu) {
	uint16 value = (peekByte(pointer + 1, cpu) << 8) + peekByte(pointer, cpu);
    return value;
}

//print given instruction instruction
static void printInstruction(uint16 PC, cpu_state * cpu) {
    uint8 opcode = peekByte(PC, cpu);
    //create case for 0xCB prefix instructions here
    if (opcode == 0xCB) {
        printf("%s", cbOpcodes[peekByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) {
        printf("%s", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) {
        printf(opcodes[opcode].name, peekByte(PC + 1, cpu));
    } else {
        printf(opcodes[opcode].name, peekShort(PC + 1, cpu));
    }
    printf("\n");
}

//print unsigned byte to standard input
static void printByteUnsigned(uint8 byte) {
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

//save a short as two bytes in little endian
static void saveShort(uint8 *arrayPointer, uint16 value) {
    *(arrayPointer) = value & 0xFF; //Least significant byte
    *(arrayPointer + 1) = (value >> 8) & 0xFF; //Most significant byte
}

//get a short from two uint8s in an array where the value is stored in little endian (eg. memory)
static uint16 getShort(uint8 *pointer) {
	uint16 value = (*(pointer + 1) << 8) + *(pointer);
    return value;
}

//move the stack pointer and save a short to the stack
static void saveShortToStack(uint16 value, cpu_state *cpu) {
    cpu->SP -= 2;
    saveShort(&(cpu->MEM[cpu->SP]), value);
}

static uint16 readShortFromStack(cpu_state *cpu) {
    uint16 value = getShort(&(cpu->MEM[cpu->SP]));
    cpu->SP += 2;
    return value;
}

//set the state of a flag
static void setFlag(uint8 flag, cpu_state *cpu) {
    /* Set Flag */
    //create a mask for the flag, then OR it to set the flag to one
    cpu->registers.F |= (0b1 << flag);
}

//clear the state of a flag
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
static bool readBit(uint8 bit, uint8 *store) {
    //get register, shift down to correct flag offset and compare
    return ((*store >> bit) & 0b1) == 0b1;
}

#endif /* MAIN_H */
