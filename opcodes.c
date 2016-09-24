/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include "main.h"

uint8 oneByte(cpu_state *cpu) {
    return readNextByte(cpu);
}

int8 oneByteSigned(cpu_state *cpu) {
    return (int8)readNextByte(cpu);
}

//read a short from memory
uint16 twoBytes(cpu_state *cpu) {
	uint16 lsb = (uint16)readNextByte(cpu); //read least significant byte first
	uint16 msb = ((uint16)readNextByte(cpu)) << 8; //then read most significant byte
    return lsb | msb;
}

//compare instruction
void cp(uint8 b, uint8 opcode, cpu_state *cpu) {
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

//jump to a 16bit address if condition is set
void jp_c(bool set, uint16 address, uint8 opcode, cpu_state *cpu) {
    if (set) {
        cpu->PC = address;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//jump relative to PC if condition is met
void jr_c_8(int8 address, bool set, uint8 opcode, cpu_state *cpu) {
    if (set) {
        cpu->PC += address;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//load 8 bit value into some register
void ld_8(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory
void ld_8_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and increment the value in the HL register
void ldi(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->registers.HL++;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and increment the value in the HL register
void ldi_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->registers.HL++;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some register and decrement the value in the HL register
void ldd(uint8 value, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->registers.HL--;
    cpu->wait = opcodes[opcode].cycles;
}

//load 8 bit value into some address in memory and decrement the value in the HL register
void ldd_m(uint8 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeByte(address, value, cpu);
    cpu->registers.HL--;
    cpu->wait = opcodes[opcode].cycles;
}

//load 16 bit value into some destination register
void ld_16(uint16 value, uint16 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = value;
    cpu->wait = opcodes[opcode].cycles;
}

//load 16 bit value into some addres in memory
void ld_16_m(uint16 value, uint16 address, uint8 opcode, cpu_state *cpu) {
    writeShort(address, value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//increment a byte in a register
void inc_8(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //zero flag
    (*reg + 1) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //negative flag
    clearFlag(NF, cpu);
    //half-carry flag
    ((*reg & 0xF) == 0xF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    *reg += 1;
    cpu->wait = opcodes[opcode].cycles;
}

//increment a byte at the memory location stored in HL
void inc_8_m(uint8 opcode, cpu_state *cpu) {
    //zero flag
    (readByte(cpu->registers.HL, cpu) + 1) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //negative flag
    clearFlag(NF, cpu);
    //half-carry flag
    ((readByte(cpu->registers.HL, cpu) & 0xF) == 0xF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    writeByte(cpu->registers.HL, readByte(cpu->registers.HL, cpu) + 1, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//increment a short
void inc_16(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //inc_16 doesn't set or clear any flags
    *reg += 1; //can't use "++" as it takes higher priority over pointer dereference
    cpu->wait = opcodes[opcode].cycles;
}

//decrement a byte in memory
void dec_8(uint8 *reg, uint8 opcode, cpu_state *cpu) {
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
void dec_8_m(uint8 opcode, cpu_state *cpu) {
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
void dec_16(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //dec_16 doesn't set or clear any flags
    *reg -= 1; //can't use "-- as it takes higher priority over pointer dereference
    cpu->wait = opcodes[opcode].cycles;
}

//add together some 8 bit unsigned value and the A register
void add_8(uint8 value, uint8 opcode, cpu_state *cpu) {
    //zero flag
    (cpu->registers.A + value) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    //negative flag
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
void sub_8(uint8 value, uint8 opcode, cpu_state *cpu) {
    //zero flag
    (cpu->registers.A == value) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    //negative flag
    setFlag(NF, cpu);
    //half-carry flag
    ((cpu->registers.A & 0xF) < (value & 0xF)) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //carry flag
    (cpu->registers.A < value) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //set value after flags are calculated
    cpu->registers.A -= value;
    cpu->wait = opcodes[opcode].cycles;
}

//add together two unsigned 16 bit values and set flags
void add_16(uint16 value, uint16 *reg, uint8 opcode, cpu_state *cpu) {
    //zero flag isn't touched
    //negative flag
    clearFlag(NF, cpu);
    //half-carry flag (in 16bit ALU the highest bytes set the CF last, so only check for the high byte 3 -> 4 bit carry)
    ((*reg & 0xFFF) + (value & 0xFFF) > 0xFFF) ? setFlag(HF, cpu) : clearFlag(HF, cpu);
    //carry flag
    ((*reg & 0xFF) + (value & 0xFF) > 0xFF) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //set value after flags are calculated
    *reg += value;
    cpu->wait = opcodes[opcode].cycles;
}

//8 bit add between the A register, some value, and the carry flag
void adc(uint8 value, uint8 opcode, cpu_state *cpu) {
    add_8(value + readFlag(CF, cpu), opcode, cpu);
}

//8 bit subtract between the A register, some value, and the carry flag
void sbc(uint8 value, uint8 opcode, cpu_state *cpu) {
    add_8(value + readFlag(CF, cpu), opcode, cpu);
}

//rotate register A left, old bit 7 to carry bit and bit 0
void rlca(uint8 opcode, cpu_state *cpu) {
    //set carry flag based on bit 7
    if (cpu->registers.A >> 7) {
        setFlag(CF, cpu);
    } else {
        clearFlag(CF, cpu);
    }
    //shift left and use carry flag/bit 7 as new bit 0
    cpu->registers.A = (cpu->registers.A << 1) | readFlag(CF, cpu);
    //zero flag
    (cpu->registers.A) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(HF, cpu);
    clearFlag(NF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//complement the A register
void cpl(uint8 opcode, cpu_state *cpu) {
    setFlag(NF, cpu);
    setFlag(HF, cpu);
    cpu->registers.A = ~cpu->registers.A;
    cpu->wait = opcodes[opcode].cycles;
}

//push a short onto the stack
void push(uint16 value, uint8 opcode, cpu_state *cpu) {
    writeShortToStack(value, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//pop a short from the stack
void pop(uint16 *reg, uint8 opcode, cpu_state *cpu) {
    *reg = readShortFromStack(cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//standard call. Save PC to the stack
void call_c(bool set, uint16 pointer, uint8 opcode, cpu_state *cpu) {
    if (set) {
        writeShortToStack(cpu->PC, cpu);
        cpu->PC = pointer;
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        cpu->wait = opcodes[opcode].cycles;
    }
}

//return after call. Restore PC from stack
void ret_c(bool set, uint8 opcode, cpu_state *cpu) {
    if (set) {
        cpu->PC = readShortFromStack(cpu);
        cpu->wait = opcodes[opcode].cyclesMax;
    } else {
        //do nothing
        cpu->wait = opcodes[opcode].cycles;
    }
}

//return after call and enable interrupts
void reti(uint8 opcode, cpu_state *cpu) {
    cpu->PC = readShortFromStack(cpu);
    cpu->wait = opcodes[opcode].cycles;
    cpu->ime = true;
}

//enable interrupts after this instruction and the next one are finished
void ei(uint8 opcode, cpu_state *cpu) {
    //make sure interrupts aren't enabled and aren't changing
    if (!cpu->ime && !cpu->imeCounter) {
        cpu->imeCounter = 2;
    }
    cpu->wait = opcodes[opcode].cycles;
}

//disable interrupts after this instruction and the next one are finished
void di(uint8 opcode, cpu_state *cpu) {
    //make sure interrupts are enabled and not changing
    if (cpu->ime && !cpu->imeCounter) {
        cpu->imeCounter = 2;
    }
    cpu->wait = opcodes[opcode].cycles;
}

//restart at given address. Save previous PC to the stack
void rst(uint8 pc, uint8 opcode, cpu_state *cpu) {
    writeShortToStack(cpu->PC, cpu);
    cpu->PC = (uint16)pc;
    cpu->wait = opcodes[opcode].cycles;
}

//xor A register with given value and set flags
void xor(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A ^= value;
    (cpu->registers.A) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(NF, cpu);
    clearFlag(HF, cpu);
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//perform logical and on the A register with a given value and set values
void and(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A &= value;
    //set zero flag if result is zero
    (!cpu->registers.A) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    clearFlag(NF, cpu);
    setFlag(HF, cpu);
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//perform logical or on the A register with a given value and set flags
void or(uint8 value, uint8 opcode, cpu_state *cpu) {
    cpu->registers.A |= value;
    //set zero flag if result is zero
    (!cpu->registers.A) ? setFlag(ZF, cpu) : clearFlag(ZF, cpu);
    clearFlag(NF, cpu);
    clearFlag(HF, cpu);
    clearFlag(CF, cpu);
    cpu->wait = opcodes[opcode].cycles;
}

//decimal adjust register A. Hex to Binary Coded Decimal. This makes the max
//value of every byte 9 to allow for easy translation into decimal values.
void daa(uint8 opcode, cpu_state *cpu) {
    uint8 correction = 0;
    //check whether the last operation was subtraction
    if (!readFlag(NF, cpu)) { //addition
        //correct lower byte. If value is greater than 9, push the excess to the
        //next byte. If half-carry flag set, restore the lost value. essentually,
        //half carry happens when the byte has a vlue greater than 15. 16 is then
        //carried over to the next byte. in BCD this is only a carry of 10, so
        //to correct one must restore the excess 6 to the lower byte.
        if (readFlag(HF, cpu) || ((cpu->registers.A & 0xF) > 0x09)) {
            correction += 0x06;
        }
        //correct upper byte. Similar to above regarding the situation with the
        //half-carry flag except in the larger byte this is the carry flag instead
        if (readFlag(CF, cpu) || (cpu->registers.A > 0x9F)) {
            correction += 0x60;
        }
        //carry flag - can only be set, not cleared
        if (((uint16)cpu->registers.A) + ((uint16)correction) > 0xFF) setFlag(CF, cpu);
        //assign value
        cpu->registers.A += correction;
    } else { //subtraction
        //correct lower byte. Same idea as explained above, but in reverse
        if (readFlag(HF, cpu)) {
            correction += 0x06;
        }
        //correct upper byte. Same idea as explained above, but in reverse
        if (readFlag(CF, cpu)) {
            correction += 0x60;
        }
        //carry flag - can only be set, not cleared
        if (cpu->registers.A < correction) setFlag(CF, cpu);
        //assign value
        cpu->registers.A -= correction;
    }
    //half-carry flag
    clearFlag(HF, cpu);
    //zero flag
    (cpu->registers.A) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    cpu->wait = opcodes[opcode].cycles;
    //printInstruction(true, cpu->PC - 1, cpu);
    //debug(cpu);
}

/* ==== CB PREFIX INSTRUCTIONS ==== */

//left shift a given register. Set new bit 0 to 0 and put the old bit 7 into the carry flag
void sla(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry flag based on 8th bit
    (*reg >> 7) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift left
    *reg = *reg << 1;
    //zero flag
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(HF, cpu);
    clearFlag(NF, cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//logical right shift of given register. Set new bit 7 to 0 and put the old bit 0 into carry flag
void srl(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    //set carry bit based on 1st bit
    (*reg & 0b1) ? setFlag(CF, cpu) : clearFlag(CF, cpu);
    //shift right
    *reg = *reg >> 1;
    //zero flag
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(HF, cpu);
    clearFlag(NF, cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//swap the upper and lower nibbles of some register
void swap(uint8 *reg, uint8 opcode, cpu_state *cpu) {
    clearFlag(NF, cpu);
    clearFlag(HF, cpu);
    clearFlag(CF, cpu);
    *reg = ((*reg & 0xF) << 4) | ((*reg & 0xF0) >> 4);
    (*reg) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set flags based on the status of a bit in a register
void bit(uint8 bit, uint8 *reg, uint8 opcode, cpu_state *cpu) {
    (readBit(bit, reg)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(NF, cpu);
    setFlag(HF, cpu);
    cpu->wait = cbOpcodes[opcode].cycles;
}

//set flags based on the status of a bit in memory
void bit_m(uint8 bit, uint8 opcode, cpu_state *cpu) {
    (readBitMem(bit, cpu)) ? clearFlag(ZF, cpu) : setFlag(ZF, cpu);
    clearFlag(NF, cpu);
    setFlag(HF, cpu);
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
        case 0x27: //SLA A
            sla(&cpu->registers.A, opcode, cpu);
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
        case 0x37: //SWAP A
            swap(&cpu->registers.A, opcode, cpu);
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
        case 0x47: //BIT 0, A
            bit(0, &cpu->registers.A, opcode, cpu);
            break;
        case 0x48: //BIT 1, B
            bit(1, &cpu->registers.B, opcode, cpu);
            break;
        case 0x49: //BIT 1, C
            bit(1, &cpu->registers.C, opcode, cpu);
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
        case 0x57: //BIT 2, A
            bit(2, &cpu->registers.A, opcode, cpu);
            break;
        case 0x58: //BIT 3, B
            bit(3, &cpu->registers.B, opcode, cpu);
            break;
        case 0x59: //BIT 3, C
            bit(3, &cpu->registers.C, opcode, cpu);
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
        case 0x67: //BIT 4, A
            bit(4, &cpu->registers.A, opcode, cpu);
            break;
        case 0x68: //BIT 5, B
            bit(5, &cpu->registers.B, opcode, cpu);
            break;
        case 0x69: //BIT 5, C
            bit(5, &cpu->registers.C, opcode, cpu);
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
            bit(6, &cpu->registers.A, opcode, cpu);
            break;
        case 0x86: //RES 0, (HL)
            res_m(0, opcode, cpu);
            break;
        case 0x87: //RES 0, A
            res(0, &cpu->registers.A, opcode, cpu);
            break;
        default: //instruction not implemented
            printf("Error prefix 0xCB instruction not found: ");
            printInstruction(false, cpu->PC - 2, cpu);
            return 1;
    }
    return 0;
}

//execute next instruction
int execute(cpu_state * cpu) {
    //printInstruction(cpu->PC, cpu);
    //grab instruction
    uint8 opcode = readNextByte(cpu);
    //find and execute next instruction
    //v2
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
        case 0x11: //LD DE, d16
            ld_16(twoBytes(cpu), &cpu->registers.DE, opcode, cpu);
            break;
        case 0x12: //LD (DE), A
            ld_8_m(cpu->registers.A, cpu->registers.DE, opcode, cpu);
            break;
        case 0x13: //INC DE
            inc_16(&cpu->registers.DE, opcode, cpu);
            break;
        case 0x15: //DEC D
            dec_8(&cpu->registers.D, opcode, cpu);
            break;
        case 0x16: //LD D, d8
            ld_8(oneByte(cpu), &cpu->registers.D, opcode, cpu);
            break;
        case 0x18: //JR r8
            jr_c_8(oneByteSigned(cpu), true, opcode, cpu);
            break;
        case 0x19: //ADD HL, DE
            add_16(cpu->registers.DE, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x1A: //LD A, (DE)
            ld_8(readByte(cpu->registers.DE, cpu), &cpu->registers.A, opcode, cpu);
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
        case 0x20: //JR NZ, r8
            jr_c_8(oneByteSigned(cpu), !readFlag(ZF, cpu), opcode, cpu);
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
        case 0x26: //LD H, d8
            ld_8(oneByte(cpu), &cpu->registers.H, opcode, cpu);
            break;
        case 0x27: //DAA
            daa(opcode, cpu);
            break;
        case 0x28: //JR Z,r8
            jr_c_8(oneByteSigned(cpu), readFlag(ZF, cpu), opcode, cpu);
            break;
        case 0x29: //ADD HL, HL
            add_16(cpu->registers.HL, &cpu->registers.HL, opcode, cpu);
            break;
        case 0x2A: //LDI A, (HL)
            ldi(readByte(cpu->registers.HL, cpu), &cpu->registers.A, opcode, cpu);
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
        case 0x30: //JR NC
            jr_c_8(oneByteSigned(cpu), !readFlag(CF, cpu), opcode, cpu);
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
            adc(cpu->registers.C, opcode, cpu);
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
        case 0xCD: //CALL a16
            call_c(true, twoBytes(cpu), opcode, cpu);
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
        case 0xD5: //PUSH DE
            push(cpu->registers.DE, opcode, cpu);
            break;
        case 0xD6: //SUB d8
            sub_8(oneByte(cpu), opcode, cpu);
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

    //v1
    /*switch (nextByte) {
        case 0x00: //NOP
            cpu->wait = 1;
            break;
        case 0x01: //LD BC,d16
            cpu->registers.BC = twoByte(cpu)
            //saveShortBigEndian(cpu->REG + B, twoByte(cpu));
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
        //TODO: output opcode and dump cpu info
            printf("File read error:");
            exit(3);
            break;
    }*/
