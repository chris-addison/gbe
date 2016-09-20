/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

//set the ime (interrupt master enable)
static void updateIME(cpu_state *cpu) {
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

//check interrupts and act on them
static void checkInterrupts(cpu_state *cpu) {
    if (cpu->ime && (readByte(0xFF0F, cpu) & readByte(0xFFFF, cpu))) {
        uint8 interrupt = readByte(0xFF0F, cpu) & readByte(0xFFFF, cpu);
        if (interrupt & 0b1) { //v blank
            //TODO: clean this up
            writeByte(0xFF0F, readByte(0xFF0F, cpu) & ~0b1, cpu);
            cpu->ime = false;
            writeShortToStack(cpu->PC, cpu);
            cpu->PC = 0x40;
            cpu->wait = 12;
        }
        if (interrupt & 0b10) { //lcd stat

        }
        if (interrupt & 0b100) { //timer

        }
        if (interrupt & 0b1000) { //serial

        }
        if (interrupt & 0b10000) { //joypad

        }
    }
}
