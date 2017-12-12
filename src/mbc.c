#include <stdlib.h>
#include <stdio.h>
#include "mbc.h"
#include "types.h"
#include "cpu.h"

static uint8 readBasic(uint16 address, Cpu *cpu);
static void writeNone(uint16 address, uint8 value, Cpu *cpu);
static void writeMBC1(uint16 address, uint8 value, Cpu *cpu);
//static uint8 readMBC2(uint16 address, Cpu *cpu);
//static void writeMBC2(uint16 address, uint8 value, Cpu *cpu);
//static uint8 readMBC3(uint16 address, Cpu *cpu);
//static void writeMBC3(uint16 address, uint8 value, Cpu *cpu);
//static uint8 readMBC5(uint16 address, Cpu *cpu);
//static void writeMBC5(uint16 address, uint8 value, Cpu *cpu);

static void switchBank(uint16 bank, Cpu *cpu);

// Setup read/write callbacks to the correct function to handle the given mbc number
void setupMBCCallbacks(Cpu *cpu) {
    cpu->readMBC = readBasic;
    switch (cpu->mbc) {
        case 0:
            cpu->writeMBC = writeNone;
            break;
        case 1:
            cpu->writeMBC = writeMBC1;
            break;
        default:
            printf("MBC2 or higher not yet supported");
            exit(346);
    }
}

// Default MBC read
static uint8 readBasic(uint16 address, Cpu *cpu) {
    uint16 offset = address - EXTERNAL_RAM_BASE;
    if (offset > EXTERNAL_RAM_BOUND) {
        printf("Error: provided invalid read address for external ram!\n");
        return 0xFF;
    }
    if (cpu->RAM_enable) {
        return cpu->memory.ramBank[offset];
    }
    return 0xFF;
}

// Write to MBC when there isn't one
static void writeNone(uint16 address, uint8 value, Cpu *cpu) {
    printf("Error: writing to MBC when there isn't one!\n");
}

static void writeMBC1(uint16 address, uint8 value, Cpu *cpu) {
    if (address >= VRAM_BASE) {
        printf("Invalid address for MBC write: %X\n", address);
        return;
    }

    if (address < 0x2000) {
        cpu->RAM_enable = (value == 0x0A);
    } else if (address < 0x4000) {
        uint8 bank = value & 0x1F;
        if (!bank) bank = 0x01;
        cpu->currentRomBank = bank;
        // TODO: map bank
    } else {

    }
}