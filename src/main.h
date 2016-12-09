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

//structure to store list of opcodes and associated info
struct opcode {
	char* name;
	uint8 cycles;
    uint8 cyclesMax;
    uint8 bytes;
} const opcodes[256];

//structure to store list of CB-prefix opcodes
struct cbOpcode {
    char* name;
    uint8 cycles;
} const cbOpcodes[256];

//store format to printf each instruction, it's number of cycles, and how many bytes the instruction is in memory.
const struct opcode opcodes[256] = {
	{ "0x00 NOP", 4, 4, 1 },
	{ "0x01 LD BC, 0x%04X", 12, 12, 3 },
	{ "0x02 LD (BC), A", 8, 8, 1 },
	{ "0x03 INC BC", 8, 8, 1 },
	{ "0x04 INC B", 4, 4, 1 },
	{ "0x05 DEC B", 4, 4, 1 },
	{ "0x06 LD B, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x07 RLCA", 4, 4, 1 },
	{ "0x08 LD (0x%04X), SP", 20, 20, 3 },
	{ "0x09 ADD HL, BC", 8, 8, 1 },
	{ "0x0A LD A, (BC)", 8, 8, 1 },
	{ "0x0B DEC BC", 8, 8, 1 },
	{ "0x0C INC C", 4, 4, 1 },
	{ "0x0D DEC C", 4, 4, 1 },
	{ "0x0E LD C, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x0F RRCA", 4, 4, 1 },
	{ "0x10 STOP 0", 4, 4, 2 },
	{ "0x11 LD DE, 0x%04X", 12, 12, 3 },
	{ "0x12 LD (DE), A", 8, 8, 1 },
	{ "0x13 INC DE", 8, 8, 1 },
	{ "0x14 INC D", 4, 4, 1 },
	{ "0x15 DEC D", 4, 4, 1 },
	{ "0x16 LD D, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x17 RLA", 4, 4, 1 },
	{ "0x18 JR 0x%02" PRIX8, 12, 12, 2 },
	{ "0x19 ADD HL, DE", 8, 8, 1 },
	{ "0x1A LD A, (DE)", 8, 8, 1 },
	{ "0x1B DEC DE", 8, 8, 1 },
	{ "0x1C INC E", 4, 4, 1 },
	{ "0x1D DEC E", 4, 4, 1 },
	{ "0x1E LD E, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x1F RRA", 4, 4, 1 },
	{ "0x20 JR NZ, 0x%02" PRIX8, 8, 12, 2 },
	{ "0x21 LD HL, 0x%04X", 12, 12, 3 },
	{ "0x22 LDI (HL), A", 8, 8, 1 },
	{ "0x23 INC HL", 8, 8, 1 },
	{ "0x24 INC H", 4, 4, 1 },
	{ "0x25 DEC H", 4, 4, 1 },
	{ "0x26 LD H, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x27 DAA", 4, 4, 1 },
	{ "0x28 JR Z, 0x%02" PRIX8, 8, 12, 2 },
	{ "0x29 ADD HL, HL", 8, 8, 1 },
	{ "0x2A LDI A, (HL)", 8, 8, 1 },
	{ "0x2B DEC HL", 8, 8, 1 },
	{ "0x2C INC L", 4, 4, 1 },
	{ "0x2D DEC L", 4, 4, 1 },
	{ "0x2E LD L, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x2F CPL", 4, 4, 1 },
	{ "0x30 JR NC, 0x%02" PRIX8, 8, 12, 2},
	{ "0x31 LD SP, 0x%04X", 12, 12, 3 },
	{ "0x32 LDD (HL), A", 8, 8, 1 },
	{ "0x33 INC SP", 8, 8, 1 },
	{ "0x34 INC (HL)", 12, 12, 1 },
	{ "0x35 DEC (HL)", 12, 12, 1 },
	{ "0x36 LD (HL), 0x%02" PRIX8, 12, 12, 2 },
    { "0x37 SCF", 4, 4, 1 },
	{ "0x38 JR C, 0x%02" PRIX8, 8, 12, 2 },
	{ "0x39 ADD HL, SP", 8, 8, 1 },
	{ "0x3A LDD A, (HL)", 8, 8, 1 },
	{ "0x3B DEC SP", 8, 8, 1 },
	{ "0x3C INC A", 4, 4, 1 },
	{ "0x3D DEC A", 4, 4, 1 },
	{ "0x3E LD A, 0x%02" PRIX8, 8, 8, 2 },
	{ "0x3F CCF", 4, 4, 1 },
	{ "0x40 LD B, B", 4, 4, 1 },
	{ "0x41 LD B, C", 4, 4, 1 },
	{ "0x42 LD B, D", 4, 4, 1 },
	{ "0x43 LD B, E", 4, 4, 1 },
	{ "0x44 LD B, H", 4, 4, 1 },
	{ "0x45 LD B, L", 4, 4, 1 },
	{ "0x46 LD B, (HL)", 8, 8, 1 },
	{ "0x47 LD B, A", 4, 4, 1 },
	{ "0x48 LD C, B", 4, 4, 1 },
	{ "0x49 LD C, C", 4, 4, 1 },
	{ "0x4A LD C, D", 4, 4, 1 },
	{ "0x4B LD C, E", 4, 4, 1 },
	{ "0x4C LD C, H", 4, 4, 1 },
	{ "0x4D LD C, L", 4, 4, 1 },
	{ "0x4E LD C, (HL)", 8, 8, 1 },
	{ "0x4F LD C, A", 4, 4, 1 },
	{ "0x50 LD D, B", 4, 4, 1 },
	{ "0x51 LD D, C", 4, 4, 1 },
	{ "0x52 LD D, D", 4, 4, 1 },
	{ "0x53 LD D, E", 4, 4, 1 },
	{ "0x54 LD D, H", 4, 4, 1 },
	{ "0x55 LD D, L", 4, 4, 1 },
	{ "0x56 LD D, (HL)", 8, 8, 1 },
	{ "0x57 LD D, A", 4, 4, 1 },
	{ "0x58 LD E, B", 4, 4, 1 },
	{ "0x59 LD E, C", 4, 4, 1 },
	{ "0x5A LD E, D", 4, 4, 1 },
	{ "0x5B LD E, E", 4, 4, 1 },
	{ "0x5C LD E, H", 4, 4, 1 },
	{ "0x5D LD E, L", 4, 4, 1 },
	{ "0x5E LD E, (HL)", 8, 8, 1 },
	{ "0x5F LD E, A", 4, 4, 1 },
	{ "0x60 LD H, B", 4, 4, 1 },
	{ "0x61 LD H, C", 4, 4, 1 },
	{ "0x62 LD H, D", 4, 4, 1 },
	{ "0x63 LD H, E", 4, 4, 1 },
	{ "0x64 LD H, H", 4, 4, 1 },
	{ "0x65 LD H, L", 4, 4, 1 },
	{ "0x66 LD H, (HL)", 8, 8, 1 },
    { "0x67 LD H, A", 4, 4, 1 },
	{ "0x68 LD L, B", 4, 4, 1 },
	{ "0x69 LD L, C", 4, 4, 1 },
	{ "0x6A LD L, D", 4, 4, 1 },
	{ "0x6B LD L, E", 4, 4, 1 },
	{ "0x6C LD L, H", 4, 4, 1 },
	{ "0x6D LD L, L", 4, 4, 1 },
	{ "0x6E LD L, (HL)", 8, 8, 1 },
	{ "0x6F LD L, A", 4, 4, 1 },
	{ "0x70 LD (HL), B", 8, 8, 1 },
	{ "0x71 LD (HL), C", 8, 8, 1 },
	{ "0x72 LD (HL), D", 8, 8, 1 },
	{ "0x73 LD (HL), E", 8, 8, 1 },
	{ "0x74 LD (HL), H", 8, 8, 1 },
	{ "0x75 LD (HL), L", 8, 8, 1 },
	{ "0x76 HALT", 4, 4, 1 },
	{ "0x77 LD (HL), A", 8, 8, 1 },
	{ "0x78 LD A, B", 4, 4, 1 },
	{ "0x79 LD A, C", 4, 4, 1 },
	{ "0x7A LD A, D", 4, 4, 1 },
	{ "0x7B LD A, E", 4, 4, 1 },
	{ "0x7C LD A, H", 4, 4, 1 },
	{ "0x7D LD A, L", 4, 4, 1 },
	{ "0x7E LD A, (HL)", 8, 8, 1 },
	{ "0x7F LD A, A", 4, 4, 1 },
	{ "0x80 ADD A, B", 4, 4, 1 },
	{ "0x81 ADD A, C", 4, 4, 1 },
	{ "0x82 ADD A, D", 4, 4, 1 },
	{ "0x83 ADD A, E", 4, 4, 1 },
	{ "0x84 ADD A, H", 4, 4, 1 },
	{ "0x85 ADD A, L", 4, 4, 1 },
	{ "0x86 ADD A, (HL)", 8, 8, 1 },
	{ "0x87 ADD A", 4, 4, 1 },
	{ "0x88 ADC B", 4, 4, 1 },
	{ "0x89 ADC C", 4, 4, 1 },
	{ "0x8A ADC D", 4, 4, 1 },
	{ "0x8B ADC E", 4, 4, 1 },
	{ "0x8C ADC H", 4, 4, 1 },
	{ "0x8D ADC L", 4, 4, 1 },
	{ "0x8E ADC (HL)", 8, 8, 1 },
	{ "0x8F ADC A", 4, 4, 1 },
	{ "0x90 SUB B", 4, 4, 1 },
	{ "0x91 SUB C", 4, 4, 1 },
	{ "0x92 SUB D", 4, 4, 1 },
	{ "0x93 SUB E", 4, 4, 1 },
	{ "0x94 SUB H", 4, 4, 1 },
	{ "0x95 SUB L", 4, 4, 1 },
	{ "0x96 SUB (HL)", 8, 8, 1 },
	{ "0x97 SUB A", 4, 4, 1 },
	{ "0x98 SBC B", 4, 4, 1 },
	{ "0x99 SBC C", 4, 4, 1 },
	{ "0x9A SBC D", 4, 4, 1 },
	{ "0x9B SBC E", 4, 4, 1 },
	{ "0x9C SBC H", 4, 4, 1 },
	{ "0x9D SBC L", 4, 4, 1 },
	{ "0x9E SBC (HL)", 8, 8, 1 },
	{ "0x9F SBC A", 4, 4, 1 },
	{ "0xA0 AND B", 4, 4, 1 },
	{ "0xA1 AND C", 4, 4, 1 },
	{ "0xA2 AND D", 4, 4, 1 },
	{ "0xA3 AND E", 4, 4, 1 },
	{ "0xA4 AND H", 4, 4, 1 },
	{ "0xA5 AND L", 4, 4, 1 },
	{ "0xA6 AND (HL)", 8, 8, 1 },
	{ "0xA7 AND A", 4, 4, 1 },
	{ "0xA8 XOR B", 4, 4, 1 },
	{ "0xA9 XOR C", 4, 4, 1 },
	{ "0xAA XOR D", 4, 4, 1 },
	{ "0xAB XOR E", 4, 4, 1 },
	{ "0xAC XOR H", 4, 4, 1 },
	{ "0xAD XOR L", 4, 4, 1 },
	{ "0xAE XOR (HL)", 8, 8, 1 },
	{ "0xAF XOR A", 4, 4, 1 },
	{ "0xB0 OR B", 4, 4, 1 },
	{ "0xB1 OR C", 4, 4, 1 },
	{ "0xB2 OR D", 4, 4, 1 },
	{ "0xB3 OR E", 4, 4, 1 },
	{ "0xB4 OR H", 4, 4, 1 },
	{ "0xB5 OR L", 4, 4, 1 },
	{ "0xB6 OR (HL)", 8, 8, 1 },
	{ "0xB7 OR A", 4, 4, 1 },
	{ "0xB8 CP B", 4, 4, 1 },
	{ "0xB9 CP C", 4, 4, 1 },
	{ "0xBA CP D", 4, 4, 1 },
	{ "0xBB CP E", 4, 4, 1 },
	{ "0xBC CP H", 4, 4, 1 },
	{ "0xBD CP L", 4, 4, 1 },
	{ "0xBE CP (HL)", 8, 8, 1 },
	{ "0xBF CP A", 4, 4, 1 },
	{ "0xC0 RET NZ", 8, 20, 1 },
	{ "0xC1 POP BC", 12, 12, 1 },
	{ "0xC2 JP NZ, 0x%04X", 12, 16, 3 },
	{ "0xC3 JP 0x%04X", 16, 16, 3 },
	{ "0xC4 CALL NZ, 0x%04X", 12, 24, 3 },
	{ "0xC5 PUSH BC", 16, 16, 1 },
	{ "0xC6 ADD A, 0x%02" PRIX8, 8, 8, 2 },
	{ "0xC7 RST 0x00", 16, 16, 1 },
	{ "0xC8 RET Z", 8, 20, 1 },
	{ "0xC9 RET", 16, 16, 1 },
	{ "0xCA JP Z, 0x%04X", 12, 16, 3 },
	{ "0xCB PREFIX CB", 4, 4, 1 },
	{ "0xCC CALL Z, 0x%04X", 12, 24, 3 },
	{ "0xCD CALL 0x%04X", 24, 24, 3 },
	{ "0xCE ADC 0x%02" PRIX8, 8, 8, 2 },
	{ "0xCF RST 0x08", 16, 16, 1 },
	{ "0xD0 RET NC", 8, 20, 1 },
	{ "0xD1 POP DE", 12, 12, 1 },
	{ "0xD2 JP NC, 0x%04X", 12, 16, 3 },
	{ "0xD3 NONE", 0, 0, 1 },
	{ "0xD4 CALL NC, 0x%04X", 12, 24, 3 },
	{ "0xD5 PUSH DE", 16, 16, 1 },
	{ "0xD6 SUB 0x%02" PRIX8, 8, 8, 2 },
	{ "0xD7 RST 0x10", 16, 16, 1 },
	{ "0xD8 RET C", 8, 20, 1 },
	{ "0xD9 RETI", 16, 16, 1 },
	{ "0xDA JP C, 0x%04X", 12, 16, 3 },
	{ "0xDB NONE", 0, 0, 1 },
	{ "0xDC CALL C, 0x%04X", 12, 24, 3 },
	{ "0xDD NONE", 0, 0, 1 },
	{ "0xDE SBC 0x%02" PRIX8, 8, 8, 2 },
	{ "0xDF RST 0x18", 16, 16, 1 },
	{ "0xE0 LDH (0x%02" PRIX8 "), A", 12, 12, 2 },
	{ "0xE1 POP HL", 12, 12, 1 },
	{ "0xE2 LD (C), A", 8, 8, 2 },
	{ "0xE3 NONE", 0, 0, 1 },
	{ "0xE4 NONE", 0, 0, 1 },
	{ "0xE5 PUSH HL", 16, 16, 1 },
	{ "0xE6 AND 0x%02" PRIX8, 8, 8, 2 },
	{ "0xE7 RST 0x20", 16, 16, 1 },
	{ "0xE8 ADD SP, 0x%02" PRIX8, 16, 16, 2 },
	{ "0xE9 JP (HL)", 4, 4, 1 },
	{ "0xEA LD (0x%02" PRIX8 "), A", 16, 16, 3 },
	{ "0xEB NONE", 0, 0, 1 },
	{ "0xEC NONE", 0, 0, 1 },
	{ "0xED NONE", 0, 0, 1 },
	{ "0xEE XOR 0x%02" PRIX8, 8, 8, 2 },
	{ "0xEF RST 0x28", 16, 16, 1 },
	{ "0xF0 LDH A, (0x%02" PRIX8 ")", 12, 12, 2 },
	{ "0xF1 POP AF", 12, 12, 1 },
	{ "0xF2 LD A, (C)", 8, 8, 2 },
	{ "0xF3 DI", 4, 4, 1 },
	{ "0xF4 NONE", 0, 0, 1 },
	{ "0xF5 PUSH AF", 16, 16, 1 },
	{ "0xF6 OR 0x%02" PRIX8, 8, 8, 2 },
	{ "0xF7 RST 0x30", 16, 16, 1 },
	{ "0xF8 LD HL, SP+0x%02" PRIX8, 12, 12, 2 },
	{ "0xF9 LD SP, HL", 8, 8, 1 },
	{ "0xFA LD A, (0x%04X)", 16, 16, 3 },
	{ "0xFB EI", 4, 4, 1 },
	{ "0xFC NONE", 0, 0, 1 },
	{ "0xFD NONE", 0, 0, 1 },
	{ "0xFE CP 0x%02" PRIX8, 8, 8, 2 },
	{ "0xFF RST 0x38", 16, 16, 1 },
};

