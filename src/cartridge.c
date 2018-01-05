#include <stdlib.h>
#include <stdio.h>
#include "cartridge.h"
#include "cpu.h"
#include "memory_map.h"
#include "memory.h"
#include "mbc.h"

// Read cartridge info and setup the cpu based on it
void cartridgeInfo(Cpu *cpu, FILE *rom) {
    // Read in cartridge header
    fseek(rom, CART_HEADER_BASE, SEEK_SET);
    uint8 header[CART_HEADER_BOUND];
    if (fread(header, 1, sizeof(header), rom) < CART_HEADER_BOUND) {
        printf("Header incorrectly read\n");
        exit(795);
    }
    rewind(rom);

    // Fetch and store the title
    char title[16];
    for (int i = 0; i < 16; i++) {
        title[i] = (header + 0x34)[i];
    }
    printf("Now playing: %s\n", title);

    // Check if it's gbc only rom
    if (header[0x43] == 0xC0) {
        printf("GBC cartridges not supported!\n");
        exit(523);
    }

    // Get cartridge type
    cpu->cart_type = header[0x47];
    printf("Cartridge type: 0x%X\n", cpu->cart_type);

    // Get size of cartridge internal rom and ram
    uint8 romValue = header[0x48];
    switch (romValue) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08: cpu->maxRomBank = (2 << romValue); break;
        default:
            printf("Invalid cartridge!\n");
            exit(22);
    }
    uint32 romSize = cpu->maxRomBank * ROM_BANK_SIZE;

    uint8 ramValue = header[0x49];
    uint8 ramSize = 0;
    // Ram size folows no pattern, so set it with a switch
    switch (ramValue) {
        case 0x00:  ramSize = 0;   break;
        case 0x01:  ramSize = 2;   break;
        case 0x02:  ramSize = 8;   break;
        case 0x03:  ramSize = 32;  break;
        case 0x04:  ramSize = 128; break;
        case 0x05:  ramSize = 64;  break;
        default:
            printf("Invalid cartridge!\n");
            exit(22);
    }
    cpu->maxRamBank = (ramSize / 8);
    printf("ROM size: %dKB\nInternal RAM size: %dKB\n", romSize, ramSize);

    // Setup the cpu for the type of cartridge the game is.
    switch (cpu->cart_type) {
        case 0x00:
        case 0x08:
        case 0x09: cpu->mbc = 0; break;
        case 0x01:
        case 0x02:
        case 0x03: cpu->mbc = 1; break;
        case 0x05:
        case 0x06: cpu->mbc = 2; break;
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
    setupMBCCallbacks(cpu);

    cpu->RAM_exists = (ramValue != 0);
    cpu->mbc1_small_ram = (ramValue == 2);

    // Allocate and read in cartridge
    cpu->memory.rom = (uint8 *) malloc(romSize * sizeof(uint8));
    if (!cpu->memory.rom) {
        printf("Unable to malloc space for rom");
        exit(631);
    }
    if (fread(cpu->memory.rom, 1, romSize, rom) < romSize) {
        printf("File size does not match cartridge size\n");
        exit(750);
    }
    cpu->memory.romBank = cpu->memory.rom + ROM_BANK_SIZE;

    // Allocate ram
    if (cpu->RAM_exists) {
        cpu->memory.ram = (uint8 *) malloc(ramSize * 1024 * sizeof(uint8));
        if (!cpu->memory.ram) {
            printf("Unabled to malloc space for ram\n");
            exit(632);
        }
        cpu->memory.ramBank = cpu->memory.ram;
    }
}
