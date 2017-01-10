#include "memory.h"
#include "types.h"
#include "cpu.h"
#include "screen.h"
#ifdef DISPLAY
    #include "display.h"
#endif
#include "joypad.h"
#include <stdio.h>

//read a byte from a given memory address
uint8 readByte(uint16 address, cpu_state *cpu) {
    if (cpu->cart_type == 0x00 || address < 0x4000 || address > 0xC000) {
        if (address == JOYPAD) {
            return getJoypadState(cpu);
        }
        return cpu->MEM[address];
    } else if (address < 0x8000) { //handle mbc here
        //get address in rom bank
        address -= 0x4000;
        //printf("%X\n", address + (cpu->ROM_bank * 0x4000));
        return cpu->CART_ROM[address + (cpu->ROM_bank * 0x4000)];
    } else if (address < 0xA000) {
        return cpu->MEM[address];
    } else if (cpu->RAM_enable && address < 0xC000) {
        //mbc 2 has a single 256 byte RAM bank and mbc 1 has the option of having 1 1/4 sized RAM bank
        if ((cpu->mbc == 2 && address < 0xA200) || (cpu->mbc == 1 && cpu->mbc1_small_ram && address < 0xA800)) {
            address -= 0xA000;
            return cpu->CART_RAM[address];
        } else if ((cpu->mbc == 1 && !cpu->mbc1_small_ram) || cpu->mbc == 3 || cpu->mbc == 5) {
            address -= 0xA000;
            return cpu->CART_RAM[address + (cpu->RAM_bank * 0x2000)];
        } else {
            return cpu->MEM[address];
        }
    } else { // if no cases match default
        return cpu->MEM[address];
    }
}

//read next byte from the instruction pointer
uint8 readNextByte(cpu_state *cpu) {
    uint8 byte = readByte(cpu->PC, cpu);
    cpu->PC++;
    return byte;
}

void transferOAM(uint8 value, cpu_state *cpu) {
    //printf("DMA\n");
    uint16 address = ((uint16) value) << 8;
    //printf("address: 0x%X\n", address);
    for (uint8 i = 0; i < 160; i++) {
        //if (cpu->MEM[address + i]) {
            //printf("something here! 0x%X\n", cpu->MEM[address + i]);
        //}
        cpu->MEM[0xFE00 + i] = cpu->MEM[address + i];
    }
    // Transfer OAM takes 160 cycles.
    cpu->wait += 160;
}

