#ifndef INPUT_H
#define INPUT_H

typedef struct input {
    bool A;
    bool B;
    bool start;
    bool select;
    bool up;
    bool down;
    bool left;
    bool right;
} input;

extern input getInput();

#endif /* INPUT_H */