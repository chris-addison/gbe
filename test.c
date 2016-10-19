#include "test.h"

// Prints success or failed along with name of test
static void testing(char* name, bool success, test_state *state, cpu_state *cpu) {
    printf("TEST:\t%s\t[%s]\n", name, (success) ? "SUCCESS" : "FAIL");
    state->failled_tests += !success;
    state->passed_tests += success;
    // Reset cpu state after tests
    cpu = createCPU();
    // Reset flags to make tests simpler
    cpu->registers.F = 0x00;
}

// Assert that two uint8 as the same
static bool assertUint8(uint8 actual, uint8 expected) {
    bool result = (actual == expected);
    if (!result) {
        printf("Actual value: %d does not match expected value: %d\n", actual, expected);
    }
    return result;
}

// Assert that two uint8 as the same
static bool assertUint16(uint16 actual, uint16 expected) {
    bool result = (actual == expected);
    if (!result) {
        printf("Actual value: %d does not match expected value: %d\n", actual, expected);
    }
    return result;
}

// Assert flag is either set or reset
static bool assertFlag(uint8 flag, bool state, cpu_state *cpu) {
    bool result = ((cpu->registers.F >> flag) & 0b1) == state;
    if (!result) {
        char *name;
        switch (flag) {
            case 7: name = "Zero flag";         break;
            case 6: name = "Negative flag";     break;
            case 5: name = "Half-carry flag";   break;
            case 4: name = "Carry flag";        break;
        }
        printf("%s state: %s does not match expected state: %s\n", name, (state) ? "reset" : "set", (state) ? "set" : "reset");
    }
    return result;
}

static bool testLD_8(cpu_state *cpu) {
    // Test to pointer
    uint8 actual = rand() % 0x100;
    uint8 expected = rand() % 0x100;
    ld_8(expected, &actual, 0, cpu);
    bool result = assertUint8(actual, expected);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test to memory address
    cpu->MEM[0xFFFF] = rand() % 0x100;
    expected = rand() % 0x100;
    ld_8_m(expected, 0xFFFF, 0, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return assertUint8(cpu->MEM[0xFFFF], expected) && result;
}

static bool testLDI(cpu_state *cpu) {
    // Test to pointer
    uint8 actual = rand() % 0x100;
    uint8 expected = rand() % 0x100;
    uint16 expectedHL = cpu->registers.HL + 1;
    ldi(expected, &actual, 0, cpu);
    bool result = assertUint8(actual, expected);
    result &= assertUint16(cpu->registers.HL, expectedHL);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test to memory address
    cpu->MEM[0xFFFF] = rand() % 0x100;
    expected = rand() % 0x100;
    expectedHL = cpu->registers.HL + 1;
    ldi_m(expected, 0xFFFF, 0, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertUint16(cpu->registers.HL, expectedHL);
    return assertUint8(cpu->MEM[0xFFFF], expected) && result;
}

int main(int argc, char *argv[]) {
    // Create a new seed based off the clock
    srand(time(NULL));
    printf("\n[START TESTING]\n");
    struct cpu_state *cpu = createCPU();
    // Reset flags to make tests simpler
    cpu->registers.F = 0x00;
    struct test_state *state = (struct test_state *) malloc(sizeof(struct test_state));

    // Tests
    testing("LD 8", testLD_8(cpu), state, cpu);
    testing("LDI", testLDI(cpu), state, cpu);

    printf("\n[TESTING COMPLETE]\n%d tests passed out of %d total tests!\n\n", state->passed_tests, state->failled_tests + state->passed_tests);
}
