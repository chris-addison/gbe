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

//take a short and save as two bytes in an array
static void saveShort(uint8 * pointer, short value) {
    *pointer = value & 0xFF;
    *(pointer + 1) = (value >> 8) & 0xFF;
}

//
static uint16 getShort(uint8 * pointer) {
    uint16 value = (*(pointer + 1) << 8) + *pointer;
    return value;
}
