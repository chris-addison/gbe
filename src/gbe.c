/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "gbe.h"
#include "cpu.h"
#include "screen.h"
#include "interrupts.h"
#include "cartridge.h"
#include "file.c"
#include "opcodes/opcodes.h"
#include "display.h"
#include "joypad.h"
#include <stdlib.h>

cpu_state *cpu;
uint8 cycles_timer = 0; 

int startEmulator(int argc, char *argv[]) {
    // Catch case when no file provided
    if (argc < 2) {
        fprintf(stderr, "Error: Usage: %s [file name]\n", argv[0]);
        exit(1);
    }

    // Open rom
    FILE *rom = romLoad(argv[1]);

    // Set up cpu
    cpu = createCPU();

    // Load cartridge
    cartridgeLoad(cpu, rom);

    // Read and print cartridge info and setup memory banks
    cartridgeInfo(cpu, rom);

    // Close the rom now that all data has been read
    romClose(rom);
    
    return 0;
}

// Step the emulator one instruction.
int cycleEmulator() {
    // Execute instruction       
    executeCPU(cpu);
    // Update the IME (Interrupt Master Enable). This allows it to be set at the correct offset.
    updateIME(cpu);
    // TEMP clock
    if (cycles_timer == 255) {
        cpu->MEM[0xFF04]++;
    }
    cycles_timer++;
    while (cpu->wait > 0) {
        updateScreen(cpu);
        checkInterrupts(cpu);
        cycleCPU(cpu);
    }
    // Instruction complete return 0
    return 0;
}

void stopEmulator() {
    //free cpu, cartridge at end
    free(cpu->CART_ROM);
    free(cpu->CART_RAM);
    free(cpu);
}
