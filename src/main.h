/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef MAIN_H
#define MAIN_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

//one byte
typedef uint8_t uint8; //unsigned
typedef int8_t int8;   //signed
//two bytes
typedef uint16_t uint16;
//four bytes
typedef uint32_t uint32;
//debug flag
#define DEBUG true
#define DEBUG_MAX_ARGS 5

//struct to hold the cpu state
typedef struct cpu_state {
    uint8 MEM[0x10000];
    struct registers {
        union {
            struct {
                uint8 F;
                uint8 A;
            };
            uint16 AF;
        };
        union {
            struct {
                uint8 C;
                uint8 B;
            };
            uint16 BC;
        };
        union {
            struct {
                uint8 E;
                uint8 D;
            };
            uint16 DE;
        };
        union {
            struct {
                uint8 L;
                uint8 H;
            };
            uint16 HL;
        };
    } registers;
    uint8 *CART_RAM;
    uint8 *CART_ROM;
    uint16 PC;
    uint16 SP;
    uint16 cycles;
    uint16 ROM_bank;
    uint8 RAM_bank;
    uint8 cart_type;
    uint8 mbc;
    uint8 wait;
    uint8 imeCounter;
    bool halt;
    bool RAM_enable;
    bool RAM_exists;
    bool mbc1_mode;
    bool mbc1_small_ram;
    bool ime;
} cpu_state;

//constant positions of flags in flags array
static const uint8 CF = 4;
static const uint8 HF = 5;
static const uint8 NF = 6;
static const uint8 ZF = 7;

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
