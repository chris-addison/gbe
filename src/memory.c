#include "memory.h"
#include "types.h"
#include "cpu.h"
#include "mbc.h"
#include "screen.h"
#ifdef DISPLAY
    #include "display.h"
#endif
#include "joypad.h"
#include "interrupts.h"
#include <stdio.h>

// Handle reads from IO registers
static uint8 readIORegisters(uint16 address, Cpu *cpu) {
    uint8 index = address - IO_BASE;
    if (index > IO_BOUND) {
        printf("Error: readIORegisters passed incorrect address: %X\n", address);
    }
    switch (address) {
        // Redirected reads
        case JOYPAD:
            return getJoypadState(cpu);
        // Masked reads
        case STAT:
            return cpu->memory.io[index] | 0x80;
        case INTERRUPT_FLAGS:
            return cpu->memory.io[index] | 0xE0;
        case TAC:
            return cpu->memory.io[index] | 0xF8;
        // Pass through reads
        case WINDOW_X:
        case WINDOW_Y:
        case SP_PALETTE_1:
        case SP_PALETTE_0:
        case BG_PALETTE:
        case SYC:
        case SCANLINE:
        case SCROLL_X:
        case SCROLL_Y:
        case LCDC:
        case TMA:
        case TIMA:
        case DIV:
            return cpu->memory.io[index];
        // Everything else
        default:
            // If IO register doesn't have read permissions or isn't defined, return 0xFF
            return 0xFF;
    }
}

// Read a byte from a given memory address
uint8 readByte(uint16 address, Cpu *cpu) {
    if (address < ROM_FIXED_BASE + ROM_FIXED_BOUND) {
        // Cartridge base
        return cpu->memory.rom[address - ROM_FIXED_BASE];
    } else if (address < ROM_SWITCHABLE_BASE + ROM_SWITCHABLE_BOUND) {
        // Cartridge bank
        return cpu->memory.romBank[address - ROM_SWITCHABLE_BASE];
    } else if (address < VRAM_BASE + VRAM_BOUND) {
        // Vram
        return cpu->memory.vramBank[address - VRAM_BASE];
    } else if (address < EXTERNAL_RAM_BASE + EXTERNAL_RAM_BOUND) {
        // Cartridge ram
        return cpu->readMBC(address, cpu);
    } else if (address < WRAM_BASE + WRAM_BOUND) {
        // Working ram
        if (address < WRAM_FIXED_BASE + WRAM_FIXED_BOUND) {
            return cpu->memory.wram[address - WRAM_FIXED_BASE];
        } else if (address < WRAM_SWITCHABLE_BASE + WRAM_SWITCHABLE_BOUND) {
            return cpu->memory.wramBank[address - WRAM_SWITCHABLE_BASE];
        } else if (address < WRAM_ECHO_BASE + WRAM_FIXED_BOUND) {
            return cpu->memory.wram[address - WRAM_ECHO_BASE];
        } else {
            return cpu->memory.wramBank[address - WRAM_ECHO_BASE];
        }
    } else if (address < OAM_BASE + OAM_BOUND) {
        // Oam (only readable in STAT modes 0 and 1)
        // TODO: limit reading to those modes
        return cpu->memory.oam[address - OAM_BASE];
    } else if (address < UNUSABLE_BASE + UNUSABLE_BOUND) {
        // Unusable
        printf("Error: read from unusable memory!\n");
        return 0xFF;
    } else if (address < IO_BASE + IO_BOUND) {
        // Io
        return readIORegisters(address, cpu);
    } else if (address < HRAM_BASE + HRAM_BOUND) {
        return cpu->memory.hram[address - HRAM_BASE];
    } else {
        // 0xFFFF IE
        return cpu->memory.ie;
    }
}

// Read next byte from the instruction pointer, then increment it
uint8 readNextByte(Cpu *cpu) {
    uint8 byte = readByte(cpu->PC, cpu);
    if (cpu->halt_bug) {
        cpu->halt_bug = false;
    } else {
        cpu->PC++;
    }
    return byte;
}

void transferOAM(uint8 value, Cpu *cpu) {
    //printf("DMA\n");
    uint16 address = ((uint16) value) << 8;
    //printf("address: 0x%X\n", address);
    for (uint8 i = 0; i < 160; i++) {
        cpu->memory.oam[i] = readByte(address + i, cpu);
    }
    // Transfer OAM takes 160 cycles.
    cpu->wait += 160;
}

