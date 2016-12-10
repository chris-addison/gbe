/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "../types.h"
#include "../common.h"
#include "../cpu.h"
#include "opcodes.h"
#include "cb_opcodes.c"

//store format to printf each instruction, it's number of cycles, and how many bytes the instruction is in memory.
struct opcode opcodes[256] = {
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

struct cbOpcode cbOpcodes[256] = {
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

//read one unsigned byte from the PC
static uint8 oneByte(cpu_state *cpu) {
    return readNextByte(cpu);
}

//read one signed byte from the PC
static int8 oneByteSigned(cpu_state *cpu) {
    return (int8)readNextByte(cpu);
}

//read a short from memory
static uint16 twoBytes(cpu_state *cpu) {
    uint16 lsb = (uint16)readNextByte(cpu); //read least significant byte first
    uint16 msb = ((uint16)readNextByte(cpu)) << 8; //then read most significant byte
    return lsb | msb;
}

//Halt the cpu until there is a interrupt
static void halt(uint8 opcode, cpu_state *cpu) {
    cpu->halt = true;
    cpu->wait = opcodes[opcode].cycles;
    //debug(true, cpu);
}

//compare instruction
static void cp(uint8 b, uint8 opcode, cpu_state *cpu) {
    uint8 a = cpu->registers.A;
    //set flags
    (a - b == 0) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    //cpu->FLAGS[ZF] = (temp == 0); //zero flag
    setFlag(NF, cpu);
    //cpu->FLAGS[NF] = 0x1; //subtract flag
    ((a & 0xF) < (b & 0xF)) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //cpu->FLAGS[HF] = ((a & 0xF) < (b & 0xF)); //half-carry flag
    (a < b) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //cpu->FLAGS[CF] = (a < b); //carry flag
    cpu->wait = opcodes[opcode].cycles;
}

//set the carry flag
static void scf(uint8 opcode, cpu_state *cpu) {
    //set carry flag
    setFlag(CF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //leave zero flag alone
    cpu->wait = opcodes[opcode].cycles;
}

//complement the carry flag
static void ccf(uint8 opcode, cpu_state *cpu) {
    //complement the state of the carry flag.
    (readFlag(CF, cpu)) ? clearFlag(CF, cpu) : setFlag(CF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //leave zero flag alone
    cpu->wait = opcodes[opcode].cycles;
}

//jump to a 16bit address if condition is set
static void jp_c(bool set, uint16 address, uint8 opcode, cpu_state *cpu) {
    if (set) {
        cpu->PC = address;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//jump relative to PC if condition is met
static void jr_c(bool set, int8 address, uint8 opcode, cpu_state *cpu) {
    if (set) {
        cpu->PC += address;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//load 8 bit value into some register
static void ld_8(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory
static void ld_8_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and increment the value in the HL register
static void ldi(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->registers.HL++;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and increment the value in the HL register
static void ldi_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->registers.HL++;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some register and decrement the value in the HL register
static void ldd(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->registers.HL--;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and decrement the value in the HL register
static void ldd_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->registers.HL--;
    cpu->wait = opcodes[opcode].cycles;
}

//load 16 bit value into some destination register
static void ld_16(uint16 value, uint16 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->wait = opcodes[opcode].cycles;
}

//load 16 bit value into some addres in memory
static void ld_16_m(uint16 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeShort(address, value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//increment a byte in a register
static void inc_8(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether result is zero
    ((uint8)(*reg + 1)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //set half-carry flag based on result
    ((*reg & 0xF) == 0xF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    *reg += 1;
    cpu->wait = opcodes[opcode].cycles;
}

//increment a byte at the memory location stored in HL
static void inc_8_m(uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether result is zero
    ((uint8)(readByte(cpu->registers.HL, cpu) + 1)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //set or reset half-carry flag based on result
    ((readByte(cpu->registers.HL, cpu) & 0xF) == 0xF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //no change to carry flag
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) + 1, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//increment a short
static void inc_16(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //inc_16 doesn't set or clear any flags
    *reg += 1; //can't use "++" as it takes higher priority over pointer dereference
    cpu->wait = opcodes[opcode].cycles;
}

//decrement a byte in memory
static void dec_8(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //zero flag
    (*reg - 1) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //negative flag
    setFlag(NF, cpu);
    //half-carry flag
    (*reg & 0xF) ? clearFlag(HF, cpu) : setFlag(HF, cpu);
    *reg -= 1;
    cpu->wait = opcodes[opcode].cycles;
}

//decrement a byte at the memory location stored in HL
static void dec_8_m(uint8 opcode, cpu_state *cpu) {
    //zero flag
    (readByte(cpu->registers.HL, cpu) - 1) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //negative flag
    setFlag(NF, cpu);
    //half-carry flag
    (readByte(cpu->registers.HL, cpu) & 0xF) ? clearFlag(HF, cpu) : setFlag(HF, cpu);
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) - 1, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//decrement a short
static void dec_16(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //dec_16 doesn't set or clear any flags
    *reg -= 1; //can't use "-- as it takes higher priority over pointer dereference
    cpu->wait = opcodes[opcode].cycles;
}

//add together some 8 bit unsigned value and the A register
static void add_8(uint8 value, uint8 opcode, cpu_state *cpu) {
    //zero flag
    ((uint8)(cpu->registers.A + value)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //half-carry flag
    ((cpu->registers.A & 0xF) + (value & 0xF) > 0xF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //carry flag
    (((uint16)cpu->registers.A) + ((uint16)value) > 0xFF) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //set value after flags are calculated
    cpu->registers.A += value;
    cpu->wait = opcodes[opcode].cycles;
}

//subtract an unsigned 8 bit value from the A register
static void sub_8(uint8 value, uint8 opcode, cpu_state *cpu) {
    //zero flag
    (cpu->registers.A == value) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    //set negative flag
    setFlag(NF, cpu);
    //set or reset half-carry flag based on result
    ((cpu->registers.A & 0xF) < (value & 0xF)) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //set or reset carry flag based on result
    (cpu->registers.A < value) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //set value after flags are calculated
    cpu->registers.A -= value;
    cpu->wait = opcodes[opcode].cycles;
}

//add together two unsigned 16 bit values and set flags
static void add_16(uint16 value, uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //zero flag isn't touched
    //reset negative flag
    clearFlag(NF, cpu);
    //set or reset half-carry flag based on result (in 16bit ALU the highest bytes set the CF last, so only check for the high byte 3 -> 4 bit carry)
    ((*reg & 0xFFF) + (value & 0xFFF) > 0xFFF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //set or reset carry flag based on result
    (((uint32)*reg) + ((uint32)value) > 0xFFFF) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //set value after flags are calculated
    *reg += value;
    cpu->wait = opcodes[opcode].cycles;
}

//8 bit add between the A register, some value, and the carry flag
static void adc(uint8 value, uint8 opcode, cpu_state *cpu) {
    add_8(value + readFlag(CF, cpu), opcode, cpu);
}

//8 bit subtract between the A register, some value, and the carry flag
static void sbc(uint8 value, uint8 opcode, cpu_state *cpu) {
    sub_8(value + readFlag(CF, cpu), opcode, cpu);
}

//rotate register A left, old bit 7 to carry bit and bit 0
static void rlca(uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (cpu->registers.A >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    cpu->registers.A = (cpu->registers.A << 1) | readFlag(CF, cpu);
    //reset zero flag
    clearFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//rotate the A register left, old bit 7 to carry bit and old carry bit to bit 0
static void rla(uint8 opcode, cpu_state *cpu) {
    //read carry flag state into temp variable
    bool flagState = readFlag(CF, cpu);
    //update the carry flag
    (cpu->registers.A >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    cpu->registers.A <<= 1;
    //insert previous flag state into bit 0
    cpu->registers.A |= flagState;
    //clear zero flag
    clearFlag(ZF, cpu);
    //clear half-carry flag
    clearFlag(HF, cpu);
    //clear negative flag
    clearFlag(NF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//rotate register A left, old bit 0 to carry bit and bit 7
static void rrca(uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (cpu->registers.A & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    cpu->registers.A = (cpu->registers.A >> 1) | (((uint8) readFlag(CF, cpu)) << 7);
    //reset zero flag
    clearFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//right rotate the A register. New 7th bit is set by the carry flag and the carry flag is set by old 1st bit.
static void rra(uint8 opcode, cpu_state *cpu) {
    //read carry flag state into temp variable
    bool flagState = readFlag(CF, cpu);
    //update the carry flag
    (cpu->registers.A & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    cpu->registers.A = cpu->registers.A >> 1;
    //insert previous flag state into bit 7
    cpu->registers.A |= flagState << 7;
    //reset zero flag
    clearFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//complement the A register
static void cpl(uint8 opcode, cpu_state *cpu) {
    //set negative flag
    setFlag(NF, cpu);
    //set half-carry flag
    setFlag(HF, cpu);
    //no change to zero flag and carry flag
    cpu->registers.A = ~cpu->registers.A;
    cpu->wait = opcodes[opcode].cycles;
}

//push a short onto the stack
static void push(uint16 value, uint8 opcode, cpu_state *cpu) {
    writeShortToStack(value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//pop a short from the stack
static void pop(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = readShortFromStack(cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//standard call. Save PC to the stack
static void call_c(bool set, uint16 pointer, uint8 opcode, cpu_state *cpu) {
    if (set) {
        //write PC to stack
        writeShortToStack(cpu->PC, cpu);
        //change to new PC
        cpu->PC = pointer;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //condition not met: do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//return after call. Restore PC from stack
static void ret_c(bool set, uint8 opcode, cpu_state *cpu) {
    if (set) {
        //restore PC from stack
        cpu->PC = readShortFromStack(cpu);
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //condition not met: do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//return after call and enable interrupts
static void reti(uint8 opcode, cpu_state *cpu) {
    cpu->PC = readShortFromStack(cpu);
    cpu->wait = opcodes[opcode].cycles;
    cpu->ime = true;
}

//enable interrupts after this instruction and the next one are finished
static void ei(uint8 opcode, cpu_state *cpu) {
    //make sure interrupts aren't enabled and aren't changing
    if (!cpu->ime && !cpu->imeCounter) {
        cpu->imeCounter = 2;
    }
    cpu->wait = opcodes[opcode].cycles;
}

//disable interrupts after this instruction and the next one are finished
static void di(uint8 opcode, cpu_state *cpu) {
    //make sure interrupts are enabled and not changing
    if (cpu->ime && !cpu->imeCounter) {
        cpu->imeCounter = 2;
    }
    cpu->wait = opcodes[opcode].cycles;
}

//restart at given address. Save previous PC to the stack
static void rst(uint8 pc, uint8 opcode, cpu_state *cpu) {
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = (uint16)pc;
    cpu->wait = opcodes[opcode].cycles;
}

//xor A register with given value and set flags
static void xor(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A ^= value;
    //set or reset zero flag based on whether result is zero
    (cpu->registers.A) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset carry flag
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//perform logical and on the A register with a given value and set values
static void and(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A &= value;
    //set or reset zero flag based on whether result is zero
    (!cpu->registers.A) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //set carry flag
    setFlag(HF, cpu);
    //reset carry flag
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//perform logical or on the A register with a given value and set flags
static void or(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A |= value;
    //set or reset zero flag based on whether result is zero
    (!cpu->registers.A) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset carry flag
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//decimal adjust register A. Hex to Binary Coded Decimal. This makes the max
//value of every byte 9 to allow for easy translation into decimal values.
static void daa(uint8 opcode, cpu_state *cpu) {
    uint16 regA = cpu->registers.A;
    //check whether the last operation was subtraction
    if (!readFlag(NF, cpu)) { //addition
        //correct lower byte. If value is greater than 9, push the excess to the
        //next byte. If half-carry flag set, restore the lost value. essentually,
        //half carry happens when the byte has a vlue greater than 15. 16 is then
        //carried over to the next byte. in BCD this is only a carry of 10, so
        //to correct one must restore the excess 6 to the lower byte.
        if (readFlag(HF, cpu) || ((cpu->registers.A & 0xF) > 0x09)) {
            regA += 0x06;
        }
        //correct upper byte. Similar to above regarding the situation with the
        //half-carry flag except in the larger byte this is the carry flag instead
        if (readFlag(CF, cpu) || (cpu->registers.A > 0x9F)) {
            regA += 0x60;
        }
    } else { //subtraction
        //correct lower byte. Same idea as explained above, but in reverse
        if (readFlag(HF, cpu)) {
            regA = (regA - 0x06) & 0xFF;
        }
        //correct upper byte. Same idea as explained above, but in reverse
        if (readFlag(CF, cpu)) {
            regA -= 0x60;
        }
    }
    // Assign back to register after masking value
    cpu->registers.A = regA & 0xFF;
    //carry flag - can only be set, not reset
    if (regA & 0x100) {
        setFlag(CF, cpu);
    }
    //half-carry flag
    clearFlag(HF, cpu);
    //zero flag
    (cpu->registers.A) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//execute next instruction
int execute(cpu_state * cpu) {
    //don't execute if halt is called
    if (cpu->halt) {
        return 0;
    }
    //printInstruction(cpu->PC, cpu);
    //grab instruction
    uint8 opcode = readNextByte(cpu);
    //find and execute next instruction
    switch (opcode) {
        case 0x00: //NOP
            cpu->wait = 4;
            break;
        case 0x01: //LD BC, d16
            ld_16(twoBytes(cpu), &cpu->registers.BC, opcode, cpu);
            break;
        case 0x02: //LD (BC), A
            ld_8_m(cpu->registers.A, cpu->registers.BC, opcode, cpu);
            break;
        case 0x03: //INC BC
            inc_16(&cpu->registers.BC, opcode, cpu);
            break;
        case 0x04: //INC B
            inc_8(&cpu->registers.B, opcode, cpu);
            break;
        case 0x05: //DEC B
            dec_8(&cpu->registers.B, opcode, cpu);
            break;
        case 0x06: //LD B, d8
            ld_8(oneByte(cpu), &cpu->registers.B, opcode, cpu);
            break;
        case 0x07: //RLCA
            rlca(opcode, cpu);
            break;
        case 0x08: //LD (a16), SP
            ld_16_m(cpu->SP, twoBytes(cpu), opcode, cpu);
            break;
        case 0x09: //ADD HL, BC
            add_16(cpu->registers.BC, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x0A: //LD A, (BC)
            ld_8(readByte(cpu->registers.BC, cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x0B: //DEC BC
            dec_16(&cpu->registers.BC, opcode, cpu);
            break;
        case 0x0C: //INC C
            inc_8(&cpu->registers.C, opcode, cpu);
            break;
        case 0x0D: //DEC C
            dec_8(&cpu->registers.C, opcode, cpu);
            break;
        case 0x0E: //LD C, d8
            ld_8(oneByte(cpu), &cpu->registers.C, opcode, cpu);
            break;
        case 0x0F: //RRCA
            rrca(opcode, cpu);
            break;
        case 0x11: //LD DE, d16
            ld_16(twoBytes(cpu), &cpu->registers.DE, opcode, cpu);
            break;
        case 0x12: //LD (DE), A
            ld_8_m(cpu->registers.A, cpu->registers.DE, opcode, cpu);
            break;
        case 0x13: //INC DE
            inc_16(&cpu->registers.DE, opcode, cpu);
            break;
        case 0x14: //INC D
            inc_8(&cpu->registers.D, opcode, cpu);
            break;
        case 0x15: //DEC D
            dec_8(&cpu->registers.D, opcode, cpu);
            break;
        case 0x16: //LD D, d8
            ld_8(oneByte(cpu), &cpu->registers.D, opcode, cpu);
            break;
        case 0x17: //RLA
            rla(opcode, cpu);
            break;
        case 0x18: //JR r8
            jr_c(true, oneByteSigned(cpu), opcode, cpu);
            break;
        case 0x19: //ADD HL, DE
            add_16(cpu->registers.DE, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x1A: //LD A, (DE)
            ld_8(readByte(cpu->registers.DE, cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x1B: //DEC DE
            dec_16(&cpu->registers.DE, opcode, cpu);
            break;
        case 0x1C: //INC E
            inc_8(&cpu->registers.E, opcode, cpu);
            break;
        case 0x1D: //DEC E
            dec_8(&cpu->registers.E, opcode, cpu);
            break;
        case 0x1E: //LD E, d8
            ld_8(oneByte(cpu), &cpu->registers.E, opcode, cpu);
            break;
        case 0x1F: //RRA
            rra(opcode, cpu);
            break;
        case 0x20: //JR NZ, r8
            jr_c(!readFlag(ZF, cpu), oneByteSigned(cpu), opcode, cpu);
            break;
        case 0x21: //LD HL, d16
            ld_16(twoBytes(cpu), &cpu->registers.HL, opcode, cpu);
            break;
        case 0x22: //LDI (HL), A
            ldi_m(cpu->registers.A, cpu->registers.HL, opcode, cpu);
            break;
        case 0x23: //INC HL
            inc_16(&cpu->registers.HL, opcode, cpu);
            break;
        case 0x24: //INC H
            inc_8(&cpu->registers.H, opcode, cpu);
            break;
        case 0x25: //DEC H
            dec_8(&cpu->registers.H, opcode, cpu);
            break;
        case 0x26: //LD H, d8
            ld_8(oneByte(cpu), &cpu->registers.H, opcode, cpu);
            break;
        case 0x27: //DAA
            daa(opcode, cpu);
            break;
        case 0x28: //JR Z,r8
            jr_c(readFlag(ZF, cpu), oneByteSigned(cpu), opcode, cpu);
            break;
        case 0x29: //ADD HL, HL
            add_16(cpu->registers.HL, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x2A: //LDI A, (HL)
            ldi(readByte(cpu->registers.HL, cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x2B: //DEC HL
            dec_16(&cpu->registers.HL, opcode, cpu);
            break;
        case 0x2C: //INC L
            inc_8(&cpu->registers.L, opcode, cpu);
            break;
        case 0x2D: //DEC L
            dec_8(&cpu->registers.L, opcode, cpu);
            break;
        case 0x2F: //CPL
            cpl(opcode, cpu);
            break;
        case 0x30: //JR NC, r8
            jr_c(!readFlag(CF, cpu), oneByteSigned(cpu), opcode, cpu);
            break;
        case 0x31: //LD SP, d16
            ld_16(twoBytes(cpu), &cpu->SP, opcode, cpu);
            break;
        case 0x32: //LDD (HL), A
            ldd_m(cpu->registers.A, cpu->registers.HL, opcode, cpu);
            break;
        case 0x34: //INC (HL)
            inc_8_m(opcode, cpu);
            break;
        case 0x35: //DEC (HL)
            dec_8_m(opcode, cpu);
            break;
        case 0x36: //LD (HL), d8
            ld_8_m(oneByte(cpu), cpu->registers.HL, opcode, cpu);
            break;
        case 0x37: //SCF
            scf(opcode, cpu);
            break;
        case 0x38: //JR C, r8
            jr_c(readFlag(CF, cpu), oneByteSigned(cpu), opcode, cpu);
            break;
        case 0x39: //ADD HL, SP
            add_16(cpu->SP, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x3A: //LDD A, (HL)
            ldd(readByte(cpu->registers.HL, cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x3C: //INC A
            inc_8(&cpu->registers.A, opcode, cpu);
            break;
        case 0x3D: //DEC A
            dec_8(&cpu->registers.A, opcode, cpu);
            break;
        case 0x3E: //LD A, d8
            ld_8(oneByte(cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x3F: //CCF
            ccf(opcode, cpu);
            break;
        case 0x40: //LD B, B
            ld_8(cpu->registers.B, &cpu->registers.B, opcode, cpu);
            break;
        case 0x41: //LD B, C
            ld_8(cpu->registers.C, &cpu->registers.B, opcode, cpu);
            break;
        case 0x42: //LD B, D
            ld_8(cpu->registers.D, &cpu->registers.B, opcode, cpu);
            break;
        case 0x43: //LD B, E
            ld_8(cpu->registers.E, &cpu->registers.B, opcode, cpu);
            break;
        case 0x44: //LD B, H
            ld_8(cpu->registers.H, &cpu->registers.B, opcode, cpu);
            break;
        case 0x45: //LD B, L
            ld_8(cpu->registers.L, &cpu->registers.B, opcode, cpu);
            break;
        case 0x46: //LD B, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.B, opcode, cpu);
            break;
        case 0x47: //LD B, A
            ld_8(cpu->registers.A, &cpu->registers.B, opcode, cpu);
            break;
        case 0x48: //LD C, B
            ld_8(cpu->registers.B, &cpu->registers.C, opcode, cpu);
            break;
        case 0x4A: //lD C, D
            ld_8(cpu->registers.D, &cpu->registers.C, opcode, cpu);
            break;
        case 0x4D: //LD C, L
            ld_8(cpu->registers.L, &cpu->registers.C, opcode, cpu);
            break;
        case 0x4E: //LD C, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.C, opcode, cpu);
            break;
        case 0x4F: //LD C, A
            ld_8(cpu->registers.A, &cpu->registers.C, opcode, cpu);
            break;
        case 0x50: //LD D, B
            ld_8(cpu->registers.B, &cpu->registers.D, opcode, cpu);
            break;
        case 0x51: //LD D, C
            ld_8(cpu->registers.C, &cpu->registers.D, opcode, cpu);
            break;
        case 0x52: //LD D, D
            ld_8(cpu->registers.D, &cpu->registers.D, opcode, cpu);
            break;
        case 0x53: //LD D, E
            ld_8(cpu->registers.E, &cpu->registers.D, opcode, cpu);
            break;
        case 0x54: //LD D, H
            ld_8(cpu->registers.H, &cpu->registers.D, opcode, cpu);
            break;
        case 0x55: //LD D, L
            ld_8(cpu->registers.L, &cpu->registers.D, opcode, cpu);
            break;
        case 0x56: //LD D, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.D, opcode, cpu);
            break;
        case 0x57: //LD D, A
            ld_8(cpu->registers.A, &cpu->registers.D, opcode, cpu);
            break;
        case 0x58: //LD E, B
            ld_8(cpu->registers.B, &cpu->registers.E, opcode, cpu);
            break;
        case 0x59: //LD E, C
            ld_8(cpu->registers.C, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5A: //LD E, D
            ld_8(cpu->registers.D, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5B: //LD E, E
            ld_8(cpu->registers.E, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5C: //lD E, H
            ld_8(cpu->registers.H, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5D: //LD E, L
            ld_8(cpu->registers.L, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5E: //LD E, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.E, opcode, cpu);
            break;
        case 0x5F: //LD E, A
            ld_8(cpu->registers.A, &cpu->registers.E, opcode, cpu);
            break;
        case 0x60: //LD H, B
            ld_8(cpu->registers.B, &cpu->registers.H, opcode, cpu);
            break;
        case 0x61: //LD H, C
            ld_8(cpu->registers.C, &cpu->registers.H, opcode, cpu);
            break;
        case 0x62: //LD H, D
            ld_8(cpu->registers.D, &cpu->registers.H, opcode, cpu);
            break;
        case 0x63: //LD H, E
            ld_8(cpu->registers.E, &cpu->registers.H, opcode, cpu);
            break;
        case 0x64: //LD H, H
            ld_8(cpu->registers.H, &cpu->registers.H, opcode, cpu);
            break;
        case 0x65: //LD H, L
            ld_8(cpu->registers.L, &cpu->registers.H, opcode, cpu);
            break;
        case 0x66: //LD H, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.H, opcode, cpu);
            break;
        case 0x67: //LD H, A
            ld_8(cpu->registers.A, &cpu->registers.H, opcode, cpu);
            break;
        case 0x68: //LD L, B
            ld_8(cpu->registers.B, &cpu->registers.L, opcode, cpu);
            break;
        case 0x69: //LD L, C
            ld_8(cpu->registers.C, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6A: //LD L, D
            ld_8(cpu->registers.D, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6B: //LD L, E
            ld_8(cpu->registers.E, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6C: //LD L, H
            ld_8(cpu->registers.H, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6D: //LD L, L
            ld_8(cpu->registers.L, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6E: //LD L, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.L, opcode, cpu);
            break;
        case 0x6F: //LD L, A
            ld_8(cpu->registers.A, &cpu->registers.L, opcode, cpu);
            break;
        case 0x70: //LD (HL), B
            ld_8_m(cpu->registers.B, cpu->registers.HL, opcode, cpu);
            break;
        case 0x71: //LD (HL), C
            ld_8_m(cpu->registers.C, cpu->registers.HL, opcode, cpu);
            break;
        case 0x72: //LD (HL), D
            ld_8_m(cpu->registers.D, cpu->registers.HL, opcode, cpu);
            break;
        case 0x73: //LD (HL), E
            ld_8_m(cpu->registers.E, cpu->registers.HL, opcode, cpu);
            break;
        case 0x76: //HALT
            halt(opcode, cpu);
            break;
        case 0x77: //LD (HL), A
            ld_8_m(cpu->registers.A, cpu->registers.HL, opcode, cpu);
            break;
        case 0x78: //LD A, B
            ld_8(cpu->registers.B, &cpu->registers.A, opcode, cpu);
            break;
        case 0x79: //LD A, C
            ld_8(cpu->registers.C, &cpu->registers.A, opcode, cpu);
            break;
        case 0x7A: //LD A, D
            ld_8(cpu->registers.D, &cpu->registers.A, opcode, cpu);
            break;
        case 0x7B: //LD A, E
            ld_8(cpu->registers.E, &cpu->registers.A, opcode, cpu);
            break;
        case 0x7C: //LD A, H
            ld_8(cpu->registers.H, &cpu->registers.A, opcode, cpu);
            break;
        case 0x7D: //LD A, L
            ld_8(cpu->registers.L, &cpu->registers.A, opcode, cpu);
            break;
        case 0x7E: //LD A, (HL)
            ld_8(readByte(cpu->registers.HL, cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0x7F: //LD A, A
            ld_8(cpu->registers.A, &cpu->registers.A, opcode, cpu);
            break;
        case 0x80: //ADD B
            add_8(cpu->registers.B, opcode, cpu);
            break;
        case 0x81: //ADD C
            add_8(cpu->registers.C, opcode, cpu);
            break;
        case 0x82: //ADD D
            add_8(cpu->registers.D, opcode, cpu);
            break;
        case 0x83: //ADD E
            add_8(cpu->registers.E, opcode, cpu);
            break;
        case 0x84: //ADD H
            add_8(cpu->registers.H, opcode, cpu);
            break;
        case 0x85: //ADD L
            add_8(cpu->registers.L, opcode, cpu);
            break;
        case 0x86: //ADD (HL)
            add_8(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0x87: //ADD A
            add_8(cpu->registers.A, opcode, cpu);
            break;
        case 0x88: //ADC B
            adc(cpu->registers.B, opcode, cpu);
            break;
        case 0x89: //ADC C
            adc(cpu->registers.C, opcode, cpu);
            break;
        case 0x8A: //ADC D
            adc(cpu->registers.D, opcode, cpu);
            break;
        case 0x8B: //ADC E
            adc(cpu->registers.E, opcode, cpu);
            break;
        case 0x8C: //ADC H
            adc(cpu->registers.H, opcode, cpu);
            break;
        case 0x8D: //ADC L
            adc(cpu->registers.L, opcode, cpu);
            break;
        case 0x8E: //ADC (HL)
            adc(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0x8F: //ADC A
            adc(cpu->registers.A, opcode, cpu);
            break;
        case 0x90: //SUB B
            sub_8(cpu->registers.B, opcode, cpu);
            break;
        case 0x91: //SUB C
            sub_8(cpu->registers.C, opcode, cpu);
            break;
        case 0x92: //SUB D
            sub_8(cpu->registers.D, opcode, cpu);
            break;
        case 0x93: //SUB E
            sub_8(cpu->registers.E, opcode, cpu);
            break;
        case 0x94: //SUB H
            sub_8(cpu->registers.H, opcode, cpu);
            break;
        case 0x95: //SUB L
            sub_8(cpu->registers.L, opcode, cpu);
            break;
        case 0x96: //SUB (HL)
            sub_8(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0x97: //SUB A
            sub_8(cpu->registers.A, opcode, cpu);
            break;
        case 0x98: //SBC B
            sbc(cpu->registers.B, opcode, cpu);
            break;
        case 0x99: //SBC C
            sbc(cpu->registers.C, opcode, cpu);
            break;
        case 0x9A: //SBC D
            sbc(cpu->registers.D, opcode, cpu);
            break;
        case 0x9B: //SBC E
            sbc(cpu->registers.E, opcode, cpu);
            break;
        case 0x9C: //SBC H
            sbc(cpu->registers.H, opcode, cpu);
            break;
        case 0x9D: //SBC L
            sbc(cpu->registers.L, opcode, cpu);
            break;
        case 0x9E: //SBC (HL)
            sbc(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0x9F: //SBC A
            sbc(cpu->registers.A, opcode, cpu);
            break;
        case 0xA0: //AND B
            and(cpu->registers.B, opcode, cpu);
            break;
        case 0xA1: //AND C
            and(cpu->registers.C, opcode, cpu);
            break;
        case 0xA2: //AND D
            and(cpu->registers.D, opcode, cpu);
            break;
        case 0xA3: //AND E
            and(cpu->registers.E, opcode, cpu);
            break;
        case 0xA4: //AND H
            and(cpu->registers.H, opcode, cpu);
            break;
        case 0xA5: //AND L
            and(cpu->registers.L, opcode, cpu);
            break;
        case 0xA6: //AND (HL)
            and(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0xA7: //AND A
            and(cpu->registers.A, opcode, cpu);
            break;
        case 0xA8: //XOR B
            xor(cpu->registers.B, opcode, cpu);
            break;
        case 0xA9: //XOR C
            xor(cpu->registers.C, opcode, cpu);
            break;
        case 0xAA: //XOR D
            xor(cpu->registers.D, opcode, cpu);
            break;
        case 0xAB: //XOR E
            xor(cpu->registers.E, opcode, cpu);
            break;
        case 0xAC: //XOR H
            xor(cpu->registers.H, opcode, cpu);
            break;
        case 0xAD: //XOR L
            xor(cpu->registers.L, opcode, cpu);
            break;
        case 0xAE: //XOR (HL)
            xor(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0xAF: //XOR A
            xor(cpu->registers.A, opcode, cpu);
            break;
        case 0xB0: //OR B
            or(cpu->registers.B, opcode, cpu);
            break;
        case 0xB1: //OR C
            or(cpu->registers.C, opcode, cpu);
            break;
        case 0xB2: //OR D
            or(cpu->registers.D, opcode, cpu);
            break;
        case 0xB3: //OR E
            or(cpu->registers.E, opcode, cpu);
            break;
        case 0xB4: //OR H
            or(cpu->registers.H, opcode, cpu);
            break;
        case 0xB5: //OR L
            or(cpu->registers.L, opcode, cpu);
            break;
        case 0xB6: //OR (HL)
            or(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0xB7: //OR A
            or(cpu->registers.A, opcode, cpu);
            break;
        case 0xB8: //CP B
            cp(cpu->registers.B, opcode, cpu);
            break;
        case 0xB9: //CP C
            cp(cpu->registers.C, opcode, cpu);
            break;
        case 0xBA: //CP D
            cp(cpu->registers.D, opcode, cpu);
            break;
        case 0xBB: //CP E
            cp(cpu->registers.E, opcode, cpu);
            break;
        case 0xBC: //CP H
            cp(cpu->registers.H, opcode, cpu);
            break;
        case 0xBD: //CP L
            cp(cpu->registers.L, opcode, cpu);
            break;
        case 0xBE: //CP (HL)
            cp(readByte(cpu->registers.HL, cpu), opcode, cpu);
            break;
        case 0xBF: //CP A
            cp(cpu->registers.A, opcode, cpu);
            break;
        case 0xC0: //RET NZ
            ret_c(!readFlag(ZF, cpu), opcode, cpu);
            break;
        case 0xC1: //POP BC
            pop(&cpu->registers.BC, opcode, cpu);
            break;
        case 0xC2: //JP NZ, a16
            jp_c(!readFlag(ZF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xC3: //JP a16
            jp_c(true, twoBytes(cpu), opcode, cpu); //will always jump so set condition to be true
            break;
        case 0xC4: //CALL NZ, a16
            call_c(!readFlag(ZF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xC5: //PUSH BC
            push(cpu->registers.BC, opcode, cpu);
            break;
        case 0xC6: //ADD A, d8
            add_8(oneByte(cpu), opcode, cpu);
            break;
        case 0xC7: //RST 0x00
            rst(0x00, opcode, cpu);
            break;
        case 0xC8: //RET Z
            ret_c(readFlag(ZF, cpu), opcode, cpu);
            break;
        case 0xC9: //RET
            ret_c(true, opcode, cpu); //will always return so set the condition to be true
            break;
        case 0xCA: //JP Z, a16
            jp_c(readFlag(ZF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xCB: //PREFIX CB
            //return result of the cb prefix instruction
            return prefixCB(cpu);
            break;
        case 0xCC: //CALL Z, a16
            call_c(readFlag(ZF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xCD: //CALL a16
            call_c(true, twoBytes(cpu), opcode, cpu);
            break;
        case 0xCE: //ADC d8
            adc(oneByte(cpu), opcode, cpu);
            break;
        case 0xCF: //RST 0x08
            rst(0x08, opcode, cpu);
            break;
        case 0xD0: //RET NC
            ret_c(!readFlag(CF, cpu), opcode, cpu);
            break;
        case 0xD1: //POP DE
            pop(&cpu->registers.DE, opcode, cpu);
            break;
        case 0xD2: //JP NC, a16
            jp_c(!readFlag(CF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xD4: //CALL NC, a16
            call_c(!readFlag(CF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xD5: //PUSH DE
            push(cpu->registers.DE, opcode, cpu);
            break;
        case 0xD6: //SUB d8
            sub_8(oneByte(cpu), opcode, cpu);
            break;
        case 0xD8: //RET C
            ret_c(readFlag(CF, cpu), opcode, cpu);
            break;
        case 0xD9: //RETI
            reti(opcode, cpu);
            break;
        case 0xDA: //JP C, a16
            jp_c(readFlag(CF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xDC: //CALL C, a16
            call_c(readFlag(CF, cpu), twoBytes(cpu), opcode, cpu);
            break;
        case 0xDE: //SBC d8
            sbc(oneByte(cpu), opcode, cpu);
            break;
        case 0xE0: //LDH (a8), A
            ld_8_m(cpu->registers.A, 0xFF00 + oneByte(cpu), opcode, cpu);
            break;
        case 0xE1: //POP HL
            pop(&cpu->registers.HL, opcode, cpu);
            break;
        case 0xE2: //LD (C), A
            ld_8_m(cpu->registers.A, 0xFF00 + cpu->registers.C, opcode, cpu);
            break;
        case 0xE5: //PUSH HL
            push(cpu->registers.HL, opcode, cpu);
            break;
        case 0xE6: //AND d8
            and(readNextByte(cpu), opcode, cpu);
            break;
        case 0xE7: //RST 0x20
            rst(0x20, opcode, cpu);
            break;
        case 0xE9: //JP (HL) -> read as JP HL
            jp_c(true, cpu->registers.HL, opcode, cpu);
            break;
        case 0xEA: //LD (a16), A
            ld_8_m(cpu->registers.A, twoBytes(cpu), opcode, cpu);
            break;
        case 0xEE: //XOR d8
            xor(oneByte(cpu), opcode, cpu);
            break;
        case 0xEF: //RST 0x28
            rst(0x28, opcode, cpu);
            break;
        case 0xF0: //LDH A, (a8)
            ld_8(readByte(0xFF00 + readNextByte(cpu), cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0xF1: //POP AF
            pop(&cpu->registers.AF, opcode, cpu);
            break;
        case 0xF3: //DI
            di(opcode, cpu);
            break;
        case 0xF5: //PUSH AF
            push(cpu->registers.AF, opcode, cpu);
            break;
        case 0xF6: //OR d8
            or(oneByte(cpu), opcode, cpu);
            break;
        case 0xF9: //LD SP, HL
            ld_16(cpu->registers.HL, &cpu->SP, opcode, cpu);
            break;
        case 0xFA: //LD A, (a16)
            ld_8(readByte(twoBytes(cpu), cpu), &cpu->registers.A, opcode, cpu);
            break;
        case 0xFB: //EI
            ei(opcode, cpu);
            break;
        case 0xFE: //CP d8
            cp(oneByte(cpu), opcode, cpu);
            break;
        case 0xFF: //RST 0x38
            rst(0x38, opcode, cpu);
            break;
        default: //instruction not implemented
            printf("Error instruction not found: ");
            printInstruction(false, cpu->PC - 1, cpu);
            return 1;
    }
    return 0;
}
