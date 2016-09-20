/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

uint16 cycles = 0;

//read the current scanline(LY) from 0xFF44
static uint8 readScanline(cpu_state *cpu) {
    return readByte(0xFF44, cpu);
}

//increment the current scanline(LY)
static void incrementScanline(cpu_state *cpu) {
    cpu->MEM[0xFF44]++;
}

//set the scanline(LY)
static void setScanline(uint8 scanline, cpu_state *cpu) {
    cpu->MEM[0xFF44] = scanline;
}

//set the current screen mode in the STAT register in memory
static void setMode(uint8 mode, cpu_state *cpu) {
    //fetch current STAT and set the correct lower two bits
    writeByte(0xFF41, (readByte(0xFF41, cpu) && ~0b11) | mode, cpu);
}

//logic to control the screen
void updateScreen(cpu_state *cpu) {
    uint8 screenMode = readByte(0xFF41, cpu) & 0b11; //grab last two bits for checking the screen mode
    cycles++;
    //oder and number of cycles ref: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
    //TL;DR: flow is 143 * (OAM -> VRAM -> H_BLANK) -> 10 * V_BLANK
    switch (screenMode) {
        case 0x2: //OAM
            if (cycles >= 80) {
                setMode(VRAM, cpu);
                cycles = 0;
            }
            break;
        case 0x3: //VRAM
            if (cycles >= 172) {
                //TODO: prepare to draw
                setMode(H_BLANK, cpu);
                cycles = 0;
            }
            break;
        case 0x0: //H_BLANK
            if (cycles >= 204) {
                incrementScanline(cpu);
                //TODO: draw stuff
                //switch to vblank when the scanline hits 144
                if (readScanline(cpu) > 143) {
                    //write new status to the the STAT register
                    setMode(V_BLANK, cpu);
                } else {
                    setMode(OAM, cpu);
                }
                cycles = 0;
            }
            break;
        case 0x1: //V_BLANK
            if (cycles >= 204) {
                incrementScanline(cpu);
                //reset the scanline and switch the mode back to OAM
                if (readScanline(cpu) > 153) {
                    //res
                    setScanline(0, cpu);
                    //write new status to the the STAT register
                    setMode(OAM, cpu);
                }
                cycles = 0;
            }
            break;
    }
}
