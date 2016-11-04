#ifndef WINDOWS
    #include <time.h>
#endif
#include "main.h"
#include "cartridge.c"

typedef struct test_state {
    uint8 passed_tests;
    uint8 failled_tests;
} test_state;
