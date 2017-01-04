#ifndef WINDOWS
    #include <time.h>
#endif
#include "../types.h"
#include "../cartridge.c"
#include "../opcodes/opcodes.c"

typedef struct test_state {
    uint8 passed_tests;
    uint8 failled_tests;
} test_state;
