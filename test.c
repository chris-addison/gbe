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

// Test ld_8, ld_8_m methods
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

// Test ldi, ldi_m methods
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

// Test ldd, ldd_m method
static bool testLDD(cpu_state *cpu) {
    // Test to pointer
    uint8 actual = rand() % 0x100;
    uint8 expected = rand() % 0x100;
    uint16 expectedHL = cpu->registers.HL - 1;
    ldd(expected, &actual, 0, cpu);
    bool result = assertUint8(actual, expected);
    result &= assertUint16(cpu->registers.HL, expectedHL);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test to memory address
    cpu->MEM[0xFFFF] = rand() % 0x100;
    expected = rand() % 0x100;
    expectedHL = cpu->registers.HL - 1;
    ldd_m(expected, 0xFFFF, 0, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertUint16(cpu->registers.HL, expectedHL);
    return assertUint8(cpu->MEM[0xFFFF], expected) && result;
}

// Test ld_16, ld_16_m methods
static bool testLD_16(cpu_state *cpu) {
    // Test to pointer
    uint16 actual = rand() % 0x10000;
    uint16 expected = rand() % 0x10000;
    ld_16(expected, &actual, 0, cpu);
    bool result = assertUint16(actual, expected);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test to memory address
    cpu->MEM[0xFFF0] = rand() % 0x100;
    cpu->MEM[0xFFF1] = rand() % 0x100;
    expected = rand() % 0x10000;
    ld_16_m(expected, 0xFFF0, 0, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return assertUint16((cpu->MEM[0xFFF1] << 8) + cpu->MEM[0xFFF0], expected) && result;
}

// Test pop, push methods
static bool testPUSH_POP(cpu_state *cpu) {
    uint16 expected = rand() % 0x10000;
    push(expected, 0, cpu);
    // Check memory
    bool result = assertUint16((cpu->MEM[cpu->SP + 1] << 8) + cpu->MEM[cpu->SP], expected);
    uint16 returnedValue = 0;
    pop(&returnedValue, 0, cpu);
    // Check result of pop
    result &= assertUint16(returnedValue, expected);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test add_8 method
static bool testADD(cpu_state *cpu) {
    // Simple add
    // Reset A for simple start value
    cpu->registers.A = 0x00;
    uint8 testValue = 0x01;
    add_8(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x01);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // General flag test
    cpu->registers.A = 0xFF;
    testValue = 0x01;
    add_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, true, cpu);
    // Carry flag test
    cpu->registers.A = 0xF0;
    testValue = 0x20;
    add_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Half-carry flag test
    cpu->registers.A = 0x0F;
    testValue = 0x01;
    add_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

int main(int argc, char *argv[]) {
    // Create a new seed based off the clock
    srand(time(NULL));
    printf("\n[START TESTING]\n");
    struct cpu_state *cpu = createCPU();
    // Reset flags to make tests simpler
    cpu->registers.F = 0x00;
    struct test_state *state = (struct test_state *) malloc(sizeof(struct test_state));

    /*    Tests    */

    // 8 bit loads
    testing("LD 8", testLD_8(cpu), state, cpu);
    testing("LDI", testLDI(cpu), state, cpu);
    testing("LDD", testLDD(cpu), state, cpu);

    // 16 bit loads
    testing("LD 16", testLD_16(cpu), state, cpu);
    testing("STK OP", testPUSH_POP(cpu), state, cpu);

    // 8 bit arithemtic
    testing("ADD 8", testADD(cpu), state, cpu);


    printf("\n[TESTING COMPLETE]\n%d tests passed out of %d total tests!\n\n", state->passed_tests, state->failled_tests + state->passed_tests);
}
