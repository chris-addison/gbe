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
bool updateIME(Cpu *cpu) {
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
void setInterruptFlag(uint8 flag, Cpu *cpu) {
    cpu->memory.io[INTERRUPT_FLAGS - IO_BASE] |= flag;
}

//clear interrupt flag
void clearInterruptFlag(uint8 flag, Cpu *cpu) {
    cpu->memory.io[INTERRUPT_FLAGS - IO_BASE] &= ~flag;
}

// Save the PC, jump to interrupt handler, and reset the ime
static void interruptVBlank(Cpu *cpu) {
    clearInterruptFlag(INTR_V_BLANK, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x40;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptSTAT(Cpu *cpu) {
    clearInterruptFlag(INTR_STAT, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x48;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptTimer(Cpu *cpu) {
    clearInterruptFlag(INTR_TIMER, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x50;
    cpu->wait = 12;
}

// Save the PC, just ot interrupt handler, and reset ime
static void interruptJoypad(Cpu *cpu) {
    clearInterruptFlag(INTR_JOYPAD, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x60;
    cpu->wait = 12;
}

// Run timer
static void cycleTimer(Cpu *cpu) {
    // Run timer if enabled
    if (readBit(2, &cpu->memory.io[TAC - IO_BASE])) {
        if (timer_counter >= TIMER_DURATION[cpu->memory.io[TAC - IO_BASE] & 0b11]) {
            timer_counter = 0;
            if (cpu->memory.io[TIMA - IO_BASE] == 255) {
                // Load value from TMA register into TIMA register
                cpu->memory.io[TIMA - IO_BASE] = cpu->memory.io[TMA - IO_BASE];
                // Set interrupt
                setInterruptFlag(INTR_TIMER, cpu);
            } else {
                // Increment the TIMA register
                cpu->memory.io[TIMA - IO_BASE]++;
            }
        }
        timer_counter++;
    }
}

// Cycle the clock for DIV
static void cycleClock(Cpu *cpu) {
    if (cycles_timer == 255) {
        cpu->memory.io[DIV - IO_BASE]++;
    }
    cycles_timer++;
}

// Return all servicable interrupts (enabled and set)
uint8 availableInterrupts(Cpu *cpu) {
    return cpu->memory.io[INTERRUPT_FLAGS - IO_BASE] & cpu->memory.ie & 0x1F;
}

// Check interrupts and act on them
void handleInterrupts(Cpu *cpu, bool active_ime, uint8 interrupts) {
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
