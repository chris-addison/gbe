#ifndef FRONTEND_H
#define FRONTEND_H

typedef struct windowSize {
    int width;
    int height;
} windowSize;

extern void frontend_swap_buffers();
extern void frontend_get_window_size(windowSize *size);

#endif /* FRONTEND */