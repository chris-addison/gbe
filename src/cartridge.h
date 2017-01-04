#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdio.h>
#include "cpu.h"

extern void cartridgeLoad(cpu_state *cpu, FILE *rom);
extern void cartridgeInfo(cpu_state *cpu, FILE *rom);

#endif /* CARTRIDE_H */