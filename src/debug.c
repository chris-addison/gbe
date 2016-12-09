/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

bool runUntilStop = false;
bool runToTarget = false;
uint16 targetAddress = 0x0;

static void clearStdin() {
    int c;
    while((c = getchar()) != '\n' && c != EOF ) {}
}

//simple gdb-like debug
void debug(bool force, cpu_state * cpu) {
    //only debug if stepping one instruction at a time.
    //if reach target address for runto then enter stepping again
    if (!force && (runUntilStop || (runToTarget && (targetAddress != cpu->PC)))) {
        return;
    } else if (runToTarget && (targetAddress == cpu->PC)) {
        runToTarget = false;
        targetAddress = 0;
    }
    printInstruction(true, cpu->PC, cpu);
    char command[21];
    //keep scanning while valid input is provided.
    while(scanf("%20[^\n]", command)) {
        //clear stdin to avoid any potential loops or bugs
        clearStdin();
        //split the input into seperate array entries. One for each space-seperated string
        //credit: http://stackoverflow.com/a/13281447
        //the array simply points to indexes in the command, so don't modify until done.
        uint8 argc = 0;
        char *argv[DEBUG_MAX_ARGS];
        char *arg = strtok(command, " ");
        while(arg && argc < DEBUG_MAX_ARGS -1) {
            argv[argc++] = arg;
            arg = strtok(0, " ");
        }
        argv[argc] = NULL; //set entry after last as null
        if (strcmp(n, argv[0]) ^ strcmp(next, argv[0])) {
            //clear any run bools if debug gets called and a user wishes to step through
            runUntilStop = false;
            runToTarget = false;
            targetAddress = 0x0;
            break;
        } else if (strcmp(q, argv[0]) ^ strcmp(quit, argv[0])) {
            //stop
            printf("Not continuing\n");
            exit(0);
        } else if (!strcmp(op, argv[0])) {
            printInstruction(true, cpu->PC, cpu);
        } else if (!strcmp(run, argv[0])) {
            //run until debug is called and the user wishes to step through
            runUntilStop = true;
            break;
        } else if (!strcmp(runTo, argv[0])) { //runto 0x0151
            //run to a specific address
            targetAddress = strtol(argv[1], NULL, 0);
            runToTarget = true;
            break;
        } else if (!strcmp(r, argv[0])) {
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
            printf("\nROM:\t");
            printShort(cpu->ROM_bank);
            printf("\tRAM enabled:\t");
            (cpu->RAM_enable) ? printf("1") : printf("0");
            printf("\tRAM:\t");
            printByte(cpu->RAM_bank);
            printf("\n");
        } else {
            printf("Command: \"%s\" not recognised\n", command);
        }
    }
}
