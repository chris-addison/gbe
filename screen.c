/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

uint16 cycles = 0;

//read the current scanline(LY) from 0xFF44
static uint8 readScanline(cpu_state *cpu) {
    return readByte(SCANLINE, cpu);
}

//increment the current scanline(LY)
static void incrementScanline(cpu_state *cpu) {
    writeByte(SCANLINE, readScanline(cpu) + 1, cpu);
}

//set the scanline(LY)
static void setScanline(uint8 scanline, cpu_state *cpu) {
    writeByte(SCANLINE, scanline, cpu);
}

//set the current screen mode in the STAT register in memory
static void setMode(uint8 mode, cpu_state *cpu) {
    //fetch current STAT and set the correct lower two bits
    writeByte(STAT, (readByte(STAT, cpu) && ~0b11) | mode, cpu);
}

//logic to control the screen
void updateScreen(cpu_state *cpu) {
    uint8 screenMode = readByte(STAT, cpu) & 0b11; //grab last two bits for checking the screen mode
    cycles++;
    //oder and number of cycles ref: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
    //TL;DR: flow is 143 * (OAM -> VRAM -> H_BLANK) -> 10 * V_BLANK
    switch (screenMode) {
        case OAM:
            if (cycles >= 80) {
                setMode(VRAM, cpu);
                cycles = 0;
            }
            break;
        case VRAM:
            if (cycles >= 172) {
                //TODO: prepare to draw
                setMode(H_BLANK, cpu);
                cycles = 0;
            }
            break;
        case H_BLANK:
            if (cycles >= 204) {
                incrementScanline(cpu);
                //TODO: draw one line
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
        case V_BLANK:
            if (cycles >= 204) {
                incrementScanline(cpu);
                //reset the scanline and switch the mode back to OAM
                if (readScanline(cpu) > 153) {
                    //reset the scanline back to 0
                    setScanline(0, cpu);
                    //write new status to the the STAT register
                    setMode(OAM, cpu);
                }
                cycles = 0;
            }
            break;
    }
}
