/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

void cartridgeInfo(cpu_state *cpu) {
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
    unsigned int ROM_size = 32 << ROM_value;
    uint8 RAM_value = cpu->MEM[0x149];
    uint8 RAM_size = 0;
    //ram size folows no pattern, so set it with a switch
    switch(RAM_value) {
        case 0x00:  RAM_size = 0;   break;
        case 0x01:  RAM_size = 2;   break;
        case 0x02:  RAM_size = 8;   break;
        case 0x03:  RAM_size = 32;  break;
        case 0x04:  RAM_size = 128; break;
        case 0x05:  RAM_size = 64;  break;
        default:
            printf("Invalid cartridge!\n");
            exit(22);
    }
    printf("ROM size: %dKB\nInternal RAM size: %dKB\n", ROM_size, RAM_size);
}

int main(int argc, char *argv[]) {
    //catch lack of file
    if (argc < 2) {
        fprintf(stderr, "Error: Usage: %s [file name]\n", argv[0]);
        exit(1);
    }
    //grab file
    FILE *rom = fopen(argv[1], "r");

    //TEMP ERROR FILE
    //FILE *logger = fopen("log.txt", "w");

    //if file doesn't exist, warn user and exit
    if (rom == NULL) {
        fprintf(stderr, "file load error\n");
        exit(2);
    }

    //set up cpu
    struct cpu_state * cpu = createCPU();
    fread(cpu->MEM, 1, 0x8000, rom);
    fclose(rom);

    //read and print cartridge info
    cartridgeInfo(cpu);

    //simple game loop.
    while(true) {
        cpu->MEM[0xff00] |= 0xCF; //SET NO BUTTONS PRESSED 0b11001111
        if (cpu->wait <= 0) {
            //printInstruction(true, cpu->PC, cpu);
            //printInstructionToFile(cpu->PC, logger, cpu);
            debug(false, cpu);
            if (execute(cpu) && DEBUG) {
                debug(true, cpu);
            }
            //update the IME (Interrupt Master Enable). This allows it to be set at the correct offset.
            updateIME(cpu);
        }
        updateScreen(cpu);
        checkInterrupts(cpu);
        cpu->wait--;
    }

    //free cpu, cartridge at end
    free(cpu);
    return 0;
}
