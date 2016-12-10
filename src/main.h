/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef MAIN_H
#define MAIN_H

//debug commands
static const char n[] = "n";
static const char next[] = "next";
static const char q[] = "q";
static const char quit[] = "quit";
static const char r[] = "r";
static const char op[] = "op";
static const char run[] = "run";
static const char runTo[] = "runto";

//memory locations
#define SCANLINE 0xFF44
#define SCROLL_X 0xFF43
#define SCROLL_Y 0xFF42
#define STAT 0xFF41
#define LCDC 0xFF40
#define INTERRUPTS_ENABLED 0xFFFF
#define INTERRUPT_FLAGS 0xFF0F

//debug flag
#define DEBUG true
#define DEBUG_MAX_ARGS 5

//screen constants
#define H_BLANK 0x0
#define V_BLANK 0x1
#define OAM 0x2
#define VRAM 0x3

//interrupt bit offsets
#define INTR_V_BLANK 0b1
#define INTR_STAT 0b10
#define INTR_TIMER 0b100
#define INTR_SERIAL 0b1000
#define INTR_JOYPAD 0b10000

#include "common.c"
#include "debug.c"
#include "cpu.c"

#ifdef DISPLAY
    #include "display.h"
    #include "display.c"
#endif

#include "interrupts.c"
#include "screen.c"

#endif /* MAIN_H */
