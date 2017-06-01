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
void updateIME(cpu_state *cpu) {
    //ime must be set or reset after the instruction after EI or DI.
    //by setting imeCounter to 2 with EI or DI this will set or reset
    //the flag at the correct timing.
    if (cpu->imeCounter > 0) {
        cpu->imeCounter--;
        if (cpu->imeCounter == 0) {
            //toggle flag. EI and DI instructions must check flag state.
            cpu->ime = !cpu->ime;
        }
    }
}

//set interrupt flag
void setInterruptFlag(uint8 flag, cpu_state *cpu) {
    writeByte(INTERRUPT_FLAGS, readByte(INTERRUPT_FLAGS, cpu) | flag, cpu);
}

//clear interrupt flag
void clearInterruptFlag(uint8 flag, cpu_state *cpu) {
    writeByte(INTERRUPT_FLAGS, readByte(INTERRUPT_FLAGS, cpu) & ~flag, cpu);
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
    uint8 timerControl = readByte(TAC, cpu);
    // Run timer if enabled
    if (readBit(2, &timerControl)) {
        if (timer_counter >= TIMER_DURATION[timerControl & 0b11]) {
            timer_counter = 0;
            if (readByte(TIMA, cpu) == 255) {
                // Load value from TMA register into TIMA register
                writeByte(TIMA, readByte(TMA, cpu), cpu);
                // Set interrupt
                setInterruptFlag(INTR_TIMER, cpu);
            } else {
                // Increment the TIMA register
                writeByte(TIMA, readByte(TIMA, cpu) + 1, cpu);
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

// Return true if they are interrups enabled and set
uint8 availableInterrupts(cpu_state *cpu) {
    return readByte(INTERRUPT_FLAGS, cpu) & readByte(INTERRUPTS_ENABLED, cpu);
}

// Check interrupts and act on them
void checkInterrupts(cpu_state *cpu) {
    cycleTimer(cpu);
    cycleClock(cpu);
    //printByte(readByte(INTERRUPT_FLAGS, cpu));
    uint8 interrupts = availableInterrupts(cpu);
    if (cpu->ime && interrupts) {
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
    } else if (cpu->halt && interrupts) {
        cpu->halt = false;
    }
}
