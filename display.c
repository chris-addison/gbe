#include "main.h"

#ifdef LINUX
    Display *display;
    Window window;
    int screen;
    XEvent event;
    XWindowAttributes windowAttributes = {0};
    XSetWindowAttributes setWindowAttributes;
#endif
#ifdef OPENGL
    Window root;
    GLint attributes[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    XVisualInfo *visualInfo;
    Colormap colormap;
    GLXContext glContext;
    GLuint textureID;
#endif

uint8 frameBuffer[3 * DISPLAY_WIDTH * DISPLAY_HEIGHT];
uint8 tiles[384][8][8];
//uint8 tileMap[1024][8][8];

void startDisplay() {
    #ifdef LINUX
        display = XOpenDisplay(NULL);
        if (display == NULL) {
            printf("X11: failue to open display\n");
            exit(11);
        }

        visualInfo = glXChooseVisual(display, 0, attributes);
        if (visualInfo == NULL) {
            printf("GL: Failure to choose a visual\n");
            exit(31);
        }

        root = DefaultRootWindow(display);
        colormap = XCreateColormap(display, root, visualInfo->visual, AllocNone);

        setWindowAttributes.colormap = colormap;
        setWindowAttributes.event_mask = ExposureMask | KeyPressMask;

        window = XCreateWindow(display, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, visualInfo->depth,
             InputOutput, visualInfo->visual, CWColormap | CWEventMask, &setWindowAttributes);

        XMapWindow(display, window);
        XStoreName(display, window, "GBE");

        screen = DefaultScreen(display);

        glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
        glXMakeCurrent(display, window, glContext);
        glClearColor( 1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glXSwapBuffers(display, window);
    #endif
}

void loadTiles(cpu_state *cpu) {
    uint8 *vram = cpu->MEM + 0x8000;
    for (int tileNum = 0; tileNum < 384; tileNum++) {
        for (int y = 0; y < 8; y++) {
            uint8 byteLow = *(vram + 2*y + tileNum*16);
            uint8 byteHigh = *(vram + 2*y + 1 + tileNum*16);
            for (int x = 0; x < 8; x++) {
                tiles[tileNum][x][y] = ((byteLow & 0x80) >> 7) | ((byteHigh & 0x80) >> 6);
                byteLow <<= 1;
                byteHigh <<= 1;
            }
        }
    }
}

void loadScanline(cpu_state *cpu) {
    uint8 scanLine = cpu->MEM[SCANLINE];
    bool mapNumber = ((cpu->MEM[LCDC] >> 6) & 0b1);
    int mapLocation = (!mapNumber) ? 0x9C00 : 0x9800;
    // Draw tileset onto the window
    mapLocation += ((scanLine + cpu->MEM[SCROLL_Y]) >> 3) << 5;
    short lineOffset = cpu->MEM[SCROLL_X] >> 3;
    short x = cpu->MEM[SCROLL_X] & 7;
    short y = (scanLine + cpu->MEM[SCROLL_Y]) & 7;
    short tile = cpu->MEM[mapLocation + lineOffset];
    //printf("tile: %d\n", tile);
    if (!mapLocation) {
        printf("HMMM\n");
        tile = ((int8) tile) + 256;
    }
    short drawOffset = DISPLAY_WIDTH * scanLine;
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        frameBuffer[(drawOffset + i)*3 + 0] = COLOURS[tiles[tile][x][y]];
        frameBuffer[(drawOffset + i)*3 + 1] = COLOURS[tiles[tile][x][y]];
        frameBuffer[(drawOffset + i)*3 + 2] = COLOURS[tiles[tile][x][y]];
        x++;
        if (x == 8) {
            x = 0;
            lineOffset = (lineOffset + 1) & 31;
            tile = cpu->MEM[mapLocation + lineOffset];
            if (!mapLocation) {
                printf("HMMM\n");
                tile = ((int8) tile) + 256;
            }
        }
    }
}

void draw(cpu_state *cpu) {
    loadTiles(cpu);
    //XGetWindowAttributes(display, window, &windowAttributes);
    //glViewport(0, 0, windowAttributes.width, windowAttributes.height);
    glClear(GL_COLOR_BUFFER_BIT);
    //glEnable(GL_TEXTURE_2D);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    //TODO: switch to non-deprecated method of drawing
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
    //glColor3f(1, 1, 1);
    //glBegin(GL_QUADS);
    //glTexCoord2i(0, 1); glVertex2i(-1, -1);
    //glTexCoord2i(0, 0); glVertex2i(-1, 1);
    //glTexCoord2i(1, 0); glVertex2i(1, 1);
    //glTexCoord2i(1, 1); glVertex2i(1, -1);
    //glEnd();
    glRasterPos2f(-1, 1);
    glPixelZoom(2, -2);
    glDrawPixels(DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
    glXSwapBuffers(display, window);
}

void stopDisplay() {
    #ifdef OPENGL
        glXMakeCurrent(display, None, NULL);
        glXDestroyContext(display, glContext);
    #endif
    #ifdef X11
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    #endif
}
