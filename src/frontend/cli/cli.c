#include "../../gbe.h"
#include <stdbool.h>

int main(int argc, char *argv[]) {
    int out = startEmulator(argc, argv);
    // Run until error
    while (!out) {
        out = cycleEmulator();
    }
    stopEmulator();
}