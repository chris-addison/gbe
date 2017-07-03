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
static bool displayEnabled(cpu_state *cpu) {
    // 7th bit of LCDC
    return readBit(7, &cpu->MEM[LCDC]);
}

// Check if a STAT interrupt should happen with the given STAT state
static bool checkSTATInterrupt(uint8 stat) {
    if ((stat & STAT_INTR_H_BLANK) && (stat & STAT_INTR_LYC)) {
        return true;
    }
    uint8 mode = stat & STAT_MODE_MASK;
    if (mode == H_BLANK) {
        return (stat & STAT_INTR_H_BLANK);
    } else if (mode == V_BLANK) {
        return (stat & STAT_INTR_V_BLANK);
    } else if (mode == OAM) {
        return (stat & STAT_INTR_OAM);
    }
    return false;
}

// Check to see if scanline equals the the LY Compare value. If equal set flag and fire
// interrupt if enabled.
static void updateCoincidenceFlag(cpu_state * cpu) {
    if (cpu->MEM[SCANLINE] == cpu->MEM[SYC]) {
        cpu->MEM[STAT] |= CONINCIDENCE_FLAG;
        // Fire interrupt if enabled
        if (cpu->MEM[STAT] & STAT_INTR_LYC) {
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
    cpu->MEM[STAT] &= ~STAT_MODE_MASK;
    cpu->MEM[STAT] |= mode & STAT_MODE_MASK;
}

// Timing and control flow taking from SameBoy, which is derived itself from GiiBiiAdvance
// https://github.com/LIJI32/SameBoy/blob/master/Core/display.c
// https://github.com/AntonioND/giibiiadvance

// Single step for the logic to control the screen
void updateScreen(cpu_state *cpu) {
    // Cycle the screen
    cpu->screen_cycles++;
    // LCDC behavior different for display on vs off
    if (displayEnabled(cpu)) {
        uint16 cycles = cpu->screen_cycles % LCDC_LINE_CYCLES;
        if (cpu->MEM[SCANLINE] < SCREEN_HEIGHT - 1) {
            if (cpu->screen_cycles > 0 && cycles == 0) {
                incrementScanline(cpu);
            } else if (cycles == LCDC_OAM_CYCLES) {
                setMode(VRAM, cpu);
            } else if (cycles == LCDC_OAM_CYCLES + LCDC_VRAM_CYCLES) {
                setMode(H_BLANK, cpu);
            } else if (cycles == LCDC_OAM_CYCLES + LCDC_VRAM_CYCLES + LCDC_H_BLANK_CYCLES) {
                setMode(OAM, cpu);
                loadScanline(cpu);
            }
        } else if (cpu->MEM[SCANLINE] == SCREEN_HEIGHT - 1) {
            draw(cpu);
        }
    } else {
        // Reset scanline
        cpu->MEM[SCANLINE] = 0;

        // Set mode to 0
        cpu->MEM[STAT] &= ~STAT_MODE_MASK;

        // Still handle v_blank when display is disabled
        if (cpu->screen_cycles >= LCDC_SCREEN_CYCLES) {
            cpu->screen_cycles -= LCDC_SCREEN_CYCLES;
            setInterruptFlag(INTR_V_BLANK, cpu);
            draw(cpu);
        }

        // Reset window line
        cpu->window_line = 0xFF;
    }


}
