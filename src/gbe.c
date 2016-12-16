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

int startEmulator(int argc, char *argv[]) {
    // Catch case when no file provided
    if (argc < 2) {
        fprintf(stderr, "Error: Usage: %s [file name]\n", argv[0]);
        exit(1);
    }

    // Open rom
    FILE *rom = romLoad(argv[1]);

    // Set up cpu
    cpu_state *cpu = createCPU();

    // Load cartridge
    cartridgeLoad(cpu, rom);

    // Read and print cartridge info and setup memory banks
    cartridgeInfo(cpu, rom);

    // Close the rom now that all data has been read
    romClose(rom);

    uint8 cycles = 0;    
    // Simple game loop.
    while(true) {
        // TEMP clock
        if (cycles == 255) {
            cpu->MEM[0xFF04]++;
        }
        cycles++;      
        if (cpu->wait <= 0) {            
            executeCPU(cpu);
            //update the IME (Interrupt Master Enable). This allows it to be set at the correct offset.
            updateIME(cpu);    
        }
        //printf("%X\n", cpu->MEM[0xFF00]);
        updateScreen(cpu);
        checkInterrupts(cpu);
        cycleCPU(cpu);        
    }

    //free cpu, cartridge at end
    free(cpu->CART_ROM);
    free(cpu->CART_RAM);
    free(cpu);
    return 0;
}
