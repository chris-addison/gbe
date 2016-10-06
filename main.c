/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

//read cartridge info and setup the cpu based on it
void cartridgeInfo(cpu_state *cpu, FILE *rom) {
    //fetch and store the title
    char title[16];
    for (int i = 0; i < 16; i++) {
        title[i] = (&cpu->MEM[0x134])[i];
    }
    printf("Now playing: %s\n", title);

    //get cartridge type
    cpu->cart_type = cpu->MEM[0x147];
    printf("Cartridge type: %d\n", cpu->cart_type);

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
    //setup the cpu_state for the type of cartridge the game is.
    switch(cpu->cart_type) {
        case 0x00: cpu->mbc = 0; break;
        case 0x01:
        case 0x02:
        case 0x03: cpu->mbc = 1; break;
        case 0x11:
        case 0x12:
        case 0x13: cpu->mbc = 3; break;
        case 0x19:
        case 0x1A:
        case 0x1B: cpu->mbc = 5; break;
        default:
            printf("Cartridge type not supported\n");
            exit(13);
    }
    cpu->RAM_exists = (RAM_value != 0);
    cpu->mbc1_small_ram = (RAM_value == 2);
    if (cpu->mbc == 0) {
        //read the rest of the game data into address space 0x4000 to 0x7FFF
        fread(cpu->MEM + 0x4000, 1, 0x4000, rom);
    } else if (cpu->mbc == 3 || cpu->mbc == 5) {
        //malloc the rest of the cartridge
        cpu->CART_ROM = (uint8 *) malloc(ROM_size * 1024 * sizeof(uint8));
        //read into newly malloced array
        if (!fread(cpu->CART_ROM + 0x4000, 1, (ROM_size * 1024) - 0x4000, rom)) {
            //if the header returns an incorrect cartridge size
            printf("Cartridge not supported1\n");
            exit(13);
        }
    }
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
    FILE *logger = fopen("log.txt", "w");

    //if file doesn't exist, warn user and exit
    if (rom == NULL) {
        fprintf(stderr, "file load error\n");
        exit(2);
    }

    //set up cpu
    struct cpu_state * cpu = createCPU();
    //all cartridge types load 0x4000 first
    fread(cpu->MEM, 1, 0x4000, rom);
    //read and print cartridge info and setup memory banks
    cartridgeInfo(cpu, rom);
    fclose(rom);

    //simple game loop.
    while(true) {
        cpu->MEM[0xff00] = 0x7E; //SET NO BUTTONS PRESSED 0b11001111
        if (cpu->wait <= 0) {
            //printInstruction(true, cpu->PC, cpu);
            printInstructionToFile(cpu->PC, logger, cpu);
            if (DEBUG) {
                debug(false, cpu);
            }
            if (execute(cpu) && DEBUG) {
                //force a run/runto to stop when an error has occurred
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
    free(cpu->CART_ROM);
    free(cpu->CART_RAM);
    free(cpu);
    return 0;
}
