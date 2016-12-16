#include <GL/glu.h>
#include <GL/gl.h>

#include "../types.h"
#include "../display.h"
#include "../frontend/frontend.h"

// Update viewport size
void gl_update_viewport() {
    windowSize window = {};
    frontend_get_window_size(&window);
    glViewport(0, 0, window.width, window.height);
}

// Draw the framebuffer to the window
void gl_display_framebuffer_on_window(uint8 *frameBuffer) {
    gl_update_viewport();
    //XGetWindowAttributes(display, window, &windowAttributes);
    //glViewport(0, 0, windowAttributes.width, windowAttributes.height);
    glClear(GL_COLOR_BUFFER_BIT);
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
    glEnd();
    /*glRasterPos2f(-1, 1);
    glPixelZoom(2, -2);
    glDrawPixels(DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);*/
    frontend_swap_buffers();
}

// Clear the window 
void gl_clear_window() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    frontend_swap_buffers();
}