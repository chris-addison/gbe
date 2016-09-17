/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"
#include "debug.c"
#include "opcodes.c"

int main(int argc, char *argv[]) {
    //catch lack of file
    if (argc < 2) {
        fprintf(stderr, "Error: Usage: %s [file name]\n", argv[0]);
        exit(1);
    }
    //grab file
    FILE *rom = fopen(argv[1], "r");
    //if file not exist warn user and exit
    if (rom == NULL) {
        printf("file load error\n");
        exit(2);
    }
    
    //set up cpu
    struct cpu_state * cpu = (struct cpu_state *) malloc(sizeof(struct cpu_state));
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;
    cpu->wait = 0;
    cpu->registers.A = 0x11;
    cpu->MEM[0xFF44] = 0x91;
	//TODO: set the rest of the registers and memory values to their initial settings
    fread(cpu->MEM, 1, 0x8000, rom);

    //fetch and store the title
    char title[16];
    for (int i = 0; i < 16; i++) {
        title[i] = (&cpu->MEM[0x134])[i];
    }
    printf("Now playing: %s\n", title);

    //get cartridge type
    uint8 cart_type = cpu->MEM[0x147];
    printf("Cartridge type: %d\n", cart_type);

    //get cartridge internal rom and ram
    uint8 ROM_value = cpu->MEM[0x148];
    unsigned int ROM_size = 2 << (5 + ROM_value);
    uint8 RAM_value = cpu->MEM[0x149];
    unsigned int RAM_size = 2 << ((2 * RAM_value) - 1);
    printf("ROM size: %dKB\nInternal RAM size: %dKB\n", ROM_size, RAM_size);

    //simple game loop.
    for (int i = 0; i < 100000000; i++) {
        if (cpu->wait <= 0) {
            if (DEBUG) {
                //debug(cpu);
            }
            if (execute(cpu)) {
                debug(cpu);
                //break;
            }
        }
        cpu->wait--;
    }

    //free cpu, cartridge at end
    free(cpu);
    return 0;
}
