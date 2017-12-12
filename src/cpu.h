#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "memory_map.h"

// Constant positions of flags in flags array
enum {
    CF = 4,
    HF = 5,
    NF = 6,
    ZF = 7
};

typedef struct Cpu Cpu;

// Struct to hold the cpu state
struct Cpu {
    struct Memory {
        uint8 oam[OAM_BOUND];
        uint8 io[IO_BOUND];
        uint8 hram[HRAM_BOUND];
        uint8 *romBank;
        uint8 *ramBank;
        uint8 *wramBank;
        uint8 *vramBank;
        uint8 *rom; // malloc'ed
        uint8 *ram; // malloc'ed
        uint8 *wram; // malloc'ed
        uint8 *vram; // malloc'ed
        uint8 ie;
    } memory;
    struct Registers {
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
    uint8 (*readMBC)(uint16 address, Cpu *cpu);
    void (*writeMBC)(uint16 address, uint8 value, Cpu *cpu);
    uint16 PC;
    uint16 SP;
    uint16 cycles;
    uint16 currentRomBank;
    uint16 maxRomBank;
    uint8 currentRamBank;
    uint8 maxRamBank;
    uint8 cart_type;
    uint8 mbc;
    uint8 wait;
    bool halt;
    bool halt_bug;
    bool RAM_enable;
    bool RAM_exists;
    bool mbc1_mode;
    bool mbc1_small_ram;
    bool ime;
    bool ime_enable;
};

// Create and return a new cpu state
extern Cpu *createCPU();
// Reset the given cpu state back to initial values
extern void resetCPU(Cpu *cpu);
// Execute an instruction
extern int executeCPU(Cpu *cpu);
// Cycle the cpu while instruction is taking place
// Used to emulate the time taken to exec each instruction
extern void cycleCPU(Cpu *cpu);

#endif /* CPU_H */
