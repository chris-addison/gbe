/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef GBE_H
#define GBE_H

#include "types.h"

#define EMULATOR_INTER_JOYPAD 0b1

extern int startEmulator(int argc, char *argv[]);
extern int cycleEmulator();
extern void emulatorInterrupt(uint32 interruptFlag);
extern void stopEmulator();

#endif /* GBE_H */
