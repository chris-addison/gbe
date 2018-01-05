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
static void writeMBC3(uint16 address, uint8 value, Cpu *cpu);
//static uint8 readMBC5(uint16 address, Cpu *cpu);
static void writeMBC5(uint16 address, uint8 value, Cpu *cpu);

static void switchRomBank(uint16 bank, Cpu *cpu) {
    if (bank >= cpu->maxRomBank) {
        printf("Out of bounds ROM bank: %d", bank);
        return;
    }
    cpu->currentRomBank = bank;
    cpu->memory.romBank = cpu->memory.rom + (ROM_BANK_SIZE * bank);
}

static void switchRamBank(uint8 bank, Cpu *cpu) {
    if (bank >= cpu->maxRamBank) {
        printf("Out of bounds RAM bank: %d", bank);
        return;
    }
    cpu->currentRamBank = bank;
    cpu->memory.ramBank = cpu->memory.ram + (RAM_BANK_SIZE * bank);
}

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
        case 3:
            cpu->writeMBC = writeMBC3;
            break;
        case 5:
            cpu->writeMBC = writeMBC5;
            break;
        default:
            printf("MBC 2, 6, 7 not yet supported\n");
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

// Handles writes to a mbc 1. Expects addresses between 0x0 and 0x7FFF
static void writeMBC1(uint16 address, uint8 value, Cpu *cpu) {
    if (address >= VRAM_BASE) {
        printf("Invalid address for MBC 1 write: %X\n", address);
        return;
    }

    if (address < 0x2000) {
        cpu->RAM_enable = (value == 0x0A);
    } else if (address < 0x4000) {
        uint8 bank = value & 0x1F;
        if (!bank) bank = 0x01;
        switchRomBank(bank, cpu);
    } else if (address < 0x6000) {
        uint8 upperTwoBits = (value & 0x3);
        if (cpu->mbc1Mode) {
            switchRamBank(upperTwoBits, cpu);
        } else {
            uint8 bank = cpu->currentRomBank | (cpu->currentRomBank & 0x1F);
            switchRomBank(bank, cpu);
        }
    } else { // < 0x8000
        cpu->mbc1Mode = value & 0x01;
        if (cpu->mbc1Mode) {
            switchRomBank(cpu->currentRomBank & 0x1F, cpu);
        } else {
            switchRamBank(0, cpu);
        }
    }
}

// Handles writes to a mbc 3. Expects addresses between 0x0 and 0x7FFF
static void writeMBC3(uint16 address, uint8 value, Cpu *cpu) {
    if (address >= VRAM_BASE) {
        printf("Invalid address for MBC 3 write: %X\n", address);
        return;
    }

    if (address < 0x2000) {
        cpu->RAM_enable = (value == 0x0A);
    } else if (address < 0x4000) {
        uint8 bank = value & 0x7F;
        if (!bank) bank = 0x01;
        switchRomBank(bank, cpu);
    } else if (address < 0x6000) {
        switchRamBank(value, cpu);
    } else { // < 0x8000
        // TODO: Latch RTC
        printf("RTC latch\n");
    }
}

// Handles writes to a mbc 5. Expects addresses between 0x0 and 0x5FFF
static void writeMBC5(uint16 address, uint8 value, Cpu *cpu) {
    if (address >= VRAM_BASE) {
        printf("Invalid address for MBC write: %X\n", address);
        return;
    }

    if (address < 0x2000) {
        cpu->RAM_enable = (value == 0x0A);
    } else if (address < 0x3000) {
        uint16 bank = (cpu->currentRomBank & 0x100) | value;
        switchRomBank(bank, cpu);
    } else if (address < 0x4000) {
        uint16 bank = (cpu->currentRomBank & 0xFF) | ((value & 0x01) << 8);
        switchRomBank(bank, cpu);
    } else if (address < 0x6000) {
        // TODO: rumble and rtc
        switchRamBank(value, cpu);
    } else {
        printf("Invalid address (>= 0x6000) for MBC 5 write: %X\n", address);
    }
}