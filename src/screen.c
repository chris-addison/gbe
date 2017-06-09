/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "common.h"
#include "cpu.h"
#include "screen.h"
#include "memory.h"
#include "interrupts.h"
#include "display.h"
#include <time.h>

// Read the current scanline(LY) from 0xFF44
static uint8 readScanline(cpu_state *cpu) {
    return cpu->MEM[SCANLINE];
}

// Return true if the display is active
static bool displayActivated(cpu_state *cpu) {
    // 7th bit of LCDC
    return readBit(7, &cpu->MEM[LCDC]);
}

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

// Timing and control flow taking from SameBoy, which is derived itself from GiiBiiAdvance
// https://github.com/LIJI32/SameBoy/blob/master/Core/display.c
// https://github.com/AntonioND/giibiiadvance

// Single step for the logic to control the screen
void updateScreen(cpu_state *cpu) {
    // Cycle the screen
    cpu->screen_cycles++;
    if (displayActivated(cpu)) { // DISPLAY ENABLED
        // Update scanline
        cpu->MEM[SCANLINE] = cpu->screen_cycles / LCDC_LINE_CYCLES;
        printf("SCANLINE: %d\n", cpu->MEM[SCANLINE]);
        // Handle end of the screen
        if (cpu->screen_cycles == LCDC_SCREEN_CYCLES) {
            cpu->screen_cycles = 0;
            // Set mode to be 0
            cpu->MEM[STAT] &= ~STAT_MODE_MASK;
            // Set scanline to 0
            cpu->MEM[SCANLINE] = 0;

            // Reset window line
            resetWindowLine();
        // Handle V-blank
        } else if (cpu->screen_cycles == LCDC_SCREEN_CYCLES * SCREEN_HEIGHT) {
            // Set mode to 1 (V-BLANK)
            cpu->MEM[STAT] &= ~STAT_MODE_MASK;
            cpu->MEM[STAT] |= V_BLANK;
            // Set v blank interrupt flag
            setInterruptFlag(INTR_V_BLANK, cpu);

            // Entering v-blank fires OAM interrupt if enabled
            if (cpu->MEM[STAT] & SCREEN_INTER_OAM) {
                //TODO: Interrupt
            }

            draw(cpu);
        // Handle lines 0 to 143 (actually displayed lines)
        } else if (cpu->screen_cycles < SCREEN_HEIGHT * LCDC_LINE_CYCLES) {
            uint16 position = cpu->screen_cycles % LCDC_LINE_CYCLES;
            if (position == 0) {
                // Set mode to 2 (OAM)
                cpu->MEM[STAT] &= ~STAT_MODE_MASK;
                cpu->MEM[STAT] |= OAM;

                // TODO: check window and increment window line
                printf("increment scanline!\n");
                loadScanline(cpu);
            } else if (position == LCDC_MODE2_CYCLES) {
                // Set mode to 3 (VRAM)
                cpu->MEM[STAT] &= ~STAT_MODE_MASK;
                cpu->MEM[STAT] |= VRAM;

                // Reset window line
                resetWindowLine();
            } else if (position == LCDC_MODE2_CYCLES + LCDC_MODE3_CYCLES) {
                // Set mode to 0 (H-BLANK)
                cpu->MEM[STAT] &= ~STAT_MODE_MASK;
            }
        // Handle line 153
        } else if (cpu->screen_cycles >= (SCREEN_LINES - 1) * LCDC_LINE_CYCLES) {
            cpu->MEM[SCANLINE] = 0;
        // Handle lines 144 to 152
        }

        updateCoincidenceFlag(cpu);

    } else { // DISPLAY DISABLED
        // Keep scanline at 0
        cpu->MEM[SCANLINE] = 0;
        // Set mode to 0
        cpu->MEM[STAT] &= ~STAT_MODE_MASK;
        // Set coincidence flag
        cpu->MEM[STAT] |= CONINCIDENCE_FLAG;
        // Still handle vblank
        if (cpu->screen_cycles == LCDC_SCREEN_CYCLES) {
            cpu->screen_cycles = 0;
            setInterruptFlag(INTR_V_BLANK, cpu);
            draw(cpu);
        }

        // Reset window line
        resetWindowLine();
    }

    /*uint8 screenMode = cpu->MEM[STAT] & 0b11; //grab last two bits for checking the screen mode
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
                        loadScanline(cpu);
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
                            draw(cpu);
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
        }*/
}
