/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "common.h"
#include "cpu.h"
#include "screen.h"
#include "memory.h"
#include "interrupts.h"
#ifdef DISPLAY
    #include "display.h"
#endif
#include <time.h>

uint16 cycles = 0;
bool displayActive = true;
uint8 displayActiveCounter = 0;

// Check to see if scanline equals the the LY Compare value. If equal set flag and fire
// interrupt if enabled.
static void updateCoincidenceFlag(cpu_state * cpu) {
    if (cpu->MEM[SCANLINE] == cpu->MEM[SYC]) {
        cpu->MEM[STAT] |= CONINCIDENCE_FLAG;
        // Fire interrupt if enabled
        if (cpu->MEM[STAT] & SCREEN_INTER_LYC) {
            setInterruptFlag(INTR_STAT, cpu);
        }
    } else {
        cpu->MEM[STAT] &= ~CONINCIDENCE_FLAG;
    }
}

// Increment the current scanline(LY)
static void incrementScanline(cpu_state *cpu) {
    cpu->MEM[SCANLINE]++;
    updateCoincidenceFlag(cpu);
}

// Set the scanline(LY)
static void setScanline(uint8 scanline, cpu_state *cpu) {
    cpu->MEM[SCANLINE] = scanline;
    updateCoincidenceFlag(cpu);
}

// Set the current screen mode in the STAT register in memory
static void setMode(uint8 mode, cpu_state *cpu) {
    // Fetch current STAT and set the correct lower two bits
    cpu->MEM[STAT] &= ~0b11;
    cpu->MEM[STAT] |= mode & 0b11;
    // Check what interrupts for STAT are enabled and fire if switching into that mode
    if (mode == H_BLANK && cpu->MEM[STAT] & SCREEN_INTER_H_BLANK) {
        setInterruptFlag(INTR_STAT, cpu);
    } else if (mode == V_BLANK && cpu->MEM[STAT] & SCREEN_INTER_V_BLANK) {
        setInterruptFlag(INTR_STAT, cpu);
    } else if (mode == OAM && cpu->MEM[STAT] & SCREEN_INTER_OAM) {
        setInterruptFlag(INTR_STAT, cpu);
    }
}

// Single step for the logic to control the screen
void updateScreen(cpu_state *cpu) {
    //Order and number of cycles ref: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
    //TL;DR: flow is 143 * (OAM -> VRAM -> H_BLANK) -> 10 * V_BLANK
    if (displayActive) { // DISPLAY ENABLED
        uint8 screenMode = cpu->MEM[STAT] & 0b11; //grab last two bits for checking the screen mode
        cycles++;
        switch (screenMode) {
            case OAM:
                if (cycles >= 80) {
                    setMode(VRAM, cpu);
                    cycles = 0;
                }
                break;
            case VRAM:
                if (cycles >= 172) {
                    // Load scanline during VRAM
                    #ifdef DISPLAY
                        if (displayActive) {
                            loadScanline(cpu);
                        }
                    #endif
                    setMode(H_BLANK, cpu);
                    cycles = 0;
                }
                break;
            case H_BLANK:
                if (cycles >= 204) {
                    incrementScanline(cpu);
                    //switch to vblank when the scanline hits 144
                    if (cpu->MEM[SCANLINE] > 143) {
                        //write new status to the the STAT register
                        setMode(V_BLANK, cpu);
                        //set an interrupt flag
                        setInterruptFlag(INTR_V_BLANK, cpu);
                        // Draw the frame at beginning of v blank.
                        // Only display if correct bit is set. Ths can only be togged during V Blank
                        #ifdef DISPLAY
                            resetWindowLine();
                            if (readBit(7, &cpu->MEM[LCDC])) {
                                draw(cpu);
                            } else {
                                displayActive = false;
                                displayActiveCounter = 255;
                            }
                        #endif
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
                    if (cpu->MEM[SCANLINE] > 153) {
                        //reset the scanline back to 0
                        setScanline(0, cpu);
                        //write new status to the the STAT register
                        setMode(OAM, cpu);
                        //load tiles as V Blank is now over
                        #ifdef DISPLAY
                            loadTiles(cpu);
                        #endif
                    }
                    cycles = 0;
                }
                break;
        }
    } else { // DISPLAY DISABLED
        if (!readBit(7, &cpu->MEM[LCDC])) {
            displayActiveCounter = 255;
        } else {
            displayActiveCounter--;
            if (displayActiveCounter == 0) {
                displayActive = true;
                #ifdef DISPLAY
                    resetWindowLine();
                #endif
                cycles = 0;
                setScanline(0, cpu);
                setMode(V_BLANK, cpu);
            }
        }
    }
}