const struct cbOpcode cbOpcodes[256] = {
    { "0xCB00 RLC B", 8 },
    { "0xCB01 RLC C", 8 },
    { "0xCB02 RLC D", 8 },
    { "0xCB03 RLC E", 8 },
    { "0xCB04 RLC H", 8 },
    { "0xCB05 RLC L", 8 },
    { "0xCB06 RLC (HL)", 16 },
    { "0xCB07 RLC A", 8 },
    { "0xCB08 RRC B", 8 },
    { "0xCB09 RRC C", 8 },
    { "0xCB0A RRC D", 8 },
    { "0xCB0B RRC E", 8 },
    { "0xCB0C RRC H", 8 },
    { "0xCB0D RRC L", 8 },
    { "0xCB0E RRC (HL)", 16 },
    { "0xCB0F RRC A", 8 },
    { "0xCB10 RL B", 8 },
    { "0xCB11 RL C", 8 },
    { "0xCB12 RL D", 8 },
    { "0xCB13 RL E", 8 },
    { "0xCB14 RL H", 8 },
    { "0xCB15 RL L", 8 },
    { "0xCB16 RL (HL)", 16 },
    { "0xCB17 RL A", 8 },
    { "0xCB18 RR B", 8 },
    { "0xCB19 RR C", 8 },
    { "0xCB1A RR D", 8 },
    { "0xCB1B RR E", 8 },
    { "0xCB1C RR H", 8 },
    { "0xCB1D RR L", 8 },
    { "0xCB1E RR (HL)", 16 },
    { "0xCB1F RR A", 8 },
    { "0xCB20 SLA B", 8 },
    { "0xCB21 SLA C", 8 },
    { "0xCB22 SLA D", 8 },
    { "0xCB23 SLA E", 8 },
    { "0xCB24 SLA H", 8 },
    { "0xCB25 SLA L", 8 },
    { "0xCB26 SLA (HL)", 16 },
    { "0xCB27 SLA A", 8 },
    { "0xCB28 SRA B", 8 },
    { "0xCB29 SRA C", 8 },
    { "0xCB2A SRA D", 8 },
    { "0xCB2B SRA E", 8 },
    { "0xCB2C SRA H", 8 },
    { "0xCB2D SRA L", 8 },
    { "0xCB2E SRA (HL)", 16 },
    { "0xCB2F SRA A", 8 },
    { "0xCB30 SWAP B", 8 },
    { "0xCB31 SWAP C", 8 },
    { "0xCB32 SWAP D", 8 },
    { "0xCB33 SWAP E", 8 },
    { "0xCB34 SWAP H", 8 },
    { "0xCB35 SWAP L", 8 },
    { "0xCB36 SWAP (HL)", 16 },
    { "0xCB37 SWAP A", 8 },
    { "0xCB38 SRL B", 8 },
    { "0xCB39 SRL C", 8 },
    { "0xCB3A SRL D", 8 },
    { "0xCB3B SRL E", 8 },
    { "0xCB3C SRL H", 8 },
    { "0xCB3D SRL L", 8 },
    { "0xCB3E SRL (HL)", 16 },
    { "0xCB3F SRL A", 8 },
    { "0xCB40 BIT 0, B", 8 },
    { "0xCB41 BIT 0, C", 8 },
    { "0xCB42 BIT 0, D", 8 },
    { "0xCB43 BIT 0, E", 8 },
    { "0xCB44 BIT 0, H", 8 },
    { "0xCB45 BIT 0, L", 8 },
    { "0xCB46 BIT 0, (HL)", 16 },
    { "0xCB47 BIT 0, A", 8 },
    { "0xCB48 BIT 1, B", 8 },
    { "0xCB49 BIT 1, C", 8 },
    { "0xCB4A BIT 1, D", 8 },
    { "0xCB4B BIT 1, E", 8 },
    { "0xCB4C BIT 1, H", 8 },
    { "0xCB4D BIT 1, L", 8 },
    { "0xCB4E BIT 1, (HL)", 16 },
    { "0xCB4F BIT 1, A", 8 },
    { "0xCB50 BIT 2, B", 8 },
    { "0xCB51 BIT 2, C", 8 },
    { "0xCB52 BIT 2, D", 8 },
    { "0xCB53 BIT 2, E", 8 },
    { "0xCB54 BIT 2, H", 8 },
    { "0xCB55 BIT 2, L", 8 },
    { "0xCB56 BIT 2, (HL)", 16 },
    { "0xCB57 BIT 2, A", 8 },
    { "0xCB58 BIT 3, B", 8 },
    { "0xCB59 BIT 3, C", 8 },
    { "0xCB5A BIT 3, D", 8 },
    { "0xCB5B BIT 3, E", 8 },
    { "0xCB5C BIT 3, H", 8 },
    { "0xCB5D BIT 3, L", 8 },
    { "0xCB5E BIT 3, (HL)", 16 },
    { "0xCB5F BIT 3, A", 8 },
    { "0xCB60 BIT 4, B", 8 },
    { "0xCB61 BIT 4, C", 8 },
    { "0xCB62 BIT 4, D", 8 },
    { "0xCB63 BIT 4, E", 8 },
    { "0xCB64 BIT 4, H", 8 },
    { "0xCB65 BIT 4, L", 8 },
    { "0xCB66 BIT 4, (HL)", 16 },
    { "0xCB67 BIT 4, A", 8 },
    { "0xCB68 BIT 5, B", 8 },
    { "0xCB69 BIT 5, C", 8 },
    { "0xCB6A BIT 5, D", 8 },
    { "0xCB6B BIT 5, E", 8 },
    { "0xCB6C BIT 5, H", 8 },
    { "0xCB6D BIT 5, L", 8 },
    { "0xCB6E BIT 5, (HL)", 16 },
    { "0xCB6F BIT 5, A", 8 },
    { "0xCB70 BIT 6, B", 8 },
    { "0xCB71 BIT 6, C", 8 },
    { "0xCB72 BIT 6, D", 8 },
    { "0xCB73 BIT 6, E", 8 },
    { "0xCB74 BIT 6, H", 8 },
    { "0xCB75 BIT 6, L", 8 },
    { "0xCB76 BIT 6, (HL)", 16 },
    { "0xCB77 BIT 6, A", 8 },
    { "0xCB78 BIT 7, B", 8 },
    { "0xCB79 BIT 7, C", 8 },
    { "0xCB7A BIT 7, D", 8 },
    { "0xCB7B BIT 7, E", 8 },
    { "0xCB7C BIT 7, H", 8 },
    { "0xCB7D BIT 7, L", 8 },
    { "0xCB7E BIT 7, (HL)", 16 },
    { "0xCB7F BIT 7, A", 8 },
    { "0xCB80 RES 0, B", 8 },
    { "0xCB81 RES 0, C", 8 },
    { "0xCB82 RES 0, D", 8 },
    { "0xCB83 RES 0, E", 8 },
    { "0xCB84 RES 0, H", 8 },
    { "0xCB85 RES 0, L", 8 },
    { "0xCB86 RES 0, (HL)", 16 },
    { "0xCB87 RES 0, A", 8 },
    { "0xCB88 RES 1, B", 8 },
    { "0xCB89 RES 1, C", 8 },
    { "0xCB8A RES 1, D", 8 },
    { "0xCB8B RES 1, E", 8 },
    { "0xCB8C RES 1, H", 8 },
    { "0xCB8D RES 1, L", 8 },
    { "0xCB8E RES 1, (HL)", 16 },
    { "0xCB8F RES 1, A", 8 },
    { "0xCB90 RES 2, B", 8 },
    { "0xCB91 RES 2, C", 8 },
    { "0xCB92 RES 2, D", 8 },
    { "0xCB93 RES 2, E", 8 },
    { "0xCB94 RES 2, H", 8 },
    { "0xCB95 RES 2, L", 8 },
    { "0xCB96 RES 2, (HL)", 16 },
    { "0xCB97 RES 2, A", 8 },
    { "0xCB98 RES 3, B", 8 },
    { "0xCB99 RES 3, C", 8 },
    { "0xCB9A RES 3, D", 8 },
    { "0xCB9B RES 3, E", 8 },
    { "0xCB9C RES 3, H", 8 },
    { "0xCB9D RES 3, L", 8 },
    { "0xCB9E RES 3, (HL)", 16 },
    { "0xCB9F RES 3, A", 8 },
    { "0xCBA0 RES 4, B", 8 },
    { "0xCBA1 RES 4, C", 8 },
    { "0xCBA2 RES 4, D", 8 },
    { "0xCBA3 RES 4, E", 8 },
    { "0xCBA4 RES 4, H", 8 },
    { "0xCBA5 RES 4, L", 8 },
    { "0xCBA6 RES 4, (HL)", 16 },
    { "0xCBA7 RES 4, A", 8 },
    { "0xCBA8 RES 5, B", 8 },
    { "0xCBA9 RES 5, C", 8 },
    { "0xCBAA RES 5, D", 8 },
    { "0xCBAB RES 5, E", 8 },
    { "0xCBAC RES 5, H", 8 },
    { "0xCBAD RES 5, L", 8 },
    { "0xCBAE RES 5, (HL)", 16 },
    { "0xCBAF RES 5, A", 8 },
    { "0xCBB0 RES 6, B", 8 },
    { "0xCBB1 RES 6, C", 8 },
    { "0xCBB2 RES 6, D", 8 },
    { "0xCBB3 RES 6, E", 8 },
    { "0xCBB4 RES 6, H", 8 },
    { "0xCBB5 RES 6, L", 8 },
    { "0xCBB6 RES 6, (HL)", 16 },
    { "0xCBB7 RES 6, A", 8 },
    { "0xCBB8 RES 7, B", 8 },
    { "0xCBB9 RES 7, C", 8 },
    { "0xCBBA RES 7, D", 8 },
    { "0xCBBB RES 7, E", 8 },
    { "0xCBBC RES 7, H", 8 },
    { "0xCBBD RES 7, L", 8 },
    { "0xCBBE RES 7, (HL)", 16 },
    { "0xCBBF RES 7, A", 8 },
    { "0xCBC0 SET 0, B", 8 },
    { "0xCBC1 SET 0, C", 8 },
    { "0xCBC2 SET 0, D", 8 },
    { "0xCBC3 SET 0, E", 8 },
    { "0xCBC4 SET 0, H", 8 },
    { "0xCBC5 SET 0, L", 8 },
    { "0xCBC6 SET 0, (HL)", 16 },
    { "0xCBC7 SET 0, A", 8 },
    { "0xCBC8 SET 1, B", 8 },
    { "0xCBC9 SET 1, C", 8 },
    { "0xCBCA SET 1, D", 8 },
    { "0xCBCB SET 1, E", 8 },
    { "0xCBCC SET 1, H", 8 },
    { "0xCBCD SET 1, L", 8 },
    { "0xCBCE SET 1, (HL)", 16 },
    { "0xCBCF SET 1, A", 8 },
    { "0xCBD0 SET 2, B", 8 },
    { "0xCBD1 SET 2, C", 8 },
    { "0xCBD2 SET 2, D", 8 },
    { "0xCBD3 SET 2, E", 8 },
    { "0xCBD4 SET 2, H", 8 },
    { "0xCBD5 SET 2, L", 8 },
    { "0xCBD6 SET 2, (HL)", 16 },
    { "0xCBD7 SET 2, A", 8 },
    { "0xCBD8 SET 3, B", 8 },
    { "0xCBD9 SET 3, C", 8 },
    { "0xCBDA SET 3, D", 8 },
    { "0xCBDB SET 3, E", 8 },
    { "0xCBDC SET 3, H", 8 },
    { "0xCBDD SET 3, L", 8 },
    { "0xCBDE SET 3, (HL)", 16 },
    { "0xCBDF SET 3, A", 8 },
    { "0xCBE0 SET 4, B", 8 },
    { "0xCBE1 SET 4, C", 8 },
    { "0xCBE2 SET 4, D", 8 },
    { "0xCBE3 SET 4, E", 8 },
    { "0xCBE4 SET 4, H", 8 },
    { "0xCBE5 SET 4, L", 8 },
    { "0xCBE6 SET 4, (HL)", 16 },
    { "0xCBE7 SET 4, A", 8 },
    { "0xCBE8 SET 5, B", 8 },
    { "0xCBE9 SET 5, C", 8 },
    { "0xCBEA SET 5, D", 8 },
    { "0xCBEB SET 5, E", 8 },
    { "0xCBEC SET 5, H", 8 },
    { "0xCBED SET 5, L", 8 },
    { "0xCBEE SET 5, (HL)", 16 },
    { "0xCBEF SET 5, A", 8 },
    { "0xCBF0 SET 6, B", 8 },
    { "0xCBF1 SET 6, C", 8 },
    { "0xCBF2 SET 6, D", 8 },
    { "0xCBF3 SET 6, E", 8 },
    { "0xCBF4 SET 6, H", 8 },
    { "0xCBF5 SET 6, L", 8 },
    { "0xCBF6 SET 6, (HL)", 16 },
    { "0xCBF7 SET 6, A", 8 },
    { "0xCBF8 SET 7, B", 8 },
    { "0xCBF9 SET 7, C", 8 },
    { "0xCBFA SET 7, D", 8 },
    { "0xCBFB SET 7, E", 8 },
    { "0xCBFC SET 7, H", 8 },
    { "0xCBFD SET 7, L", 8 },
    { "0xCBFE SET 7, (HL)", 16 },
    { "0xCBFF SET 7, A", 8 }
};

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
#include "opcodes.c"
#include "cpu.c"

#ifdef DISPLAY
    #include "display.h"
    #include "display.c"
#endif

#include "interrupts.c"
#include "screen.c"

#endif /* MAIN_H */
