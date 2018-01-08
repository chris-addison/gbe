#include "../../gbe.h"
#include "../frontend.h"
#include "../../input.h"
#include "../../window.h"
#include <stdbool.h>

int main(int argc, char *argv[]) {
    int out = startEmulator(argc, argv);
    // Run until error
    while (!out) {
        out = cycleEmulator();
    }
    stopEmulator();
}

void getInput(input *current_input) {
    // Do nothing
}

void startDisplay() {
    // Do nothing
}

void displayOnWindow(uint8 *frameBuffer) {
    // Do nothing
}

void stopDisplay() {
    // Do nothing
}