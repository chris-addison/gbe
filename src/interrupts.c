/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "cpu.h"
#include "common.h"
#include "memory.h"
#include "interrupts.h"

uint8 cycles_timer = 0;
uint16 timer_counter = 0;
const uint16 TIMER_DURATION[] = {1024, 16, 64, 256};

//set the ime (interrupt master enable)
bool updateIME(cpu_state *cpu) {
    // Grab current state of ime
    bool ime_state = cpu->ime;
    if (cpu->ime_enable) {
        // Toggle state
        cpu->ime = true;
        cpu->ime_enable = false;
    }
    // Return active state of the IME (IME doesn't take effect until after the next instruction has started executing)
    return ime_state;
}

//set interrupt flag
void setInterruptFlag(uint8 flag, cpu_state *cpu) {
    cpu->MEM[INTERRUPT_FLAGS] |= flag;
}

//clear interrupt flag
void clearInterruptFlag(uint8 flag, cpu_state *cpu) {
    cpu->MEM[INTERRUPT_FLAGS] &= ~flag;
}

// Save the PC, jump to interrupt handler, and reset the ime
static void interruptVBlank(cpu_state *cpu) {
    clearInterruptFlag(INTR_V_BLANK, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x40;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptSTAT(cpu_state *cpu) {
    clearInterruptFlag(INTR_STAT, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x48;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptTimer(cpu_state *cpu) {
    clearInterruptFlag(INTR_TIMER, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x50;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptJoypad(cpu_state *cpu) {
    clearInterruptFlag(INTR_JOYPAD, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x60;
    cpu->wait = 12;
}

// Run timer
static void cycleTimer(cpu_state *cpu) {
    // Run timer if enabled
    if (readBit(2, &cpu->MEM[TAC])) {
        if (timer_counter >= TIMER_DURATION[cpu->MEM[TAC] & 0b11]) {
            timer_counter = 0;
            if (cpu->MEM[TIMA] == 255) {
                // Load value from TMA register into TIMA register
                cpu->MEM[TIMA] = cpu->MEM[TMA];
                // Set interrupt
                setInterruptFlag(INTR_TIMER, cpu);
            } else {
                // Increment the TIMA register
                cpu->MEM[TIMA]++;
            }
        }
        timer_counter++;
    }
}

// Cycle the clock for DIV
static void cycleClock(cpu_state *cpu) {
    if (cycles_timer == 255) {
        cpu->MEM[DIV] += 1;
    }
    cycles_timer++;
}

// Return all servicable interrupts (enabled and set)
uint8 availableInterrupts(cpu_state *cpu) {
    return cpu->MEM[INTERRUPT_FLAGS] & cpu->MEM[INTERRUPTS_ENABLED] & 0x1F;
}

// Check interrupts and act on them
void handleInterrupts(cpu_state *cpu, bool active_ime, uint8 interrupts) {
    cycleTimer(cpu);
    cycleClock(cpu);
    //printByte(readByte(INTERRUPT_FLAGS, cpu));
    if (active_ime && interrupts) {
        if (interrupts & INTR_V_BLANK) {
            interruptVBlank(cpu);
        }
        if (interrupts & INTR_STAT) {
            printf("interrupt STAT\n");
            interruptSTAT(cpu);
        }
        if (interrupts & INTR_TIMER) {
            printf("interrupt timer\n");
            interruptTimer(cpu);
        }
        if (interrupts & INTR_SERIAL) {
            printf("interrupt serial\n");
        }
        if (interrupts & INTR_JOYPAD) {
            printf("interrupt joypad\n");
            interruptJoypad(cpu);
        }
    }
}
