#ifndef CPU_H
#define CPU_H

#include "types.h"

// Constant positions of flags in flags array
#define CF 4
#define HF 5
#define NF 6
#define ZF 7

// Struct to hold the cpu state
typedef struct {
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

// Create and return a new cpu state
extern cpu_state* createCPU();
// Reset the given cpu state back to initial values
extern void resetCPU(cpu_state *cpu);
// Execute an instruction
extern int executeCPU(cpu_state *cpu);
// Cycle the cpu while instruction is taking place
// Used to emulate the time taken to exec each instruction
extern void cycleCPU(cpu_state *cpu);

#endif /* CPU_H */
