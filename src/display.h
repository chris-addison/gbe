#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "cpu.h"

#ifdef X11
    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <X11/keysym.h>
#endif
#ifdef OPENGL
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glu.h>
#endif
#ifdef SDL
    #include <SDL2/SDL.h>
#endif

#define DISPLAY_HEIGHT 144
#define DISPLAY_WIDTH 160

#define WINDOW_HEIGHT 288
#define WINDOW_WIDTH 320

extern void startDisplay();
extern void loadTiles(cpu_state *cpu);
extern void loadScanline(uint8 scanLine, cpu_state *cpu);
extern void draw(cpu_state *cpu);
extern void stopDisplay();

#endif /* DISPLAY_H */
