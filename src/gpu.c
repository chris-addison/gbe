// 8000-8FFF and at 8800-97FF
if (cpu->PC == 0x282a) { // Tetris finish writing to Tiles (Address from: http://cturt.github.io/cinoop.html)
    uint8 *vram = cpu->MEM + 0x8000;
    for (int i = 0; i < 16; i++) {
        printf("%.2X ", *(vram + i));
    }
    printf("\n");
    // Print out tiles
    for (int i = 0; i < 16*256; i += 2) {
        uint8 byteLow = *(vram + i);
        uint8 byteHigh = *(vram + i + 1);
        for (int j = 0; j < 8; j++) {
            uint8 pixel = ((byteLow & 0x80) >> 7) | ((byteHigh & 0x80) >> 6);
            if (pixel) {
                printf("0%X ", pixel);
            } else {
                printf("   ");
            }
            byteLow <<= 1;
            byteHigh <<= 1;
        }
        printf("\n");
    }
    exit(1);
}
