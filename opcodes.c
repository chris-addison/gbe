/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"
#include "common.c"

uint8 oneByteUnsigned(struct cpu_state * cpu) {
    return readNextByte(cpu);
}

int8 oneByteSigned(struct cpu_state * cpu) {
    return (int8)readNextByte(cpu);
}

//read a short from memory. Value is assumed to be little endian.
uint16 twoByte(struct cpu_state * cpu) {
	uint16 lsb = (uint16)readNextByte(cpu); //read least significant byte first
	uint16 msb = ((uint16)readNextByte(cpu)) << 8; //then read most significant byte
    return lsb | msb;
}

//compare instruction
void cp(uint8 a, uint8 b, struct cpu_state * cpu) {
    uint8 temp = a - b;
    //set flags
    cpu->FLAGS[ZF] = (temp == 0); //zero flag
    cpu->FLAGS[NF] = 0x1; //subtract flag
    cpu->FLAGS[HF] = ((a & 0xF) < (b & 0xF)); //half-carry flag
    cpu->FLAGS[CF] = (a < b); //carry flag
}

int execute(struct cpu_state * cpu) {
    //grab instruction
    uint8 nextByte = readNextByte(cpu);
    //temp values as switch has no scope in c
    uint8 tempByte = 0x0;
    int8 tempSigned = 0x0;
    uint8 tempByteTwo = 0x0;
    uint8 readByte = 0x0;
    uint16 tempShort = 0x0;
    
    //find and execute next instruction
    switch (nextByte) {
        case 0x00: //NOP
            cpu->wait = 1;
            break;
        case 0x01: //LD BC,d16
            saveShortBigEndian(cpu->REG + B, twoByte(cpu));
            cpu->wait = 12;
            break;
        case 0x02: //LD (BC),A
            tempShort = getShortLittleEndian(cpu->REG + B);
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 8;
            break;
        case 0x03:
            break;
        case 0x04:
            break;
        case 0x05:
            break;
        case 0x06: //LD B,d8
            cpu->REG[B] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x07:
            break;
        case 0x08: //LD (a16),SP
            tempShort = twoByte(cpu);
            saveShortLittleEndian(cpu->MEM + tempShort, cpu->SP);
            break;
        case 0x09:
            break;
        case 0x0A: //LD A,(BC)
            tempShort = getShortBigEndian(cpu->REG + B);
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x0B:
            break;
        case 0x0C:
            break;
        case 0x0D:
            break;
        case 0x0E: //LD C,d8
            cpu->REG[C] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x0F:
            break;
        case 0x10:
            break;
        case 0x11: //LD DE,d16
            saveShortBigEndian(cpu->REG + D, twoByte(cpu));
            cpu->wait = 12;
            break;
        case 0x12: //LD (DE),A
            tempShort = getShortBigEndian(cpu->REG + D);
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 8;
            break;
        case 0x13:
            break;
        case 0x14:
            break;
        case 0x15:
            break;
        case 0x16: //LD D,d8
            cpu->REG[D] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x17:
            break;
        case 0x18: //JR r8
            readByte = oneByteSigned(cpu);
            cpu->PC += (readByte - 2);
            cpu->wait = 12;
            break;
        case 0x19:
            break;
        case 0x1A: //LD A,(DE)
            tempShort = getShortBigEndian(cpu->REG + D);
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x1B:
            break;
        case 0x1C:
            break;
        case 0x1D:
            break;
        case 0x1E: //LD E,d8
            cpu->REG[E] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x1F:
            break;
        case 0x20: //JR NZ,r8
            readByte = oneByteSigned(cpu);
            if (cpu->FLAGS[ZF] == 0) {
                cpu->PC += (readByte - 2);
                cpu->wait = 12;
            } else {
                cpu->wait = 8;
            }
            break;
        case 0x21: //LD HL,d16
            saveShortBigEndian(cpu->REG + H, twoByte(cpu));
            cpu->wait = 12;
            break;
        case 0x22: //LD (HL+),A
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[A];
            //Increment the HL
            tempShort++;
            saveShortBigEndian(cpu->REG + H, tempShort);
            cpu->wait = 8;
            break;
        case 0x23:
            break;
        case 0x24:
            break;
        case 0x25:
            break;
        case 0x26: //LD H,d8
            cpu->REG[H] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x27:
            break;
        case 0x28:  //JR Z,r8
            readByte = oneByteUnsigned(cpu);
            if (cpu->FLAGS[ZF]) {
                cpu->PC += (uint16)readByte - 2;
                cpu->wait = 12;
            } else {
                cpu->wait = 8;
            }
            break;
        case 0x29:
            break;
        case 0x2A: //LD A,(HL+)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[A] = cpu->MEM[tempShort];
            //decrement the HL
            tempShort++;
            saveShortBigEndian(cpu->REG + H, tempShort);
            cpu->wait = 8;
            break;
        case 0x2B:
            break;
        case 0x2C:
            break;
        case 0x2D:
            break;
        case 0x2E: //LD L,d8
            cpu->REG[L] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x2F:
            break;
        case 0x30: //JR NC,r8
            readByte = oneByteSigned(cpu);
            if (cpu->FLAGS[CF] == 0) {
                cpu->PC += (readByte - 2);
                cpu->wait = 12;
            } else {
                cpu->wait = 8;
            }
            break;
        case 0x31: //LD SP,d16
            cpu->SP = twoByte(cpu);
            cpu->wait = 12;
            break;
        case 0x32: //LD (HL-),A
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[A];
            //decrement the HL
            tempShort--;
            saveShortBigEndian(cpu->REG + H, tempShort);
            cpu->wait = 8;
            break;
        case 0x33:
            break;
        case 0x34:
            break;
        case 0x35:
            break;
        case 0x36: //LD (HL),d8
            tempShort = cpu->REG[H];
            cpu->MEM[tempShort] = oneByteUnsigned(cpu);
            cpu->wait = 12;
            break;
        case 0x37:
            break;
        case 0x38: //JR C,r8
            readByte = oneByteSigned(cpu);
            if (cpu->FLAGS[CF]) {
                cpu->PC += (readByte - 2);
                cpu->wait = 12;
            } else {
                cpu->wait = 8;
            }
            break;
        case 0x39:
            break;
        case 0x3A: //LD A,(HL-)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[A] = cpu->MEM[tempShort];
            //decrement the HL
            tempShort--;
            saveShortBigEndian(cpu->REG + H, tempShort);
            cpu->wait = 8;
            break;
        case 0x3B:
            break;
        case 0x3C:
            break;
        case 0x3D:
            break;
        case 0x3E: //LD A,d8
            cpu->REG[A] = oneByteUnsigned(cpu);
            cpu->wait = 8;
            break;
        case 0x3F:
            break;
        case 0x40: //LD B,B
            cpu->REG[B] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x41: //LD B,C
            cpu->REG[B] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x42: //LD B,D
            cpu->REG[B] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x43: //LD B,E
            cpu->REG[B] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x44: //LD B,H
            cpu->REG[B] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x45: //LD B,L
            cpu->REG[B] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x46: //LD B,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[B] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x47: //LD B,A
            cpu->REG[B] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x48: //LD C,B
            cpu->REG[C] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x49: //LD C,C
            cpu->REG[C] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x4A: //LD C,D
            cpu->REG[C] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x4B: //LD C,E
            cpu->REG[C] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x4C: //LD C,H
            cpu->REG[C] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x4D: //LD C,L
            cpu->REG[C] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x4E: //LD E,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[C] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x4F: //LD C,A
            cpu->REG[C] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x50: //LD D,B
            cpu->REG[D] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x51: //LD D,C
            cpu->REG[D] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x52: //LD D,D
            cpu->REG[D] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x53: //LD D,E
            cpu->REG[D] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x54: //LD D,H
            cpu->REG[D] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x55: //LD D,L
            cpu->REG[D] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x56: //LD D,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[D] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x57: //LD D,A
            cpu->REG[D] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x58: //LD E,B
            cpu->REG[E] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x59: //LD E,C
            cpu->REG[E] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x5A: //LD E,D
            cpu->REG[E] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x5B: //LD E,E
            cpu->REG[E] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x5C: //LD E,H
            cpu->REG[E] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x5D: //LD E,L
            cpu->REG[E] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x5E: //LD E,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[E] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x5F: //LD E,A
            cpu->REG[E] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x60: //LD H,B
            cpu->REG[H] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x61: //LD H,C
            cpu->REG[H] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x62: //LD H,D
            cpu->REG[H] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x63: //LD H,E
            cpu->REG[H] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x64: //LD H,H
            cpu->REG[H] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x65: //LD H,L
            cpu->REG[H] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x66: //LD H,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[H] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x67: //LD H,A
            cpu->REG[H] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x68: //LD L,B
            cpu->REG[L] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x69: //LD L,C
            cpu->REG[L] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x6A: //LD L,D
            cpu->REG[L] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x6B: //LD L,E
            cpu->REG[L] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x6C: //LD L,H
            cpu->REG[L] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x6D: //LD L,L
            cpu->REG[L] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x6E: //LD L,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[L] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x6F: //LD L,A
            cpu->REG[L] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x70: //LD (HL),B
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[B];
            cpu->wait = 8;
            break;
        case 0x71: //LD (HL),C
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[C];
            cpu->wait = 8;
            break;
        case 0x72: //LD (HL),D
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[D];
            cpu->wait = 8;
            break;
        case 0x73: //LD (HL),E
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[E];
            cpu->wait = 8;
            break;
        case 0x74: //LD (HL),H
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[H];
            cpu->wait = 8;
            break;
        case 0x75: //LD (HL),L
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[L];
            cpu->wait = 8;
            break;
        case 0x76: //HALT
            break;
        case 0x77: //LD (HL),A
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 8;
            break;
        case 0x78: //LD A,B
            cpu->REG[A] = cpu->REG[B];
            cpu->wait = 4;
            break;
        case 0x79: //LD A,C
            cpu->REG[A] = cpu->REG[C];
            cpu->wait = 4;
            break;
        case 0x7A: //LD A,D
            cpu->REG[A] = cpu->REG[D];
            cpu->wait = 4;
            break;
        case 0x7B: //LD A,E
            cpu->REG[A] = cpu->REG[E];
            cpu->wait = 4;
            break;
        case 0x7C: //LD A,H
            cpu->REG[A] = cpu->REG[H];
            cpu->wait = 4;
            break;
        case 0x7D: //LD A,L
            cpu->REG[A] = cpu->REG[L];
            cpu->wait = 4;
            break;
        case 0x7E: //LD A,(HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0x7F: //LD A,A
            cpu->REG[A] = cpu->REG[A];
            cpu->wait = 4;
            break;
        case 0x80:
            break;
        case 0x81:
            break;
        case 0x82:
            break;
        case 0x83:
            break;
        case 0x84:
            break;
        case 0x85:
            break;
        case 0x86:
            break;
        case 0x87:
            break;
        case 0x88:
            break;
        case 0x89:
            break;
        case 0x8A:
            break;
        case 0x8B:
            break;
        case 0x8C:
            break;
        case 0x8D:
            break;
        case 0x8E:
            break;
        case 0x8F:
            break;
        case 0x90:
            break;
        case 0x91:
            break;
        case 0x92:
            break;
        case 0x93:
            break;
        case 0x94:
            break;
        case 0x95:
            break;
        case 0x96:
            break;
        case 0x97:
            break;
        case 0x98:
            break;
        case 0x99:
            break;
        case 0x9A:
            break;
        case 0x9B:
            break;
        case 0x9C:
            break;
        case 0x9D:
            break;
        case 0x9E:
            break;
        case 0x9F:
            break;
        case 0xA0:
            break;
        case 0xA1:
            break;
        case 0xA2:
            break;
        case 0xA3:
            break;
        case 0xA4:
            break;
        case 0xA5:
            break;
        case 0xA6:
            break;
        case 0xA7:
            break;
        case 0xA8:
            break;
        case 0xA9:
            break;
        case 0xAA:
            break;
        case 0xAB:
            break;
        case 0xAC:
            break;
        case 0xAD:
            break;
        case 0xAE:
            break;
        case 0xAF:
            break;
        case 0xB0:
            break;
        case 0xB1:
            break;
        case 0xB2:
            break;
        case 0xB3:
            break;
        case 0xB4:
            break;
        case 0xB5:
            break;
        case 0xB6:
            break;
        case 0xB7:
            break;
        case 0xB8: //CP B
            cp(cpu->REG[A], cpu->REG[B], cpu);
            cpu->wait = 4;
            break;
        case 0xB9: //CP C
            cp(cpu->REG[A], cpu->REG[C], cpu);
            cpu->wait = 4;
            break;
        case 0xBA: //CP D
            cp(cpu->REG[A], cpu->REG[D], cpu);
            cpu->wait = 4;
            break;
        case 0xBB: //CP E
            cp(cpu->REG[A], cpu->REG[E], cpu);
            cpu->wait = 4;
            break;
        case 0xBC: //CP H
            cp(cpu->REG[A], cpu->REG[H], cpu);
            cpu->wait = 4;
            break;
        case 0xBD: //CP L
            cp(cpu->REG[A], cpu->REG[L], cpu);
            break;
        case 0xBE: //CP (HL)
            tempShort = getShortBigEndian(cpu->REG + H);
            tempByte = cpu->MEM[tempShort];
            cp(cpu->REG[A], tempByte, cpu);
            cpu->wait = 8;
            break;
        case 0xBF: //CP A
            cp(cpu->REG[A], cpu->REG[A], cpu);
            cpu->wait = 4;
            break;
        case 0xC0:
            break;
        case 0xC1:
            break;
        case 0xC2:
            break;
        case 0xC3: //JP a16
			tempShort = twoByte(cpu);
            printShort(tempShort);
            cpu->PC = tempShort;
            cpu->wait = 16;
            break;
        case 0xC4:
            break;
        case 0xC5:
            break;
        case 0xC6:
            break;
        case 0xC7:
            break;
        case 0xC8:
            break;
        case 0xC9:
            break;
        case 0xCA:
            break;
        case 0xCB:
            break;
        case 0xCC:
            break;
        case 0xCD:
            break;
        case 0xCE:
            break;
        case 0xCF:
            break;
        case 0xD0:
            break;
        case 0xD1:
            break;
        case 0xD2:
            break;
        case 0xD4:
            break;
        case 0xD5:
            break;
        case 0xD6:
            break;
        case 0xD7:
            break;
        case 0xD8:
            break;
        case 0xD9:
            break;
        case 0xDA:
            break;
        case 0xDC:
            break;
        case 0xDE:
            break;
        case 0xDF:
            break;
        case 0xE0: //LDH (a8),A
            tempByte = oneByteUnsigned(cpu);
            tempShort = 0xFF00 + tempByte;
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 8;
            break;
        case 0xE1:
            break;
        case 0xE2: //LD (C),A
            tempByte = cpu->REG[C];
            tempShort = 0xFF00 + tempByte;
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 8;
            break;
        case 0xE5:
            break;
        case 0xE6:
            break;
        case 0xE7:
            break;
        case 0xE8:
            break;
        case 0xE9:
            break;
        case 0xEA: //LD (a16),A
            tempShort = twoByte(cpu);
            cpu->MEM[tempShort] = cpu->REG[A];
            cpu->wait = 16;
            break;
        case 0xEE:
            break;
        case 0xEF:
            break;
        case 0xF0: //LDH A,(a8)
            tempByte = oneByteUnsigned(cpu);
            tempShort = 0xFF00 + tempByte;
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0xF1:
            break;
        case 0xF2: //LD A,(C)
            tempByte = cpu->REG[C];
            tempShort = 0xFF00 + tempByte;
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 8;
            break;
        case 0xF3:
            break;
        case 0xF5:
            break;
        case 0xF6:
            break;
        case 0xF7:
            break;
        case 0xF8: //LD HL,SP+r8
            tempSigned = oneByteSigned(cpu);
            saveShortBigEndian(cpu->REG + H, cpu->SP + tempSigned);
            //set flags
            cpu->FLAGS[ZF] = 0; //zero flag
            cpu->FLAGS[NF] = 0; //subtract flag
            if (tempSigned >= 0) {
                cpu->FLAGS[HF] = ((cpu->SP & 0xF) + (tempSigned & 0xF)) > 0xF; //half-carry flag
                cpu->FLAGS[CF] = ((cpu->SP & 0xFF) + tempSigned) > 0xFF; //carry flag
            } else {
                cpu->FLAGS[HF] = (cpu->SP & 0xF) <= (tempSigned & 0xF); //half-carry flag
                cpu->FLAGS[CF] = (cpu->SP & 0xFF) <= (tempSigned & 0xFF); //carry flag
            }
            cpu->wait = 12;
            break;
        case 0xF9: //LD SP,HL
            cpu->SP = getShortBigEndian(cpu->REG + H);
            cpu->wait = 8;
            break;
        case 0xFA: //LD A,(a16)
            tempShort = twoByte(cpu);
            cpu->REG[A] = cpu->MEM[tempShort];
            cpu->wait = 16;
            break;
        case 0xFB:
            break;
        case 0xFE: //CP d8
            readByte = oneByteUnsigned(cpu);
            cp(cpu->REG[A], readByte, cpu);
            cpu->wait = 8;
            break;
        case 0xFF:
            break;
        default:
            printf("File read error:");
            exit(3);
            break;
    }
    return 0;
}
