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

        window = XCreateWindow(display, root, 0, 0, 160, 144, 0, visualInfo->depth,
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

void drawScanline(cpu_state *cpu) {
    uint8 *vram = cpu->MEM + 0x8000;
    // Print out tiles
    uint8 tiles[256][8][8];
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
    // Draw tileset onto the window
    uint8 data[3 * 160 * 144];
    int x = 0;
    int y = 0;
    int scanLine = 0;
    int tileNum = 0;
    for (int i = 0; i < 160*144; i++) {
        x = i % 8;
        scanLine = i / 160;
        y = scanLine % 8;
        tileNum = ((scanLine / 8) * 20 + ((i / 8) % 20)) % 384;
        if (tiles[tileNum][x][y]) {
            data[i*3 + 0] = 0;
            data[i*3 + 1] = 0;
            data[i*3 + 2] = 0;
        } else {
            data[i*3 + 0] = 0xFF;
            data[i*3 + 1] = 0xFF;
            data[i*3 + 2] = 0xFF;
        }
    }
    XGetWindowAttributes(display, window, &windowAttributes);
    glViewport(0, 0, windowAttributes.width, windowAttributes.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, data);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 1); glVertex2i(-1, -1);
    glTexCoord2i(0, 0); glVertex2i(-1, 1);
    glTexCoord2i(1, 0); glVertex2i(1, 1);
    glTexCoord2i(1, 1); glVertex2i(1, -1);
    glEnd();
    //glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, data);
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
