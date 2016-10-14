#include "main.h"

//rotate given register left, old bit 7 to carry bit and bit 0
void rlc(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    (*reg >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left and use carry flag/bit 7 as new bit 0
    *reg = (*reg << 1) | readFlag(CF, cpu);
    //zero flag
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(HF, cpu);
    clearFlag(NF, cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//rotate a given register left, old bit 7 to carry bit and old carry bit to bit 0
void rl(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//rotate a given register left, old bit 0 to carry bit and bit 7
void rrc(uint8* reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//rotate a given register left, old bit 0 to carry bit and bit 7
void rrc_m(uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//right rotate a given register. New 7th bit is set by the carry flag and the carry flag is set by old 1st bit.
void rr(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//arithmetic left shift a given register. Set new bit 0 to 0 and put the old bit 7 into the carry flag
void sla(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//arithmetic right shift a given register. Set new bit 7 to the previus bit 7 and put the old bit 0 into the carry flag
void sra(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//logical right shift of given register. Set new bit 7 to 0 and put the old bit 0 into carry flag
void srl(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//swap the upper and lower nibbles of some register
void swap(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//swap the upper and lower nibbles of some register
void swap_m(uint8 opcode, cpu_state *cpu) {
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
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set flags based on the status of a bit in a register
void bit(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether the bit is zero
    (readBit(bit, reg)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //reset negative flag
    clearFlag(NF, cpu);
    //set half-carry flag
    setFlag(HF, cpu);
    //no change to carry flag
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set flags based on the status of a bit in memory
void bit_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    //set or reset zero flag based on whether the bit is zero
    (readBitMem(bit, cpu)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //clear negative flag
    clearFlag(NF, cpu);
    //set half-carry flag
    setFlag(HF, cpu);
    //no change to carry flag
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set 1 bit
void set(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg |= (0b1 << bit);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set 1 bit in memory location stored in HL
void set_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) | (0b1 << bit), cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//reset 1 bit
void res(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg &= ~(0b1 << bit);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//reset 1 bit in memory location stored in HL
void res_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) & ~(0b1 << bit), cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//execute instruction with a 0xCB pefix
int prefixCB(cpu_state *cpu) {
    //grab instruction
    uint8 opcode = readNextByte(cpu);
    switch(opcode) {
        case 0x02: //RLC D
            rlc(&cpu->registers.D, opcode, cpu);
            break;
        case 0x03: //RLC E
            rlc(&cpu->registers.E, opcode, cpu);
            break;
        case 0x10: //RL B
            rl(&cpu->registers.B, opcode, cpu);
            break;
        case 0x0E: //RRC (HL)
            rrc_m(opcode, cpu);
            break;
        case 0x12: //RL D
            rl(&cpu->registers.D, opcode, cpu);
            break;
        case 0x1A: //RR D
            rr(&cpu->registers.D, opcode, cpu);
            break;
        case 0x1B: //RR E
            rr(&cpu->registers.E, opcode, cpu);
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
        case 0x3B: //SRL E
            srl(&cpu->registers.E, opcode, cpu);
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
        case 0x53: //BIT 2, E
            bit(2, &cpu->registers.E, opcode, cpu);
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
        case 0x7E: //BIT 7, (HL)
            bit_m(7, opcode, cpu);
            break;
        case 0x7F: //BIT 7, A
            bit(7, &cpu->registers.A, opcode, cpu);
            break;
        case 0x86: //RES 0, (HL)
            res_m(0, opcode, cpu);
            break;
        case 0x87: //RES 0, A
            res(0, &cpu->registers.A, opcode, cpu);
            break;
        case 0x8E: //RES 1, (HL)
            res_m(1, opcode, cpu);
            break;
        case 0x8F: //RES 1, A
            res(1, &cpu->registers.A, opcode, cpu);
            break;
        case 0x96: //RES 2, (HL)
            res_m(2, opcode, cpu);
            break;
        case 0x97: //RES 2, A
            res(2, &cpu->registers.A, opcode, cpu);
            break;
        case 0x9E: //RES 3, (HL)
            res_m(3, opcode, cpu);
            break;
        case 0xA6: //RES 4, (HL)
            res_m(4, opcode, cpu);
            break;
        case 0xAE: //RES 5, (HL)
            res_m(5, opcode, cpu);
            break;
        case 0xAF: //RES 5, A
            res(5, &cpu->registers.A, opcode, cpu);
            break;
        case 0xBE: //RES 7, (HL)
            res_m(7, opcode, cpu);
            break;
        case 0xCE: //SET 1, (HL)
            set_m(1, opcode, cpu);
            break;
        case 0xD6: //SET 2, (HL)
            set_m(2, opcode, cpu);
            break;
        case 0xDE: //SET 3, (HL)
            set_m(3, opcode, cpu);
            break;
        case 0xE2: //SET 4, D
            set(4, &cpu->registers.D, opcode, cpu);
            break;
        case 0xE6: //SET 4, (HL)
            set_m(4, opcode, cpu);
            break;
        case 0xEE: //SET 5, (HL)
            set_m(5, opcode, cpu);
            break;
        case 0xF6: //SET 6, (HL)
            set_m(6, opcode, cpu);
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
