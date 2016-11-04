#include "main.h"

#ifdef X11
    Display *display;
    Window window;
    XEvent event;
#endif

void startDisplay() {
    #ifdef X11
        display = XOpenDisplay(NULL);
        if (display == NULL) {
            printf("Error connecting to X11\n");
            exit(11);
        } else {
            printf("X11 is good to go!\n");
        }
        int screen = DefaultScreen(display);
        window = XCreateSimpleWindow(display,
            RootWindow(display, screen), 1000, 1000, 160, 144, 1,
            BlackPixel(display, screen), WhitePixel(display, screen));
        XSelectInput(display, window, ExposureMask | KeyPressMask);
        XMapWindow(display, window);
        XStoreName(display, window, "GBE");
    #endif
}

void stopDisplay() {
    #ifdef X11
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    #endif
}