//write a byte to the given memory address
void writeByte(uint16 address, uint8 value, cpu_state *cpu) {
    if (address >= 0xED00 && address < 0xED00 + 160) {
            printf("Writing OAM byte now: 0x%X\n", value);
        }
    //handle the echo of internal memory at 0xC000->0xDDFF and 0xE000->0xFDFF
    if (address >= 0xC000 && address < 0xDE00) {
        cpu->MEM[address + 0x2000] = value;
    } else if (address >= 0xE000 && address < 0xFE00) {
        if (address >= 0xED00 && address < 0xED00 + 160) {
            printf("Writing OAM byte now: 0x%X\n", value);
        }
        cpu->MEM[address - 0x2000] = value;
    }
    if (cpu->cart_type == 0x00 || address >= 0xC000) {
        // Update palette for the background
        #ifdef DISPLAY
            if (address == BG_PALETTE) {
                updateBackgroundColour(value);
            } else if (address == SP_PALETTE_0) {
                updateSpritePalette(0, value);
            } else if (address == SP_PALETTE_1) {
                updateSpritePalette(1, value);
            }
        #endif
        if (address == DMA) {
            transferOAM(value, cpu);
        } else if (address == DIV) {
            value = 0;
        }
        cpu->MEM[address] = value;
    } else { //handle the mbcs here
        //printf("%X:\thandle mbc address = %X, value = %X\n", cpu->PC, address, value);
        if (address < 0x2000) {
            //enable/diable cartridge RAM. If no ram, never enable.
            cpu->RAM_enable = ((value & 0xA) == 0xA && cpu->RAM_exists);
            //printf("RAM ENABLE/DISABLE\n");
        } else if (address < 0x3000 && cpu->mbc == 5) {
            if (value == 0x00) { //bank "0" is mapped to bank 1
                value = 0x01;
            }
            //clear lower byte of rom bank so it can be set
            cpu->ROM_bank &= 0xFF00;
            cpu->ROM_bank |= value;
            //printf("%x CHANGE ROM BANK: %d\n", cpu->PC, value);
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
            //printf("%x CHANGE ROM BANK: %d\n", cpu->PC, value);
        } else if (address < 0x6000) {
            //select ram bank
            cpu->RAM_bank = value;
            //printf("%x CHANGE RAM BANK: %d\n", cpu->PC, value);
        } else if (address < 0x8000) {
            if (cpu->mbc == 1) {
                //printf("CHANGE RAM/ROM MODE ON MBC1\n");
                cpu->mbc1_mode = value;
            } else {
                //do nothing for now
                printf("MMU Address not implemented! %d\n", address);
                //exit(246);
            }
        } else if (address < 0xA000) {
            //printf("Write normally\n");
            cpu->MEM[address] = value;
        } else if (cpu->RAM_enable && address < 0xC000) { // check if ram bank exists
            //mbc 2 has a single 256 byte RAM bank and mbc 1 has the option of having 1 1/4 sized RAM bank
            if ((cpu->mbc == 2 && address < 0xA200) || (cpu->mbc == 1 && cpu->mbc1_small_ram && address < 0xA800)) {
                address -= 0xA000;
                cpu->CART_RAM[address] = value;
            } else if ((cpu->mbc == 1 && !cpu->mbc1_small_ram) || cpu->mbc == 3 || cpu->mbc == 5) {
                address -= 0xA000;
                cpu->CART_RAM[address + (cpu->RAM_bank * 0x2000)] = value;
            } else {
                cpu->MEM[address] = value;
            }
        } else {
            cpu->MEM[address] = value;
        }
    }
}

//read a short from a given memory address
uint16 readShort(uint16 address, cpu_state *cpu) {
    return (((uint16) readByte(address + 1, cpu)) << 8) + readByte(address, cpu);
}

//write a short to the given memory address
void writeShort(uint16 address, uint16 value, cpu_state *cpu) {
    writeByte(address, value & 0xFF, cpu); //Least significant byte
    writeByte(address + 1, (value >> 8) & 0xFF, cpu); //Most significant byte
}

//move the stack pointer and save a short to the stack
void writeShortToStack(uint16 value, cpu_state *cpu) {
    cpu->SP -= 2;
    writeShort(cpu->SP, value, cpu);
}

//read a short from the stack and move the stack pointer
uint16 readShortFromStack(cpu_state *cpu) {
    uint16 value = readShort(cpu->SP, cpu);
    cpu->SP += 2;
    return value;
}

//set a flag
void setFlag(uint8 flag, cpu_state *cpu) {
    //create a mask for the flag, then OR it to set the flag to one
    cpu->registers.F |= (0b1 << flag);
}

//clear a flag
void clearFlag(uint8 flag, cpu_state *cpu) {
    /* Reset Flag */
    //create a mask for the flag, inverse it, then AND it to set the flag to zero
    cpu->registers.F &= ~(0b1 << flag);
}

//return true if a flag is set. Return false otherwise.
bool readFlag(uint8 flag, cpu_state *cpu) {
    //get flags register, shift down to correct flag offset and compare
    return ((cpu->registers.F >> flag) & 0b1) == 0b1;
}

//return true if a bit in a register is set. Return false otherwise.
bool readBit(uint8 bit, uint8 *reg) {
    //get register, shift down to correct flag offset
    return (*reg >> bit) & 0b1;
}

//return true if a bit in a byte at the memory address stored in HL is set. Return false otherwise.
bool readBitMem(uint8 bit, cpu_state *cpu) {
    //get register, shift down to correct flag offset
    return (readByte(cpu->registers.HL, cpu) >> bit) & 0b1;
}