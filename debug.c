/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

//simple gdb-like debug
static void debug(cpu_state * cpu) {
    char command[11];
    scanf("%10s", command);
    while (strcmp(n,command) * strcmp(next,command)) {
        printf("Not continuing\n");
        if (strcmp(q,command) || strcmp(quit,command)) {
            exit(0);
        }
        scanf("%10s", command);
    }
}
