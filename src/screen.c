/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "common.h"
#include "cpu.h"
#include "screen.h"
#include "memory.h"
#include "interrupts.h"
#include "display.h"
#include <time.h>

uint16 cycles = 0;
bool displayActive = true;
uint8 displayActiveCounter = 0;

// Check to see if scanline equals the the LY Compare value. If equal set flag and fire
// interrupt if enabled.
static void updateCoincidenceFlag(Cpu * cpu) {
    if (cpu->memory.io[SCANLINE - IO_BASE] == cpu->memory.io[SYC - IO_BASE]) {
        cpu->memory.io[STAT - IO_BASE] |= CONINCIDENCE_FLAG;
        // Fire interrupt if enabled
        if (cpu->memory.io[STAT - IO_BASE] & SCREEN_INTER_LYC) {
            setInterruptFlag(INTR_STAT, cpu);
        }
    } else {
        cpu->memory.io[STAT - IO_BASE] &= ~CONINCIDENCE_FLAG;
    }
}

// Increment the current scanline(LY)
static void incrementScanline(Cpu *cpu) {
    cpu->memory.io[SCANLINE - IO_BASE]++;
    updateCoincidenceFlag(cpu);
}

// Set the scanline(LY)
static void setScanline(uint8 scanline, Cpu *cpu) {
    cpu->memory.io[SCANLINE - IO_BASE] = scanline;
    updateCoincidenceFlag(cpu);
}

// Set the current screen mode in the STAT register in memory
static void setMode(uint8 mode, Cpu *cpu) {
    // Fetch current STAT and set the correct lower two bits
    cpu->memory.io[STAT - IO_BASE] &= ~0b11;
    cpu->memory.io[STAT - IO_BASE] |= mode & 0b11;
    // Check what interrupts for STAT are enabled and fire if switching into that mode
    if (mode == H_BLANK && cpu->memory.io[STAT - IO_BASE] & SCREEN_INTER_H_BLANK) {
        setInterruptFlag(INTR_STAT, cpu);
    } else if (mode == V_BLANK && cpu->memory.io[STAT - IO_BASE] & SCREEN_INTER_V_BLANK) {
        setInterruptFlag(INTR_STAT, cpu);
    } else if (mode == OAM && cpu->memory.io[STAT - IO_BASE] & SCREEN_INTER_OAM) {
        setInterruptFlag(INTR_STAT, cpu);
    }
}

// Single step for the logic to control the screen
void updateScreen(Cpu *cpu) {
    //Order and number of cycles ref: http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
    //TL;DR: flow is 143 * (OAM -> VRAM -> H_BLANK) -> 10 * V_BLANK
    if (displayActive) { // DISPLAY ENABLED
        uint8 screenMode = cpu->memory.io[STAT - IO_BASE] & 0b11; //grab last two bits for checking the screen mode
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
                    if (displayActive) {
                        loadScanline(cpu);
                    }
                    setMode(H_BLANK, cpu);
                    cycles = 0;
                }
                break;
            case H_BLANK:
                if (cycles >= 204) {
                    incrementScanline(cpu);
                    //switch to vblank when the scanline hits 144
                    if (cpu->memory.io[SCANLINE - IO_BASE] > 143) {
                        //write new status to the the STAT register
                        setMode(V_BLANK, cpu);
                        //set an interrupt flag
                        setInterruptFlag(INTR_V_BLANK, cpu);
                        // Draw the frame at beginning of v blank.
                        // Only display if correct bit is set. Ths can only be togged during V Blank
                        resetWindowLine();
                        if (readBit(7, &cpu->memory.io[LCDC - IO_BASE])) {
                            draw(cpu);
                        } else {
                            displayActive = false;
                            displayActiveCounter = 255;
                        }
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
                    if (cpu->memory.io[SCANLINE - IO_BASE] > 153) {
                        //reset the scanline back to 0
                        setScanline(0, cpu);
                        //write new status to the the STAT register
                        setMode(OAM, cpu);
                        //load tiles as V Blank is now over
                        loadTiles(cpu);
                    }
                    cycles = 0;
                }
                break;
        }
    } else { // DISPLAY DISABLED
        if (!readBit(7, &cpu->memory.io[LCDC - IO_BASE])) {
            displayActiveCounter = 255;
        } else {
            displayActiveCounter--;
            if (displayActiveCounter == 0) {
                displayActive = true;
                resetWindowLine();
                cycles = 0;
                setScanline(0, cpu);
                setMode(V_BLANK, cpu);
            }
        }
    }
}
