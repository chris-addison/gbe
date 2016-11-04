#include <stdio.h>

//TODO: add read methods

// Open the rom and check the file exists
static FILE* romLoad(char *file) {
    FILE *rom = fopen(file, "rb");

    // If file doesn't exist, warn user and exit
    if (rom == NULL) {
        fprintf(stderr, "File load error!\n");
        exit(2);
    }

    return rom;
}

// Close the rom
static void romClose(FILE *rom) {
    fclose(rom);
}
