#ifndef INPUT_H
#define INPUT_H

typedef struct input {
    bool a;
    bool b;
    bool start;
    bool select;
    bool up;
    bool down;
    bool left;
    bool right;
} input;

extern void getInput(input *current_input);

#endif /* INPUT_H */