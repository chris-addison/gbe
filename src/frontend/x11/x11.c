#include "../../gbe.h"
#include <stdlib.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "../../window.h"
#include "../../display.h"
#include "../../input.h"

#define WINDOW_HEIGHT 288
#define WINDOW_WIDTH 320

Display *display;
Window window;
int screen;
XEvent event;
XWindowAttributes windowAttributes = {0};
XSetWindowAttributes setWindowAttributes;
Window root;
GLint attributes[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
XVisualInfo *visualInfo;
Colormap colormap;
GLXContext glContext;
GLuint textureID;
input current_input;

// Get current input
input getInput() {    
    if (XPending(display)) {
        XNextEvent(display, &event);
        if (event.type == KeyPress || event.type == KeyRelease) {
            printf("Key code is: %d\n", event.xkey.keycode);
            switch(event.xkey.keycode) {
                case 52: // z key
                    current_input.A = (event.type == KeyPress);
                    printf("A\n");                    
                    break;
                case 53: // x key
                    current_input.B = (event.type == KeyPress);
                    printf("B\n"); 
                    break;
                case 36: // Enter key
                    current_input.start = (event.type == KeyPress);
                    printf("Start\n"); 
                    break; 
                case 134: // Shift key
                    current_input.select = (event.type == KeyPress);
                    printf("Select\n"); 
                    break;
                case 111: // Up arrow
                    current_input.up = (event.type == KeyPress);
                    printf("Up\n"); 
                    break;
                case 116: // Down arrow
                    current_input.down = (event.type == KeyPress);
                    printf("Down\n"); 
                    break;
                case 113: // Left arrow
                    current_input.left = (event.type == KeyPress);
                    printf("Left\n"); 
                    break;
                case 114: // Right arrow
                    current_input.right = (event.type == KeyPress);
                    printf("Right\n"); 
                    break; 
            }
        }
    }
    return current_input;
}

// Display frameBuffer on screen
void displayOnWindow(uint8 *frameBuffer) {
    //XGetWindowAttributes(display, window, &windowAttributes);
    //glViewport(0, 0, windowAttributes.width, windowAttributes.height);
    /*glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    //TODO: switch to non-deprecated method of drawing
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 1); glVertex2i(-1, -1);
    glTexCoord2i(0, 0); glVertex2i(-1, 1);
    glTexCoord2i(1, 0); glVertex2i(1, 1);
    glTexCoord2i(1, 1); glVertex2i(1, -1);
    glEnd();*/
    glRasterPos2f(-1, 1);
    glPixelZoom(2, -2);
    glDrawPixels(DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
    glXSwapBuffers(display, window);
}

// CLose display.
void stopDisplay() {
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glContext);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

// Start display
void startDisplay() {
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
}

// Run emulator from this method.
int main(int argc, char *argv[]) {
    startDisplay();
    startEmulator(argc, argv);
    stopDisplay();
}
