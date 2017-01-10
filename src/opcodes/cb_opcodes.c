#include "../types.h"
#include "../memory.h"
#include "../common.h"
#include "../cpu.h"
#include "opcodes.h"
#include <stdio.h>

//rotate given register left, old bit 7 to carry bit and bit 0
static void rlc(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (*reg >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    *reg = (*reg << 1) | readFlag(CF, cpu);
    //zero flag
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(HF, cpu);
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//rotate a given register left, old bit 7 to carry bit and old carry bit to bit 0
static void rl(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //read carry flag state into temp variable
    bool flagState = readFlag(CF, cpu);
    //update the carry flag
    (*reg >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    *reg <<= 1;
    //insert previous flag state into bit 0
    *reg |= flagState;
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//rotate a given register left, old bit 0 to carry bit and bit 7
static void rrc(uint8* reg, uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (*reg & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    *reg = (*reg >> 1) | (((uint8) readFlag(CF, cpu)) << 7);
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//rotate a given register left, old bit 0 to carry bit and bit 7
static void rrc_m(uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (readByte(cpu->registers.HL, cpu) & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    writeByte(cpu->registers.HL, (readByte(cpu->registers.HL, cpu) >> 1) | (((uint8) readFlag(CF, cpu)) << 7), cpu);
    //set or reset zero flag based on whether result is zero
    (readByte(cpu->registers.HL, cpu)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//right rotate a given register. New 7th bit is set by the carry flag and the carry flag is set by old 1st bit.
static void rr(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //read carry flag state into temp variable
    bool flagState = readFlag(CF, cpu);
    //update the carry flag
    (*reg & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    *reg >>= 1;
    //insert previous flag state into bit 7
    *reg |= flagState << 7;
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//arithmetic left shift a given register. Set new bit 0 to 0 and put the old bit 7 into the carry flag
static void sla(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (*reg >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left
    *reg <<= 1;
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//arithmetic right shift a given register. Set new bit 7 to the previus bit 7 and put the old bit 0 into the carry flag
static void sra(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    uint8 bit7 = *reg & 0x80; //0x80 = 0b10000000
    //set carry flag based on bit 0
    (*reg & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    *reg >>= 1;
    //set new bit seven to old bit 7
    *reg |= bit7;
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//logical right shift of given register. Set new bit 7 to 0 and put the old bit 0 into carry flag
static void srl(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry bit based on bit 0
    (*reg & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    *reg >>= 1;
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//swap the upper and lower nibbles of some register
static void swap(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset carry flag
    clearFlag(CF, cpu);
    //swap the upper and lower nibbles by masking and shifting
    *reg = ((*reg & 0xF) << 4) | ((*reg & 0xF0) >> 4);
    //set or reset zero flag based on whether result is zero
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//swap the upper and lower nibbles of some register
static void swap_m(uint8 opcode, cpu_state *cpu) {
    //reset negative flag
    clearFlag(NF, cpu);
    //reset half-carry flag
    clearFlag(HF, cpu);
    //reset carry flag
    clearFlag(CF, cpu);
    //swap the upper and lower nibbles by masking and shifting
    writeByte(cpu->registers.HL, ((readByte(cpu->registers.HL, cpu) & 0xF) << 4) | ((readByte(cpu->registers.HL, cpu) & 0xF0) >> 4), cpu);
    //set or reset zero flag based on whether result is zero
    (readByte(cpu->registers.HL, cpu)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//set flags based on the status of a bit in a register
static void bit(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether the bit is zero
    (readBit(bit, reg)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //set half-carry flag
    setFlag(HF, cpu);
    //no change to carry flag
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//set flags based on the status of a bit in memory
static void bit_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether the bit is zero
    (readBitMem(bit, cpu)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //clear negative flag
    clearFlag(NF, cpu);
    //set half-carry flag
    setFlag(HF, cpu);
    //no change to carry flag
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//set 1 bit
static void set(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg |= (0b1 << bit);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//set 1 bit in memory location stored in HL
static void set_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) | (0b1 << bit), cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//reset 1 bit
void res(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg &= ~(0b1 << bit);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//reset 1 bit in memory location stored in HL
static void res_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) & ~(0b1 << bit), cpu);
    cpu->wait = get_cb_opcode(opcode).cycles;
}

//execute instruction with a 0xCB pefix
int executeNextExtendedInstruction(cpu_state *cpu) {
    //grab instruction
    uint8 opcode = readNextByte(cpu);
    switch(opcode) {
        case 0x00: //RLC B
            rlc(&cpu->registers.B, opcode, cpu);
            break;
        case 0x01: //RLC C
            rlc(&cpu->registers.C, opcode, cpu);
            break;
        case 0x02: //RLC D
            rlc(&cpu->registers.D, opcode, cpu);
            break;
        case 0x03: //RLC E
            rlc(&cpu->registers.E, opcode, cpu);
            break;
        case 0x04: //RLC H
            rlc(&cpu->registers.H, opcode, cpu);
            break;
        case 0x05: //RLC L
            rlc(&cpu->registers.L, opcode, cpu);
            break;
        case 0x07: //RLC A
            rlc(&cpu->registers.A, opcode, cpu);
            break;
        case 0x08: //RRC B
            rrc(&cpu->registers.B, opcode, cpu);
            break;
        case 0x09: //RRC C
            rrc(&cpu->registers.C, opcode, cpu);
            break;
        case 0x0A: //RRC D
            rrc(&cpu->registers.D, opcode, cpu);
            break;
        case 0x0B: //RRC E
            rrc(&cpu->registers.E, opcode, cpu);
            break;
        case 0x0C: //RRC H
            rrc(&cpu->registers.H, opcode, cpu);
            break;
        case 0x0D: //RRC L
            rrc(&cpu->registers.L, opcode, cpu);
            break;
        case 0x0E: //RRC (HL)
            rrc_m(opcode, cpu);
            break;
        case 0x0F: //RRC A
            rrc(&cpu->registers.A, opcode, cpu);
            break;
        case 0x10: //RL B
            rl(&cpu->registers.B, opcode, cpu);
            break;
        case 0x11: //RL C
            rl(&cpu->registers.C, opcode, cpu);
            break;
        case 0x12: //RL D
            rl(&cpu->registers.D, opcode, cpu);
            break;
        case 0x13: //RL E
            rl(&cpu->registers.E, opcode, cpu);
            break;
        case 0x14: //RL H
            rl(&cpu->registers.H, opcode, cpu);
            break;
        case 0x15: //RL L
            rl(&cpu->registers.L, opcode, cpu);
            break;
        case 0x17: //RL A
            rl(&cpu->registers.A, opcode, cpu);
            break;
        case 0x18: //RR B
            rr(&cpu->registers.B, opcode, cpu);
            break;
        case 0x19: //RR C
            rr(&cpu->registers.C, opcode, cpu);
            break;
        case 0x1A: //RR D
            rr(&cpu->registers.D, opcode, cpu);
            break;
        case 0x1B: //RR E
            rr(&cpu->registers.E, opcode, cpu);
            break;
        case 0x1C: //RR H
            rr(&cpu->registers.H, opcode, cpu);
            break;
        case 0x1D: //RR L
            rr(&cpu->registers.L, opcode, cpu);
            break;
        case 0x1F: //RR A
            rr(&cpu->registers.A, opcode, cpu);
            break;
        case 0x20: //SLA B
            sla(&cpu->registers.B, opcode, cpu);
            break;
        case 0x21: //SLA C
            sla(&cpu->registers.C, opcode, cpu);
            break;
        case 0x22: //SLA D
            sla(&cpu->registers.D, opcode, cpu);
            break;
        case 0x23: //SLA E
            sla(&cpu->registers.E, opcode, cpu);
            break;
        case 0x27: //SLA A
            sla(&cpu->registers.A, opcode, cpu);
            break;
        case 0x2A: //SRA D
            sra(&cpu->registers.D, opcode, cpu);
            break;
        case 0x2F: //SRA A
            sra(&cpu->registers.A, opcode, cpu);
            break;
        case 0x30: //SWAP B
            swap(&cpu->registers.B, opcode, cpu);
            break;
        case 0x31: //SWAP C
            swap(&cpu->registers.C, opcode, cpu);
            break;
        case 0x32: //SWAP D
            swap(&cpu->registers.D, opcode, cpu);
            break;
        case 0x33: //SWAP E
            swap(&cpu->registers.E, opcode, cpu);
            break;
        case 0x34: //SWAP H
            swap(&cpu->registers.H, opcode, cpu);
            break;
        case 0x35: //SWAP L
            swap(&cpu->registers.L, opcode, cpu);
            break;
        case 0x36: //SWAP (HL)
            swap_m(opcode, cpu);
            break;
        case 0x37: //SWAP A
            swap(&cpu->registers.A, opcode, cpu);
            break;
        case 0x38: //SRL B
            srl(&cpu->registers.B, opcode, cpu);
            break;
        case 0x39: //SRL C
            srl(&cpu->registers.C, opcode, cpu);
            break;
        case 0x3A: //SRL D
            srl(&cpu->registers.D, opcode, cpu);
            break;
        case 0x3B: //SRL E
            srl(&cpu->registers.E, opcode, cpu);
            break;
        case 0x3C: //SRL H
            srl(&cpu->registers.H, opcode, cpu);
            break;
        case 0x3D: //SRL L
            srl(&cpu->registers.L, opcode, cpu);
            break;
        case 0x3F: //SRL A
            srl(&cpu->registers.A, opcode, cpu);
            break;
        case 0x40: //BIT 0, B
            bit(0, &cpu->registers.B, opcode, cpu);
            break;
        case 0x41: //BIT 0, C
            bit(0, &cpu->registers.C, opcode, cpu);
            break;
        case 0x42: //BIT 0, D
            bit(0, &cpu->registers.D, opcode, cpu);
            break;
        case 0x43: //BIT 0, E
            bit(0, &cpu->registers.E, opcode, cpu);
            break;
        case 0x44: //BIT 0, H
            bit(0, &cpu->registers.H, opcode, cpu);
            break;
        case 0x45: //BIT 0, L
            bit(0, &cpu->registers.L, opcode, cpu);
            break;
        case 0x46: //BIT 0, (HL)
            bit_m(0, opcode, cpu);
            break;
        case 0x47: //BIT 0, A
            bit(0, &cpu->registers.A, opcode, cpu);
            break;
        case 0x48: //BIT 1, B
            bit(1, &cpu->registers.B, opcode, cpu);
            break;
        case 0x49: //BIT 1, C
            bit(1, &cpu->registers.C, opcode, cpu);
            break;
        case 0x4A: //BIT 1, D
            bit(1, &cpu->registers.D, opcode, cpu);
            break;
        case 0x4B: //BIT 1, E
            bit(1, &cpu->registers.E, opcode, cpu);
            break;
        case 0x4C: //BIT 1, H
            bit(1, &cpu->registers.H, opcode, cpu);
            break;
        case 0x4D: //BIT 1, L
            bit(1, &cpu->registers.L, opcode, cpu);
            break;
        case 0x4E: //BIT 1, (HL)
            bit_m(1, opcode, cpu);
            break;
        case 0x4F: //BIT 1, A
            bit(1, &cpu->registers.A, opcode, cpu);
            break;
        case 0x50: //BIT 2, B
            bit(2, &cpu->registers.B, opcode, cpu);
            break;
        case 0x51: //BIT 2, C
            bit(2, &cpu->registers.C, opcode, cpu);
            break;
        case 0x52: //BIT 2, D
            bit(2, &cpu->registers.D, opcode, cpu);
            break;
        case 0x53: //BIT 2, E
            bit(2, &cpu->registers.E, opcode, cpu);
            break;
        case 0x54: //BIT 2, H
            bit(2, &cpu->registers.H, opcode, cpu);
            break;
        case 0x55: //BIT 2, L
            bit(2, &cpu->registers.L, opcode, cpu);
            break;
        case 0x56: //BIT 2, (HL)
            bit_m(2, opcode, cpu);
            break;
        case 0x57: //BIT 2, A
            bit(2, &cpu->registers.A, opcode, cpu);
            break;
        case 0x58: //BIT 3, B
            bit(3, &cpu->registers.B, opcode, cpu);
            break;
        case 0x59: //BIT 3, C
            bit(3, &cpu->registers.C, opcode, cpu);
            break;
        case 0x5A: //BIT 3, D
            bit(3, &cpu->registers.D, opcode, cpu);
            break;
        case 0x5B: //BIT 3, E
            bit(3, &cpu->registers.E, opcode, cpu);
            break;
        case 0x5C: //BIT 3, H
            bit(3, &cpu->registers.H, opcode, cpu);
            break;
        case 0x5D: //BIT 3, L
            bit(3, &cpu->registers.L, opcode, cpu);
            break;
        case 0x5E: //BIT 3, (HL)
            bit_m(3, opcode, cpu);
            break;
        case 0x5F: //BIT 3, A
            bit(3, &cpu->registers.A, opcode, cpu);
            break;
        case 0x60: //BIT 4, B
            bit(4, &cpu->registers.B, opcode, cpu);
            break;
        case 0x61: //BIT 4, C
            bit(4, &cpu->registers.C, opcode, cpu);
            break;
        case 0x62: //BIT 4, D
            bit(4, &cpu->registers.D, opcode, cpu);
            break;
        case 0x63: //BIT 4, E
            bit(4, &cpu->registers.E, opcode, cpu);
            break;
        case 0x64: //BIT 4, H
            bit(4, &cpu->registers.H, opcode, cpu);
            break;
        case 0x65: //BIT 4, L
            bit(4, &cpu->registers.L, opcode, cpu);
            break;
        case 0x66: //BIT 4, (HL)
            bit_m(4, opcode, cpu);
            break;
        case 0x67: //BIT 4, A
            bit(4, &cpu->registers.A, opcode, cpu);
            break;
        case 0x68: //BIT 5, B
            bit(5, &cpu->registers.B, opcode, cpu);
            break;
        case 0x69: //BIT 5, C
            bit(5, &cpu->registers.C, opcode, cpu);
            break;
        case 0x6A: //BIT 5, D
            bit(5, &cpu->registers.D, opcode, cpu);
            break;
        case 0x6B: //BIT 5, E
            bit(5, &cpu->registers.E, opcode, cpu);
            break;
        case 0x6C: //BIT 5, H
            bit(5, &cpu->registers.H, opcode, cpu);
            break;
        case 0x6D: //BIT 5, L
            bit(5, &cpu->registers.L, opcode, cpu);
            break;
        case 0x6E: //BIT 5, (HL)
            bit_m(5, opcode, cpu);
            break;
        case 0x6F: //BIT 5, A
            bit(5, &cpu->registers.A, opcode, cpu);
            break;
        case 0x70: //BIT 6, B
            bit(6, &cpu->registers.B, opcode, cpu);
            break;
        case 0x71: //BIT 6, C
            bit(6, &cpu->registers.C, opcode, cpu);
            break;
        case 0x72: //BIT 6, D
            bit(6, &cpu->registers.D, opcode, cpu);
            break;
        case 0x73: //BIT 6, E
            bit(6, &cpu->registers.E, opcode, cpu);
            break;
        case 0x74: //BIT 6, H
            bit(6, &cpu->registers.H, opcode, cpu);
            break;
        case 0x75: //BIT 6, L
            bit(6, &cpu->registers.L, opcode, cpu);
            break;
        case 0x76: //BIT 6, (HL)
            bit_m(6, opcode, cpu);
            break;
        case 0x77: //BIT 6, A
            bit(6, &cpu->registers.A, opcode, cpu);
            break;
        case 0x78: //BIT 7, B
            bit(7, &cpu->registers.B, opcode, cpu);
            break;
        case 0x79: //BIT 7, C
            bit(7, &cpu->registers.C, opcode, cpu);
            break;
        case 0x7A: //BIT 7, D
            bit(7, &cpu->registers.D, opcode, cpu);
            break;
        case 0x7B: //BIT 7, E
            bit(7, &cpu->registers.E, opcode, cpu);
            break;
        case 0x7C: //BIT 7, H
            bit(7, &cpu->registers.H, opcode, cpu);
            break;
        case 0x7D: //BIT 7, L
            bit(7, &cpu->registers.L, opcode, cpu);
            break;
        case 0x7E: //BIT 7, (HL)
            bit_m(7, opcode, cpu);
            break;
        case 0x7F: //BIT 7, A
            bit(7, &cpu->registers.A, opcode, cpu);
            break;
        case 0x80: //RES 0, B
            res(0, &cpu->registers.B, opcode, cpu);
            break;
        case 0x81: //RES 0, C
            res(0, &cpu->registers.C, opcode, cpu);
            break;
        case 0x82: //RES 0, D
            res(0, &cpu->registers.D, opcode, cpu);
            break;
        case 0x83: //RES 0, E
            res(0, &cpu->registers.E, opcode, cpu);
            break;
        case 0x84: //RES 0, H
            res(0, &cpu->registers.H, opcode, cpu);
            break;
        case 0x85: //RES 0, L
            res(0, &cpu->registers.L, opcode, cpu);
            break;
        case 0x86: //RES 0, (HL)
            res_m(0, opcode, cpu);
            break;
        case 0x87: //RES 0, A
            res(0, &cpu->registers.A, opcode, cpu);
            break;
        case 0x88: //RES 1, B
            res(1, &cpu->registers.B, opcode, cpu);
            break;
        case 0x89: //RES 1, C
            res(1, &cpu->registers.C, opcode, cpu);
            break;
        case 0x8A: //RES 1, D
            res(1, &cpu->registers.D, opcode, cpu);
            break;
        case 0x8B: //RES 1, E
            res(1, &cpu->registers.E, opcode, cpu);
            break;
        case 0x8C: //RES 1, H
            res(1, &cpu->registers.H, opcode, cpu);
            break;
        case 0x8D: //RES 1, L
            res(1, &cpu->registers.L, opcode, cpu);
            break;
        case 0x8E: //RES 1, (HL)
            res_m(1, opcode, cpu);
            break;
        case 0x8F: //RES 1, A
            res(1, &cpu->registers.A, opcode, cpu);
            break;
        case 0x90: //RES 2, B
            res(2, &cpu->registers.B, opcode, cpu);
            break;
        case 0x91: //RES 2, C
            res(2, &cpu->registers.C, opcode, cpu);
            break;
        case 0x92: //RES 2, D
            res(2, &cpu->registers.D, opcode, cpu);
            break;
        case 0x93: //RES 2, E
            res(2, &cpu->registers.E, opcode, cpu);
            break;
        case 0x94: //RES 2, H
            res(2, &cpu->registers.H, opcode, cpu);
            break;
        case 0x95: //RES 2, L
            res(2, &cpu->registers.L, opcode, cpu);
            break;
        case 0x96: //RES 2, (HL)
            res_m(2, opcode, cpu);
            break;
        case 0x97: //RES 2, A
            res(2, &cpu->registers.A, opcode, cpu);
            break;
        case 0x98: //RES 3, B
            res(3, &cpu->registers.B, opcode, cpu);
            break;
        case 0x99: //RES 3, C
            res(3, &cpu->registers.C, opcode, cpu);
            break;
        case 0x9A: //RES 3, D
            res(3, &cpu->registers.D, opcode, cpu);
            break;
        case 0x9B: //RES 3, E
            res(3, &cpu->registers.E, opcode, cpu);
            break;
        case 0x9C: //RES 3, H
            res(3, &cpu->registers.H, opcode, cpu);
            break;
        case 0x9D: //RES 3, L
            res(3, &cpu->registers.L, opcode, cpu);
            break;
        case 0x9E: //RES 3, (HL)
            res_m(3, opcode, cpu);
            break;
        case 0x9F: //RES 3, A
            res(3, &cpu->registers.A, opcode, cpu);
            break;
        case 0xA0: //RES 4, B
            res(4, &cpu->registers.B, opcode, cpu);
            break;
        case 0xA1: //RES 4, C
            res(4, &cpu->registers.C, opcode, cpu);
            break;
        case 0xA2: //RES 4, D
            res(4, &cpu->registers.D, opcode, cpu);
            break;
        case 0xA3: //RES 4, E
            res(4, &cpu->registers.E, opcode, cpu);
            break;
        case 0xA4: //RES 4, H
            res(4, &cpu->registers.H, opcode, cpu);
            break;
        case 0xA5: //RES 4, L
            res(4, &cpu->registers.L, opcode, cpu);
            break;
        case 0xA6: //RES 4, (HL)
            res_m(4, opcode, cpu);
            break;
        case 0xA7: //RES 4, A
            res(4, &cpu->registers.A, opcode, cpu);
            break;
        case 0xA8: //RES 5, B
            res(5, &cpu->registers.B, opcode, cpu);
            break;
        case 0xA9: //RES 5, C
            res(5, &cpu->registers.C, opcode, cpu);
            break;
        case 0xAA: //RES 5, D
            res(5, &cpu->registers.D, opcode, cpu);
            break;
        case 0xAB: //RES 5, E
            res(5, &cpu->registers.E, opcode, cpu);
            break;
        case 0xAC: //RES 5, H
            res(5, &cpu->registers.H, opcode, cpu);
            break;
        case 0xAD: //RES 5, L
            res(5, &cpu->registers.L, opcode, cpu);
            break;
        case 0xAE: //RES 5, (HL)
            res_m(5, opcode, cpu);
            break;
        case 0xAF: //RES 5, A
            res(5, &cpu->registers.A, opcode, cpu);
            break;
        case 0xB0: //RES 6, B
            res(6, &cpu->registers.B, opcode, cpu);
            break;
        case 0xB1: //RES 6, C
            res(6, &cpu->registers.C, opcode, cpu);
            break;
        case 0xB2: //RES 6, D
            res(6, &cpu->registers.D, opcode, cpu);
            break;
        case 0xB3: //RES 6, E
            res(6, &cpu->registers.E, opcode, cpu);
            break;
        case 0xB4: //RES 6, H
            res(6, &cpu->registers.H, opcode, cpu);
            break;
        case 0xB5: //RES 6, L
            res(6, &cpu->registers.L, opcode, cpu);
            break;
        case 0xB6: //RES 6, (HL)
            res_m(6, opcode, cpu);
            break;
        case 0xB7: //RES 6, A
            res(6, &cpu->registers.A, opcode, cpu);
            break;
        case 0xB8: //RES 7, B
            res(7, &cpu->registers.B, opcode, cpu);
            break;
        case 0xB9: //RES 7, C
            res(7, &cpu->registers.C, opcode, cpu);
            break;
        case 0xBA: //RES 7, D
            res(7, &cpu->registers.D, opcode, cpu);
            break;
        case 0xBB: //RES 7, E
            res(7, &cpu->registers.E, opcode, cpu);
            break;
        case 0xBC: //RES 7, H
            res(7, &cpu->registers.H, opcode, cpu);
            break;
        case 0xBD: //RES 7, L
            res(7, &cpu->registers.L, opcode, cpu);
            break;
        case 0xBE: //RES 7, (HL)
            res_m(7, opcode, cpu);
            break;
        case 0xBF: //RES 7, A
            res(7, &cpu->registers.A, opcode, cpu);
            break;
        case 0xC0: //SET 0, B
            set(0, &cpu->registers.B, opcode, cpu);
            break;
        case 0xC1: //SET 0, C
            set(0, &cpu->registers.C, opcode, cpu);
            break;
        case 0xC2: //SET 0, D
            set(0, &cpu->registers.D, opcode, cpu);
            break;
        case 0xC3: //SET 0, E
            set(0, &cpu->registers.E, opcode, cpu);
            break;
        case 0xC4: //SET 0, H
            set(0, &cpu->registers.H, opcode, cpu);
            break;
        case 0xC5: //SET 0, L
            set(0, &cpu->registers.L, opcode, cpu);
            break;
        case 0xC6: //SET 0, (HL)
            set_m(0, opcode, cpu);
            break;
        case 0xC7: //SET 0, A
            set(0, &cpu->registers.A, opcode, cpu);
            break;
        case 0xC8: //SET 1, B
            set(1, &cpu->registers.B, opcode, cpu);
            break;
        case 0xC9: //SET 1, C
            set(1, &cpu->registers.C, opcode, cpu);
            break;
        case 0xCA: //SET 1, D
            set(1, &cpu->registers.D, opcode, cpu);
            break;
        case 0xCB: //SET 1, E
            set(1, &cpu->registers.E, opcode, cpu);
            break;
        case 0xCC: //SET 1, H
            set(1, &cpu->registers.H, opcode, cpu);
            break;
        case 0xCD: //SET 1, L
            set(1, &cpu->registers.L, opcode, cpu);
            break;
        case 0xCE: //SET 1, (HL)
            set_m(1, opcode, cpu);
            break;
        case 0xCF: //SET 1, A
        	set(1, &cpu->registers.A, opcode, cpu);
        	break;
        case 0xD0: //SET 2, B
            set(2, &cpu->registers.B, opcode, cpu);
            break;
        case 0xD1: //SET 2, C
            set(2, &cpu->registers.C, opcode, cpu);
            break;
        case 0xD2: //SET 2, D
            set(2, &cpu->registers.D, opcode, cpu);
            break;
        case 0xD3: //SET 2, E
            set(2, &cpu->registers.E, opcode, cpu);
            break;
        case 0xD4: //SET 2, H
            set(2, &cpu->registers.H, opcode, cpu);
            break;
        case 0xD5: //SET 2, L
            set(2, &cpu->registers.L, opcode, cpu);
            break;
        case 0xD6: //SET 2, (HL)
            set_m(2, opcode, cpu);
            break;
        case 0xD7: //SET 2, A
            set(2, &cpu->registers.A, opcode, cpu);
            break;
        case 0xD8: //SET 3, B
            set(3, &cpu->registers.B, opcode, cpu);
            break;
        case 0xD9: //SET 3, C
            set(3, &cpu->registers.C, opcode, cpu);
            break;
        case 0xDA: //SET 3, D
            set(3, &cpu->registers.D, opcode, cpu);
            break;
        case 0xDB: //SET 3, E
            set(3, &cpu->registers.E, opcode, cpu);
            break;
        case 0xDC: //SET 3, H
            set(3, &cpu->registers.H, opcode, cpu);
            break;
        case 0xDD: //SET 3, L
            set(3, &cpu->registers.L, opcode, cpu);
            break;
        case 0xDE: //SET 3, (HL)
            set_m(3, opcode, cpu);
            break;
        case 0xDF: //SET 3, A
        	set(3, &cpu->registers.A, opcode, cpu);
        	break;
        case 0xE0: //SET 4, B
            set(4, &cpu->registers.B, opcode, cpu);
            break;
        case 0xE1: //SET 4, C
            set(4, &cpu->registers.C, opcode, cpu);
            break;
        case 0xE2: //SET 4, D
            set(4, &cpu->registers.D, opcode, cpu);
            break;
        case 0xE3: //SET 4, E
            set(4, &cpu->registers.E, opcode, cpu);
            break;
        case 0xE4: //SET 4, H
            set(4, &cpu->registers.H, opcode, cpu);
            break;
        case 0xE5: //SET 4, L
            set(4, &cpu->registers.L, opcode, cpu);
            break;
        case 0xE6: //SET 4, (HL)
            set_m(4, opcode, cpu);
            break;
        case 0xE7: //SET 4, A
            set(4, &cpu->registers.A, opcode, cpu);
            break;
        case 0xE8: //SET 5, B
            set(5, &cpu->registers.B, opcode, cpu);
            break;
        case 0xE9: //SET 5, C
            set(5, &cpu->registers.C, opcode, cpu);
            break;
        case 0xEA: //SET 5, D
            set(5, &cpu->registers.D, opcode, cpu);
            break;
        case 0xEB: //SET 5, E
            set(5, &cpu->registers.E, opcode, cpu);
            break;
        case 0xEC: //SET 5, H
            set(5, &cpu->registers.H, opcode, cpu);
            break;
        case 0xED: //SET 5, L
            set(5, &cpu->registers.L, opcode, cpu);
            break;
        case 0xEE: //SET 5, (HL)
            set_m(5, opcode, cpu);
            break;
        case 0xEF: //SET 5, A
        	set(5, &cpu->registers.A, opcode, cpu);
        	break;
        case 0xF0: //SET 6, B
            set(6, &cpu->registers.B, opcode, cpu);
            break;
        case 0xF1: //SET 6, C
            set(6, &cpu->registers.C, opcode, cpu);
            break;
        case 0xF2: //SET 6, D
            set(6, &cpu->registers.D, opcode, cpu);
            break;
        case 0xF3: //SET 6, E
            set(6, &cpu->registers.E, opcode, cpu);
            break;
        case 0xF4: //SET 6, H
            set(6, &cpu->registers.H, opcode, cpu);
            break;
        case 0xF5: //SET 6, L
            set(6, &cpu->registers.L, opcode, cpu);
            break;
        case 0xF6: //SET 6, (HL)
            set_m(6, opcode, cpu);
            break;
        case 0xF7: //SET 6, A
            set(6, &cpu->registers.A, opcode, cpu);
            break;
        case 0xF8: //SET 7, B
            set(7, &cpu->registers.B, opcode, cpu);
            break;
        case 0xF9: //SET 7, C
            set(7, &cpu->registers.C, opcode, cpu);
            break;
        case 0xFA: //SET 7, D
            set(7, &cpu->registers.D, opcode, cpu);
            break;
        case 0xFB: //SET 7, E
            set(7, &cpu->registers.E, opcode, cpu);
            break;
        case 0xFC: //SET 7, H
            set(7, &cpu->registers.H, opcode, cpu);
            break;
        case 0xFD: //SET 7, L
            set(7, &cpu->registers.L, opcode, cpu);
            break;
        case 0xFE: //SET 7, (HL)
            set_m(7, opcode, cpu);
            break;
        case 0xFF: //SET 7,A
            set(7, &cpu->registers.A, opcode, cpu);
            break;
        default: //instruction not implemented
            printf("Error prefix 0xCB instruction not found: ");
            printInstruction(false, cpu->PC - 2, cpu);
            return 1;
    }
    return 0;
}

const cb_opcode cb_opcodes[256];

// Return information about requested opcode
cb_opcode get_cb_opcode(uint8 value) {
    return cb_opcodes[value];
}

// Store format to printf each instruction, its number of cycles, and how many bytes the instruction is in memory.
const cb_opcode cb_opcodes[256] = {
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
