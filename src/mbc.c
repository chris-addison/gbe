#include <stdlib.h>
#include <stdio.h>
#include "mbc.h"
#include "types.h"
#include "cpu.h"

static uint8 readNone(uint16 address, Cpu *cpu);
static void writeNone(uint16 address, uint8 value, Cpu *cpu);
static uint8 readMBC1(uint16 address, Cpu *cpu);
static void writeMBC1(uint16 address, uint8 value, Cpu *cpu);
static uint8 readMBC2(uint16 address, Cpu *cpu);
static void writeMBC2(uint16 address, uint8 value, Cpu *cpu);
static uint8 readMBC3(uint16 address, Cpu *cpu);
static void writeMBC3(uint16 address, uint8 value, Cpu *cpu);
static uint8 readMBC5(uint16 address, Cpu *cpu);
static void writeMBC5(uint16 address, uint8 value, Cpu *cpu);


void setupMBCCallbacks(Cpu *cpu) {
    switch (cpu->mbc) {
        case 0:
            cpu->readMBC = readNone;
            cpu->writeMBC = writeNone;
            break;
        case 1:
            cpu->readMBC = readMBC1;
            cpu->writeMBC = writeMBC1;
            break;
        default:
            printf("MBC2 or higher not yet supported");
            exit(346);
    }
}

static uint8 readNone(uint16 address, Cpu *cpu) {

    return 0;
}