#include "main.h"

// Load the first bank of the rom
static void cartridgeLoad(cpu_state *cpu, FILE *rom) {
    // All cartridge types load 0 -> 0x4000. Catch bad roms/files.
    if (!fread(cpu->MEM, 1, 0x4000, rom)) {
        printf("Incorrect sized rom!\n");
        exit(1248);
    }
}

// Read cartridge info and setup the cpu based on it
static void cartridgeInfo(cpu_state *cpu, FILE *rom) {
    //fetch and store the title
    char title[16];
    for (int i = 0; i < 16; i++) {
        title[i] = (&cpu->MEM[0x134])[i];
    }
    printf("Now playing: %s\n", title);

    //check if it's gbc only rom
    if (cpu->MEM[0x143] == 0xC0) {
        printf("GBC cartridges not supported!\n");
        exit(523);
    }

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
    } else if (cpu->mbc == 1 || cpu->mbc == 2 || cpu->mbc == 3 || cpu->mbc == 5) {
        //malloc the rest of the cartridge
        cpu->CART_ROM = (uint8 *) malloc(ROM_size * 1024 * sizeof(uint8));
		//malloc the external ram
		if (cpu->RAM_exists) {
			cpu->CART_RAM = (uint8 *) malloc(RAM_size * 1024 * sizeof(uint8));
		}
        //read into newly malloced array
        if (!fread(cpu->CART_ROM + 0x4000, 1, (ROM_size * 1024) - 0x4000, rom)) {
            //if the header returns an incorrect cartridge size
            printf("Cartridge not supported1\n");
            exit(13);
        }
    }
}
