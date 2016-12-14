#include "types.h"
#include "joypad.h"
#include "input.h"
#include "memory.h"
#include "interrupts.h"
#include <stdio.h>

// Return the hardware representation of the input state.
// 0 is used to indicate value, thus 0xF means no buttons pressed.
// There are two columns of values. The first two bits of the upper byte set the 
// column to return input from.
//
// Column 0: 0x1
// 0xE is A
// 0xD is B
// 0x7 is Start
// 0xB is Select
//
// Column 1: 0x2
// 0xB is up
// 0x7 is down
// 0xD is left
// 0xE is right
uint8 getJoypadState(cpu_state *cpu) {
    // Get the inputs from the frontend
    input current_inputs = getInput(); 
    cpu->MEM[JOYPAD] |= 0x0F;
    //printf("Status: 0x%X\n", cpu->MEM[JOYPAD]);
    // Select the column
    if (readBit(4, &cpu->MEM[JOYPAD])) {      
        if (current_inputs.A) {
            cpu->MEM[JOYPAD] &= 0xE;
        }
        if (current_inputs.B) {
            cpu->MEM[JOYPAD] &= 0xD;
        }
        if (current_inputs.start) {
            cpu->MEM[JOYPAD] &= 0x7;
        }
        if (current_inputs.select) {
            cpu->MEM[JOYPAD] &= 0xB;
        }
    } else if (readBit(5, &cpu->MEM[JOYPAD])) {
        if (current_inputs.up) {
            cpu->MEM[JOYPAD] &= 0xB;
        }
        if (current_inputs.down) {
            cpu->MEM[JOYPAD] &= 0x7;
        }
        if (current_inputs.left) {
            cpu->MEM[JOYPAD] &= 0xD;
        }
        if (current_inputs.right) {
            cpu->MEM[JOYPAD] &= 0xE;
        }
    }

    if (current_inputs.A || current_inputs.B || current_inputs.start || current_inputs.select ||
     current_inputs.up || current_inputs.down || current_inputs.left || current_inputs.right) {
        //printf("Interrupt\n");
        setInterruptFlag(INTR_JOYPAD, cpu);
    }

    // No column selected, so return nothing.
    return cpu->MEM[JOYPAD];    
}