/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef MAIN_H
#define MAIN_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

static const char n[] = "n";
static const char next[] = "next";
static const char q[] = "q";
static const char quit[] = "quit";

//one byte
typedef uint8_t uint8; //unsigned
typedef int8_t int8;   //signed
//two bytes
typedef uint16_t uint16;
//debug flag
#define DEBUG true

//struct to hold the cpu state
typedef struct cpu_state {
    uint8 MEM[0x10000];
    uint8 REG[8];
    uint8 FLAGS[8];
    uint16 PC;
    uint16 SP;
    uint8 wait;
} cpu_state;

//constant positions of registers in reg array
static const uint8 A = 0;
static const uint8 F = 1;
static const uint8 B = 2;
static const uint8 C = 3;
static const uint8 D = 4;
static const uint8 E = 5;
static const uint8 H = 6;
static const uint8 L = 7;

//constant positions of flags in flags array
static const uint8 CF = 4;
static const uint8 HF = 5;
static const uint8 NF = 6;
static const uint8 ZF = 7;


#endif /* MAIN_H */
