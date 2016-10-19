#include "test.h"

// Prints success or failed along with name of test
static void testing(char* name, bool success, test_state *state) {
    printf("TEST:\t%s\t[%s]\n", name, (success) ? "SUCCESS" : "FAIL");
    state->failled_tests += !success;
    state->passed_tests += success;
}

static bool testLD_8(cpu_state *cpu) {

    return false;
}

int main(int argc, char *argv[]) {
    printf("\n[START TESTING]\n");
    struct cpu_state *cpu = createCPU();
    struct test_state *state = (struct test_state *) malloc(sizeof(struct cpu_state));
    testing("LD 8", testLD_8(cpu), state);
    printf("\n[TESTING COMPLETE]\n%d tests passed out of %d total tests!\n\n", state->passed_tests, state->failled_tests + state->passed_tests);
}
