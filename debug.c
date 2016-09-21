/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

//simple gdb-like debug
static void debug(cpu_state * cpu) {
    printInstruction(true, cpu->PC, cpu);
    char command[11];
    scanf("%10s", command);
    while (strcmp(n, command) * strcmp(next, command)) {
        if (strcmp(q, command) ^ strcmp(quit, command)) {
            //stop
            printf("Not continuing\n");
            exit(0);
        } else if (!strcmp(op, command)) {
            printInstruction(true, cpu->PC, cpu);
        } else if (!strcmp(r, command)) {
            //print registers
            printf("A:\t");
            printByte(cpu->registers.A);
            printf("\tF:\t");
            printByte(cpu->registers.F);
            printf("\nB:\t");
            printByte(cpu->registers.B);
            printf("\tC:\t");
            printByte(cpu->registers.C);
            printf("\nD:\t");
            printByte(cpu->registers.D);
            printf("\tE:\t");
            printByte(cpu->registers.E);
            printf("\nH:\t");
            printByte(cpu->registers.H);
            printf("\tL:\t");
            printByte(cpu->registers.L);
            printf("\nCF:\t");
            (readFlag(CF, cpu)) ? printf("1") : printf("0");
            printf("\tHF:\t");
            (readFlag(HF, cpu)) ? printf("1") : printf("0");
            printf("\tNF:\t");
            (readFlag(NF, cpu)) ? printf("1") : printf("0");
            printf("\tZF:\t");
            (readFlag(ZF, cpu)) ? printf("1") : printf("0");
            printf("\nAF:\t");
            printShort(cpu->registers.AF);
            printf("\nBC:\t");
            printShort(cpu->registers.BC);
            printf("\nDE:\t");
            printShort(cpu->registers.DE);
            printf("\nHL:\t");
            printShort(cpu->registers.HL);
            printf("\nSP:\t");
            printShort(cpu->SP);
            printf("\nPC:\t");
            printShort(cpu->PC);
            printf("\nLCDC:\t");
            printByte(cpu->MEM[LCDC]);
            printf("\tLY:\t");
            printByte(cpu->MEM[SCANLINE]);
            printf("\tSTAT:\t");
            printByte(cpu->MEM[STAT]);
            printf("\nIME:\t");
            printf("%d", cpu->ime);
            printf("\tIE:\t");
            printByte(cpu->MEM[INTERRUPTS_ENABLED]);
            printf("\tIF:\t");
            printByte(cpu->MEM[INTERRUPT_FLAGS]);
            printf("\n");
        }
        scanf("%10s", command);
    }
}
