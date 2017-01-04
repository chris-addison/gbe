#include "types.h"
#include "joypad.h"
#include "input.h"
#include "memory.h"
#include "interrupts.h"
#include <stdio.h>

input current_inputs = {};

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
    cpu->MEM[JOYPAD] |= 0x0F;
    //printf("Status: 0x%X\n", cpu->MEM[JOYPAD]);
    // Select the column
    if (readBit(4, &cpu->MEM[JOYPAD])) {
        getInput(&current_inputs);
        if (current_inputs.a) {
            cpu->MEM[JOYPAD] &= 0xFE;
        }
        if (current_inputs.b) {
            cpu->MEM[JOYPAD] &= 0xFD;
        }
        if (current_inputs.start) {
            cpu->MEM[JOYPAD] &= 0xF7;
        }
        if (current_inputs.select) {
            cpu->MEM[JOYPAD] &= 0xFB;
        }
    } else if (readBit(5, &cpu->MEM[JOYPAD])) {
        getInput(&current_inputs);
        if (current_inputs.up) {
            cpu->MEM[JOYPAD] &= 0xFB;
        }
        if (current_inputs.down) {
            cpu->MEM[JOYPAD] &= 0xF7;
        }
        if (current_inputs.left) {
            cpu->MEM[JOYPAD] &= 0xFD;
        }
        if (current_inputs.right) {
            cpu->MEM[JOYPAD] &= 0xFE;
        }
    }

    // No column selected, so return nothing.
    return cpu->MEM[JOYPAD];
}