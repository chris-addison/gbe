#ifndef DEBUG_H
#define DEBUG_H

#include "../cpu.h"
#include "../types.h"

#define DEBUG_MAX_ARGS 5

// Returns true to quit
extern bool debug(bool force, Cpu * cpu);

#endif /* DEBUG_H */
