/* -*-mode:c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#ifndef COMMON_C
#define COMMON_C
#include "main.h"

//read a byte from a given memory address
static uint8 readByte(uint16 address, cpu_state *cpu) {
    if (cpu->cart_type == 0x00 || address < 0x4000 || address > 0xC000) {
        return cpu->MEM[address];
    } else if (address < 0x8000) { //handle mbc here
        //get address in rom bank
        address -= 0x4000;
        return cpu->CART_ROM[address + (cpu->ROM_bank * 0x4000)];
    } else if (address < 0xA000) {
        return cpu->MEM[address];
    } else if (cpu->ext_ram_enable && address < 0xC000) {
        if (cpu->ext_ram_enable) {
            //get address in rom bank
            address -= 0xA000;
            return cpu->CART_RAM[address + (cpu->RAM_bank * 0x2000)];
        } else {
            return cpu->MEM[address];
        }
    }
}

//read next byte from the instruction pointer
static uint8 readNextByte(cpu_state *cpu) {
    uint8 byte = readByte(cpu->PC, cpu);
    cpu->PC++;
    return byte;
}

//write a byte to the given memory address
static void writeByte(uint16 address, uint8 value, cpu_state *cpu) {
    //handle the echo of internal memory at 0xC000->0xDDFF and 0xE000->0xFDFF
    if (address >= 0xC000 && address < 0xDE00) {
        cpu->MEM[address + 0x2000] = value;
    } else if (address >= 0xE000 && address < 0xFE00) {
        cpu->MEM[address - 0x2000] = value;
    }
    if (cpu->cart_type == 0x00 || address >= 0xC000) {
        cpu->MEM[address] = value;
    } else { //handle the mbcs here
        printf("handle mbc\n");
        if (address < 0x2000) {
            //enable/diable cartridge RAM
            cpu->ext_ram_enable = (value == 0x0A);
            printf("RAM ENABLE/DISABLE\n");
        } else if (address < 0x3000 && cpu->mbc == 5) {
            if (value == 0x00) { //bank "0" is mapped to bank 1
                value = 0x01;
            }
            //clear lower byte of rom bank so it can be set
            cpu->ROM_bank &= 0xFF00;
            cpu->ROM_bank |= value;
        } else if (address < 0x4000) {
            if (value == 0x00) { //bank "0" is mapped to bank 1
                value = 0x01;
            } else if (cpu->mbc == 1 && (value == 0x20 || value == 0x40 || value == 0x60)) {
                value += 1; //mbc 1 doesn't have banks 0x20, 0x40, 0x60, so map to the bank after
            }
            if (cpu->mbc == 5) { //in mbc 5 0x3000 -> 0x3FFF sets the 9th bit of the rom bank
                if (value & 0b1) {
                    cpu->ROM_bank |= 0b1 << 8; //set bit
                } else {
                    cpu->ROM_bank &= ~(0b1 << 8); //reset bit
                }
            } else {
                cpu->ROM_bank = value;
            }
            printf("%x CHANGE ROM BANK: %d\n", cpu->PC, value);
        } else if (address < 0x6000) {
            //select ram bank
            cpu->RAM_bank = value;
            printf("%x CHANGE RAM BANK: %d\n", cpu->PC, value);
        } else if (address < 0x8000) {
            //do nothing for now
            printf("Not implemented!\n");
            exit(246);
        } else if (address < 0xA000) {
            cpu->MEM[address] = value;
        } else if (address < 0xC000) {
            address -= 0xA000;
            cpu->CART_RAM[address + (cpu->RAM_bank * 0x2000)] = value;
        }
    }
}

//read a short from a given memory address
static uint16 readShort(uint16 address, cpu_state *cpu) {
    return (((uint16) readByte(address + 1, cpu)) << 8) + readByte(address, cpu);
}

//write a short to the given memory address
static void writeShort(uint16 address, uint16 value, cpu_state *cpu) {
    writeByte(address, value & 0xFF, cpu); //Least significant byte
    writeByte(address + 1, (value >> 8) & 0xFF, cpu); //Most significant byte
}

//move the stack pointer and save a short to the stack
static void writeShortToStack(uint16 value, cpu_state *cpu) {
    cpu->SP -= 2;
    writeShort(cpu->SP, value, cpu);
}

//read a short from the stack and move the stack pointer
static uint16 readShortFromStack(cpu_state *cpu) {
    uint16 value = readShort(cpu->SP, cpu);
    cpu->SP += 2;
    return value;
}

//set a flag
static void setFlag(uint8 flag, cpu_state *cpu) {
    //create a mask for the flag, then OR it to set the flag to one
    cpu->registers.F |= (0b1 << flag);
}

//clear a flag
static void clearFlag(uint8 flag, cpu_state *cpu) {
    /* Reset Flag */
    //create a mask for the flag, inverse it, then AND it to set the flag to zero
    cpu->registers.F &= ~(0b1 << flag);
}

//return true if a flag is set. Return false otherwise.
static bool readFlag(uint8 flag, cpu_state *cpu) {
    //get flags register, shift down to correct flag offset and compare
    return ((cpu->registers.F >> flag) & 0b1) == 0b1;
}

//return true if a bit in a register is set. Return false otherwise.
static bool readBit(uint8 bit, uint8 *reg) {
    //get register, shift down to correct flag offset and compare
    return ((*reg >> bit) & 0b1) == 0b1;
}

//return true if a bit in a byte at the memory address stored in HL is set. Return false otherwise.
static bool readBitMem(uint8 bit, cpu_state *cpu) {
    //get register, shift down to correct flag offset and compare
    return ((readByte(cpu->registers.HL, cpu) >> bit) & 0b1) == 0b1;
}

//print unsigned byte to standard input
static void printByte(uint8 byte) {
    printf("0x%02" PRIX8, byte);
}

//print signed byte to standard input
static void printByteSigned(int8 byte) {
    printf("0x%02" PRIi8, byte);
}

//print 2 bytes to standard input
static void printShort(uint16 twoBytes) {
    printf("0x%04X", twoBytes);
}

//print given instruction
static void printInstruction(bool showPC, uint16 PC, cpu_state *cpu) {
    if (showPC) {
        printf("0x%04X:  ", PC);
    }
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        printf("%s\n", cbOpcodes[readByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        printf("%s\n", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        printf(opcodes[opcode].name, readByte(PC + 1, cpu));
        printf("\n");
    } else { //print three byte instruction
        printf(opcodes[opcode].name, readShort(PC + 1, cpu));
        printf("\n");
    }
}

//print given instruction to file
static void printInstructionToFile(uint16 PC, FILE *file, cpu_state *cpu) {
    //skip the wait instructions
	if (PC >= 0x36C && PC <= 0x36F) {
		return;
	}
    fprintf(file, "0x%04X:  ", PC);
    uint8 opcode = readByte(PC, cpu);
    if (opcode == 0xCB) { //print CB prefix instruction
        fprintf(file, "%s\n", cbOpcodes[readByte(PC + 1, cpu)].name);
    } else if (opcodes[opcode].bytes == 1) { //print single byte instruction
        fprintf(file, "%s\n", opcodes[opcode].name);
    } else if (opcodes[opcode].bytes == 2) { //print two byte instruction
        fprintf(file, opcodes[opcode].name, readByte(PC + 1, cpu));
        fprintf(file, "\n");
    } else { //print three byte instruction
        fprintf(file, opcodes[opcode].name, readShort(PC + 1, cpu));
        fprintf(file, "\n");
    }
}

#endif /* COMMON_C */
