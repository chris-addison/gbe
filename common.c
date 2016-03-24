/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

//read next byte from the instruction pointer
static uint8 readNextByte(struct cpu_state * cpu) {
    uint8 byte = *(cpu->MEM + cpu->PC);
    cpu->PC++;
    return byte;
}

//print unsigned byte to standard input
static void printByteUnsigned(uint8 byte) {
    printf("0x%02" PRIX8 "\n", byte);
}

//print signed byte to standard input
static void printByteSigned(int8 byte) {
    printf("%" PRIi8 "\n", byte);
}

//print 2 bytes to standard input
static void printShort(short twoBytes) {
    printf("0x%04x\n", twoBytes);
}

//save a short as two bytes in little endian
static void saveShortLittleEndian(uint8 * arrayPointer, short value) {
    *(arrayPointer) = value & 0xFF; //Least significant byte
    *(arrayPointer + 1) = (value >> 8) & 0xFF; //Most significant byte
}

//save a short as two bytes in big endian
static void saveShortBigEndian(uint8 * arrayPointer, short value) {
	*(arrayPointer + 1) = value & 0xFF; //Least significant byte
    *(arrayPointer) = (value >> 8) & 0xFF; //Most significant byte
}

//get a short from two uint8s in an array where the value is stored in little endian (eg. memory)
static uint16 getShortLittleEndian(uint8 * pointer) {
	uint16 value = (*(pointer + 1) << 8) + *(pointer);
    return value;
}

//get a short from two uint8s in an array where the value is stored in big endian (eg. registers)
static uint16 getShortBigEndian(uint8 * pointer) {
	uint16 value = (*(pointer) << 8) + *(pointer + 1);
	return value;
}

//set or reset the state of a cpu flag
static void setFlag(bool set, uint8 flag, cpu_state * cpu) {
    if (set) {
        /* Set Flag */
        //create a mask for the flag, then OR it to set the flag to one
        cpu->REG[F] |= (0b1 << flag);
    } else {
        /* Reset Flag */
        //create a mask for the flag, inverse it, then AND it to set the flag to zero
        cpu->REG[F] &= ~(0b1 << flag);
    }
}

//return true if a flag is set. Return false otherwise.
static bool readFlag(uint8 flag, cpu_state * cpu) {
    //get flags register, shift down to correct flag offset
    return ((cpu->REG[F] >> flag) & 0b1) == 0b1;
}
