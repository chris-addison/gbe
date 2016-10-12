/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

static struct cpu_state* createCPU() {
    struct cpu_state *cpu = (struct cpu_state *) malloc(sizeof(struct cpu_state));
    //setup the PC and SP
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;
    cpu->wait = 0;
    cpu->ROM_bank = 1;
    cpu->RAM_bank = 0;
    cpu->RAM_enable = false;
    //setup startup values of registers
    cpu->registers.AF = 0x01B0;
    cpu->registers.BC = 0x0013;
    cpu->registers.DE = 0x00D8;
    cpu->registers.HL = 0x014D;
    //setup startup memory values - excludes values set to 0
    cpu->MEM[0xFF10] = 0x80;
    cpu->MEM[0xFF11] = 0xBF;
    cpu->MEM[0xFF12] = 0xF3;
    cpu->MEM[0xFF14] = 0xBF;
    cpu->MEM[0xFF16] = 0x3F;
    cpu->MEM[0xFF19] = 0xBF;
    cpu->MEM[0xFF1A] = 0x7F;
    cpu->MEM[0xFF1B] = 0xFF;
    cpu->MEM[0xFF1C] = 0x9F;
    cpu->MEM[0xFF1E] = 0xBF;
    cpu->MEM[0xFF20] = 0xFF;
    cpu->MEM[0xFF23] = 0xBF;
    cpu->MEM[0xFF24] = 0x77;
    cpu->MEM[0xFF25] = 0xF3;
    cpu->MEM[0xFF26] = 0xF1;
    cpu->MEM[0xFF40] = 0x91;
    cpu->MEM[0xFF47] = 0xFC;
    cpu->MEM[0xFF48] = 0xFF;
    cpu->MEM[0xFF49] = 0xFF;

    return cpu;
}
