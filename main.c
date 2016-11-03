/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"
#include "cartridge.c"

int main(int argc, char *argv[]) {
    //catch lack of file
    if (argc < 2) {
        fprintf(stderr, "Error: Usage: %s [file name]\n", argv[0]);
        exit(1);
    }
    //grab file
    FILE *rom = fopen(argv[1], "rb");

    //TEMP ERROR FILE
    //FILE *logger = fopen("log.txt", "w");

    //if file doesn't exist, warn user and exit
    if (rom == NULL) {
        fprintf(stderr, "file load error\n");
        exit(2);
    }

    //set up cpu
    struct cpu_state * cpu = createCPU();
    //all cartridge types load 0x4000 first
    if (!fread(cpu->MEM, 1, 0x4000, rom)) {
        printf("Incorrect sized rom!\n");
        exit(1248);
    }
    //read and print cartridge info and setup memory banks
    cartridgeInfo(cpu, rom);
    fclose(rom);

    //simple game loop.
    while(true) {
        if (cpu->MEM[0xFF00] == 0x10 || cpu->MEM[0xFF00] == 0x20) {
            cpu->MEM[0xff00] = ~cpu->MEM[0xFF00]; //SET NO BUTTONS PRESSED
        }
        if (cpu->wait <= 0) {
            //printInstruction(true, cpu->PC, cpu);
            //printInstructionToFile(cpu->PC, logger, cpu);
            // 8000-8FFF and at 8800-97FF
            if (cpu->PC == 0x282a) { // Tetris finish writing to Tiles (Address from: http://cturt.github.io/cinoop.html)
                uint8 *vram = cpu->MEM + 0x8000;
                for (int i = 0; i < 16; i++) {
                    printf("%.2X ", *(vram + i));
                }
                printf("\n");
                // Print out tiles
                for (int i = 0; i < 16*256; i += 2) {
                    uint8 byteLow = *(vram + i);
                    uint8 byteHigh = *(vram + i + 1);
                    for (int j = 0; j < 8; j++) {
                        uint8 pixel = ((byteLow & 0x80) >> 7) | ((byteHigh & 0x80) >> 6);
                        if (pixel) {
                            printf("0%X ", pixel);
                        } else {
                            printf("   ");
                        }
                        byteLow <<= 1;
                        byteHigh <<= 1;
                    }
                    printf("\n");
                }
                exit(1);
            }
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
