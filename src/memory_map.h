#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

// Menory Map
enum {
    HRAM_BASE = 0xFF80,
    IO_BASE = 0xFF00,
    UNUSABLE_BASE = 0xFEA0,
    OAM_BASE = 0xFE00,
    WRAM_ECHO_BASE = 0xE000,
    WRAM_SWITCHABLE_BASE = 0xD000,
    WRAM_FIXED_BASE = 0xC000,
    WRAM_BASE = 0xC000,
    EXTERNAL_RAM_BASE = 0xA000,
    BG_MAP_DATA2_BASE = 0x9C00,
    BG_MAP_DATA1_BASE = 0x9800,
    TILE_RAM_BASE = 0x8000,
    VRAM_BASE = 0x8000,
    ROM_SWITCHABLE_BASE = 0x4000,
    CART_HEADER_BASE = 0x0100,
    ROM_FIXED_BASE = 0x0000
};

// Memory bounds
enum {
    HRAM_BOUND = 0x7F,
    IO_BOUND = 0x80,
    UNUSABLE_BOUND = 0x60,
    OAM_BOUND = 0xA0,
    WRAM_ECHO_BOUND = 0x1E00,
    WRAM_SWITCHABLE_BOUND = 0x1000,
    WRAM_FIXED_BOUND = 0x1000,
    WRAM_BOUND = 0x3E00,
    EXTERNAL_RAM_BOUND = 0x2000,
    BG_MAP_DATA2_BOUND = 0x400,
    BG_MAP_DATA1_BOUND = 0x400,
    TILE_RAM_BOUND = 0x1800,
    VRAM_BOUND = 0x2000,
    ROM_SWITCHABLE_BOUND = 0x4000,
    CART_HEADER_BOUND = 0x50,
    ROM_FIXED_BOUND = 0x4000
};

// Sizes
enum {
    ROM_BANK_SIZE = 0x4000,
    RAM_BANK_SIZE = 0x2000,
    WRAM_BANK_SIZE = 0x1000,
    VRAM_BANK_SIZE = 0x2000
};

// Memory locations
enum {
    INTERRUPTS_ENABLED = 0xFFFF,
    WINDOW_X = 0xFF4B,
    WINDOW_Y = 0xFF4A,
    SP_PALETTE_1 = 0xFF49,
    SP_PALETTE_0 = 0xFF48,
    BG_PALETTE = 0xFF47,
    DMA = 0xFF46,
    SYC = 0xFF45,
    SCANLINE = 0xFF44,
    SCROLL_X = 0xFF43,
    SCROLL_Y = 0xFF42,
    STAT = 0xFF41,
    LCDC = 0xFF40,
    /* WAVE PATTERNS 0xFF30 to 0xFF3F */
    NR_52 = 0xFF26,
    NR_51 = 0xFF25,
    NR_50 = 0xFF24,
    NR_44 = 0xFF23,
    NR_43 = 0xFF22,
    NR_42 = 0xFF21,
    NR_41 = 0xFF20,
    NR_34 = 0xFF1E,
    NR_33 = 0xFF1D,
    NR_32 = 0xFF1C,
    NR_31 = 0xFF1B,
    NR_30 = 0xFF1A,
    NR_24 = 0xFF19,
    NR_23 = 0xFF18,
    NR_22 = 0xFF17,
    NR_21 = 0xFF16,
    NR_14 = 0xFF14,
    NR_13 = 0xFF13,
    NR_12 = 0xFF12,
    NR_11 = 0xFF11,
    NR_10 = 0xFF10,
    INTERRUPT_FLAGS = 0xFF0F,
    TAC = 0xFF07,
    TMA = 0xFF06,
    TIMA = 0xFF05,
    DIV = 0xFF04,
    SC = 0xFF02,
    SB = 0xFF01,
    JOYPAD = 0xFF00
};

#endif