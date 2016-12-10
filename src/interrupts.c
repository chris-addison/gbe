/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "types.h"
#include "cpu.h"
#include "common.h"
#include "memory.h"
#include "interrupts.h"

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

//save the PC, jump to interrupt handler, and reset the ime
static void interruptVBlank(cpu_state *cpu) {
    clearInterruptFlag(INTR_V_BLANK, cpu);
    cpu->halt = false;
    cpu->ime = false;
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = 0x40;
    cpu->wait = 12;
}

//check interrupts and act on them
void checkInterrupts(cpu_state *cpu) {
    //printByte(readByte(INTERRUPT_FLAGS, cpu));
    if (cpu->ime && (readByte(INTERRUPT_FLAGS, cpu) & readByte(INTERRUPTS_ENABLED, cpu))) {
        uint8 interrupt = readByte(INTERRUPT_FLAGS, cpu) & readByte(INTERRUPTS_ENABLED, cpu);
        if (interrupt & INTR_V_BLANK) {
            interruptVBlank(cpu);
        }
        if (interrupt & INTR_STAT) {

        }
        if (interrupt & INTR_TIMER) {

        }
        if (interrupt & INTR_SERIAL) {

        }
        if (interrupt & INTR_JOYPAD) {

        }
    }
}