static void writeIORegisters(uint16 address, uint8 value, Cpu *cpu) {
    uint8 index = address - IO_BASE;
    if (index > IO_BOUND) {
        printf("Error: writeIORegisters passed incorrect address: %X\n", address);
        return;
    }
    switch (address) {
        // Redirected writes
        case DMA:
            transferOAM(value, cpu);
            break;
        #ifdef DISPLAY
            case BG_PALETTE:
                updateBackgroundColour(value);
                cpu->memory.io[index] = value;
                break;
            case SP_PALETTE_0:
                updateSpritePalette(0, value);
                cpu->memory.io[index] = value;
                break;
            case SP_PALETTE_1:
                updateSpritePalette(1, value);
                cpu->memory.io[index] = value;
                break;
        #endif
        // Masked writes
        case JOYPAD:
            //TODO: replace this with a updateJoypad function
            cpu->memory.io[index] &= 0x0F;
            cpu->memory.io[index] |= value & 0xF0;
            break;
        case STAT:
            cpu->memory.io[index] &= 0x7;
            cpu->memory.io[index] |= value & 0xF8;

            // Bug in grey GameBoy
            // http://gbdev.gg8.se/wiki/articles/Video_Display#FF41_-_STAT_-_LCDC_Status_.28R.2FW.29
            if ((cpu->memory.io[index] & 0x3) < 2 && cpu->memory.io[LCDC - IO_BASE] & 0x80) {
                setInterruptFlag(INTR_STAT, cpu);
            }
            break;
        case DIV:
            cpu->memory.io[index] = 0;
            break;
        // Pass through writes
        case WINDOW_X:
        case WINDOW_Y:
        #ifndef DISPLAY
            case SP_PALETTE_1:
            case SP_PALETTE_0:
            case BG_PALETTE:
        #endif
        case SYC:
        case SCANLINE:
        case SCROLL_X:
        case SCROLL_Y:
        case LCDC:
        case TAC:
        case TMA:
        case TIMA:
        case SB:
        case INTERRUPT_FLAGS:
            cpu->memory.io[index] = value;
            break;
        // Everything else
        default:
            return;
    }
}

//write a byte to the given memory address
void writeByte(uint16 address, uint8 value, Cpu *cpu) {
    if (address < ROM_SWITCHABLE_BASE + ROM_SWITCHABLE_BOUND) {
        // Cartridge and cartridge bank
        return cpu->writeMBC(address, value, cpu);
    } else if (address < VRAM_BASE + VRAM_BOUND) {
        // Vram
        cpu->memory.vramBank[address - VRAM_BASE] = value;
    } else if (address < EXTERNAL_RAM_BASE + EXTERNAL_RAM_BOUND) {
        // Cartridge ram
        if (cpu->RAM_enable) {
            cpu->memory.ramBank[address - EXTERNAL_RAM_BASE] = value;
        }
    } else if (address < WRAM_BASE + WRAM_BOUND) {
        // Working ram
        if (address < WRAM_FIXED_BASE + WRAM_FIXED_BOUND) {
            cpu->memory.wram[address - WRAM_FIXED_BASE] = value;
        } else if (address < WRAM_SWITCHABLE_BASE + WRAM_SWITCHABLE_BOUND) {
            cpu->memory.wramBank[address - WRAM_SWITCHABLE_BASE] = value;
        } else if (address < WRAM_ECHO_BASE + WRAM_FIXED_BOUND) {
            cpu->memory.wram[address - WRAM_ECHO_BASE] = value;
        } else {
            cpu->memory.wramBank[address - WRAM_ECHO_BASE] = value;
        }
    } else if (address < OAM_BASE + OAM_BOUND) {
        // Oam (only writable in STAT modes 0 and 1)
        // TODO: limit writing to those modes
        cpu->memory.oam[address - OAM_BASE] = value;
    } else if (address < UNUSABLE_BASE + UNUSABLE_BOUND) {
        // Unusable
        printf("Error: write to unusable memory!\n");
        return;
    } else if (address < IO_BASE + IO_BOUND) {
        // Io
        writeIORegisters(address, value, cpu);
    } else if (address < HRAM_BASE + HRAM_BOUND) {
        cpu->memory.hram[address - HRAM_BASE] = value;
    } else {
        // 0xFFFF IE
        cpu->memory.ie = value;
    }
}

//read a short from a given memory address
uint16 readShort(uint16 address, Cpu *cpu) {
    return (((uint16) readByte(address + 1, cpu)) << 8) + readByte(address, cpu);
}

//write a short to the given memory address
void writeShort(uint16 address, uint16 value, Cpu *cpu) {
    writeByte(address, value & 0xFF, cpu); //Least significant byte
    writeByte(address + 1, (value >> 8) & 0xFF, cpu); //Most significant byte
}

//move the stack pointer and save a short to the stack
void writeShortToStack(uint16 value, Cpu *cpu) {
    cpu->SP -= 2;
    writeShort(cpu->SP, value, cpu);
}

//read a short from the stack and move the stack pointer
uint16 readShortFromStack(Cpu *cpu) {
    uint16 value = readShort(cpu->SP, cpu);
    cpu->SP += 2;
    return value;
}

//set a flag
void setFlag(uint8 flag, Cpu *cpu) {
    //create a mask for the flag, then OR it to set the flag to one
    cpu->registers.F |= (0b1 << flag);
}

//clear a flag
void clearFlag(uint8 flag, Cpu *cpu) {
    /* Reset Flag */
    //create a mask for the flag, inverse it, then AND it to set the flag to zero
    cpu->registers.F &= ~(0b1 << flag);
}

//return true if a flag is set. Return false otherwise.
bool readFlag(uint8 flag, Cpu *cpu) {
    //get flags register, shift down to correct flag offset and compare
    return ((cpu->registers.F >> flag) & 0b1) == 0b1;
}

//return true if a bit in a register is set. Return false otherwise.
bool readBit(uint8 bit, uint8 *reg) {
    //get register, shift down to correct flag offset
    return (*reg >> bit) & 0b1;
}

//return true if a bit in a byte at the memory address stored in HL is set. Return false otherwise.
bool readBitMem(uint8 bit, Cpu *cpu) {
    //get register, shift down to correct flag offset
    return (readByte(cpu->registers.HL, cpu) >> bit) & 0b1;
}