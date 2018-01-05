/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifdef DEBUG
    #include "debug/debug.h"
#endif
#include "types.h"
#include "opcodes/opcodes.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

// Initialise the cpu
static void initCPU(Cpu *cpu) {
    // Malloc memory segments
    cpu->memory.wram = (uint8 *) malloc(8 * WRAM_BANK_SIZE * sizeof(uint8));
    cpu->memory.vram = (uint8 *) malloc(2 * VRAM_BANK_SIZE * sizeof(uint8));
    if (!cpu->memory.wram || !cpu->memory.vram) {
        printf("Failed to malloc vram or wram\n");
        exit(523);
    }
    cpu->memory.wramBank = cpu->memory.wram + WRAM_BANK_SIZE;
    cpu->memory.vramBank = cpu->memory.vram;

    // Rom and ram is handled when reading the cartridge
    cpu->memory.rom = cpu->memory.romBank = NULL;
    cpu->memory.ram = cpu->memory.ramBank = NULL;

    // Setup the PC and SP
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;
    cpu->wait = 0;

    // Setup mbc stuff
    cpu->currentRomBank = 1;
    cpu->maxRomBank = 1;
    cpu->currentRamBank = 0;
    cpu->maxRamBank = 0;
    cpu->RAM_enable = false;
    cpu->readMBC = NULL;
    cpu->writeMBC = NULL;
    cpu->mbc1Mode = false;

    // Setup interrupts
    cpu->ime = false;
    cpu->ime_enable = false;
    cpu->halt = false;
    cpu->halt_bug = false;

    // Setup startup values of registers
    cpu->registers.AF = 0x01B0;
    cpu->registers.BC = 0x0013;
    cpu->registers.DE = 0x00D8;
    cpu->registers.HL = 0x014D;

    // Setup startup memory values - excludes values set to 0
    cpu->memory.io[DIV - IO_BASE] = 0x00; // DIV
    cpu->memory.io[TIMA - IO_BASE] = 0x00; // TIMA
    cpu->memory.io[TMA - IO_BASE] = 0x00; // TMA
    cpu->memory.io[TAC - IO_BASE] = 0xF8; // TAC
    cpu->memory.io[INTERRUPT_FLAGS - IO_BASE] = 0xE1; // INTERRUPT FLAGS
    cpu->memory.io[NR_10 - IO_BASE] = 0x80;
    cpu->memory.io[NR_11 - IO_BASE] = 0xBF;
    cpu->memory.io[NR_12 - IO_BASE] = 0xF3;
    cpu->memory.io[NR_14 - IO_BASE] = 0xBF;
    cpu->memory.io[NR_21 - IO_BASE] = 0x3F;
    cpu->memory.io[NR_24 - IO_BASE] = 0xBF;
    cpu->memory.io[NR_30 - IO_BASE] = 0x7F;
    cpu->memory.io[NR_31 - IO_BASE] = 0xFF;
    cpu->memory.io[NR_32 - IO_BASE] = 0x9F;
    cpu->memory.io[NR_34 - IO_BASE] = 0xBF;
    cpu->memory.io[NR_41 - IO_BASE] = 0xFF;
    cpu->memory.io[NR_44 - IO_BASE] = 0xBF;
    cpu->memory.io[NR_50 - IO_BASE] = 0x77;
    cpu->memory.io[NR_51 - IO_BASE] = 0xF3;
    cpu->memory.io[NR_52 - IO_BASE] = 0xF1;
    cpu->memory.io[LCDC - IO_BASE] = 0x91; // LCDC
    cpu->memory.io[STAT - IO_BASE] = 0x85; // STAT
    cpu->memory.io[SCROLL_Y - IO_BASE] = 0x00; // SCY
    cpu->memory.io[SCROLL_X - IO_BASE] = 0x00; // SCX
    cpu->memory.io[SCANLINE - IO_BASE] = 0x00; // LY
    cpu->memory.io[SYC - IO_BASE] = 0x00; // LYC
    cpu->memory.io[DMA - IO_BASE] = 0xFF; // DMA
    cpu->memory.io[BG_PALETTE - IO_BASE] = 0xFC; // BGP
    cpu->memory.io[SP_PALETTE_0 - IO_BASE] = 0xFF;
    cpu->memory.io[SP_PALETTE_1 - IO_BASE] = 0xFF;
    cpu->memory.io[WINDOW_Y - IO_BASE] = 0x00; // WY
    cpu->memory.io[WINDOW_X - IO_BASE] = 0x00; // WX
    cpu->memory.ie = 0x00; // INTERRUPTS ENABLED
}

// Create the cpu
Cpu* createCPU() {
    Cpu *cpu = (Cpu *) malloc(sizeof(Cpu));
    // Initialise the cpu
    initCPU(cpu);

    return cpu;
}

// Reset the cpu
void resetCPU(Cpu *cpu) {
    // Re-initialise the cpu
    initCPU(cpu);
    // TDOD: reset timers and display stuff
}

// Execute a single cpu step
int executeCPU(Cpu *cpu) {
    #ifdef DEBUG
        if (debug(false, cpu)) {
            return 1;
        }
    #endif
    int errNum = executeNextInstruction(cpu);
    if (errNum) {
        #ifdef DEBUG
            // Force a run/runto to stop when an error has occurred
            return (debug(true, cpu)) ? 1 : 0;
        #else
            // Exit if non-debug. TODO: output some sort of error message (Maybe method that can popup a box on display builds)
            return errNum;
        #endif
    } else {
        return 0;
    }
}

// Cycle cpu clock.
void cycleCPU(Cpu *cpu) {
    cpu->wait--;
}
