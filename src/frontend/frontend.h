#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdbool.h>

typedef struct windowSize {
    int width;
    int height;
} windowSize;

typedef struct frontend_input {
    bool a;
    bool b;
    bool start;
    bool select;
    bool up;
    bool down;
    bool left;
    bool right;
    bool unlock;
} frontend_input;

extern void frontend_swap_buffers();
extern void frontend_get_window_size(windowSize *size);

#endif /* FRONTEND */