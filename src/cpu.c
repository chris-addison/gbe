/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifdef DEBUG
    #include "debug/debug.h"
#endif
#include "common.h"
#include "opcodes/opcodes.h"
#include "cpu.h"
#include <stdlib.h>

// Initialise the cpu
static void initCPU(cpu_state *cpu) {
    // Setup the PC and SP
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;
    cpu->wait = 0;
    cpu->ROM_bank = 1;
    cpu->RAM_bank = 0;
    cpu->RAM_enable = false;
    // Setup interrupts
    cpu->ime = false;
    cpu->screen_cycles = 0;
    cpu->imeCounter = 0;
    cpu->halt = false;
    cpu->halt_bug = false;
    // Setup startup values of registers
    cpu->registers.AF = 0x01B0;
    cpu->registers.BC = 0x0013;
    cpu->registers.DE = 0x00D8;
    cpu->registers.HL = 0x014D;
    // Setup startup memory values - excludes values set to 0
    cpu->MEM[0xFF04] = 0x00; // DIV
    cpu->MEM[0xFF05] = 0x00; // TIMA
    cpu->MEM[0xFF06] = 0x00; // TMA
    cpu->MEM[0xFF07] = 0xF8; // TAC
    cpu->MEM[0xFF0F] = 0xE1; // INTERRUPT FLAGS
    cpu->MEM[0xFF10] = 0x80;
    cpu->MEM[0xFF11] = 0xBF;
    cpu->MEM[0xFF12] = 0xF3;
    cpu->MEM[0xFF14] = 0xBF;
    cpu->MEM[0xFF16] = 0x3F;
    cpu->MEM[0xFF19] = 0xBF;
    cpu->MEM[0xFF1A] = 0x7F;
    cpu->MEM[0xFF1B] = 0xFF;
    cpu->MEM[0xFF1C] = 0x9F;
    cpu->MEM[0xFF1E] = 0xBF;
    cpu->MEM[0xFF20] = 0xFF;
    cpu->MEM[0xFF23] = 0xBF;
    cpu->MEM[0xFF24] = 0x77;
    cpu->MEM[0xFF25] = 0xF3;
    cpu->MEM[0xFF26] = 0xF1;
    cpu->MEM[0xFF40] = 0x91; // LCDC
    cpu->MEM[0xFF41] = 0x85; // STAT
    cpu->MEM[0xFF42] = 0x00; // SCY
    cpu->MEM[0xFF43] = 0x00; // SCX
    cpu->MEM[0xFF44] = 0x00; // LY
    cpu->MEM[0xFF45] = 0x00; // LYC
    cpu->MEM[0xFF46] = 0xFF; // DMA
    cpu->MEM[0xFF47] = 0xFC; // BGP
    cpu->MEM[0xFF48] = 0xFF;
    cpu->MEM[0xFF49] = 0xFF;
    cpu->MEM[0xFF4A] = 0x00; // WY
    cpu->MEM[0xFF4B] = 0x00; // WX
    cpu->MEM[0xFFFF] = 0x00; // INTERRUPTS ENABLED
}

// Create the cpu
cpu_state* createCPU() {
    cpu_state *cpu = (cpu_state *) malloc(sizeof(cpu_state));
    // Initialise the cpu
    initCPU(cpu);

    return cpu;
}

// Reset the cpu
void resetCPU(cpu_state *cpu) {
    // Re-initialise the cpu
    initCPU(cpu);
    // TDOD: reset timers and display stuff
}

// Execute a single cpu step
int executeCPU(cpu_state *cpu) {
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
void cycleCPU(cpu_state *cpu) {
    cpu->wait--;
}
