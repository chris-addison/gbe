#include "test.h"

// Performs basic reset on the cpu
static void resetCPU(cpu_state *cpu) {
    cpu->registers.AF = 0x00;
    cpu->registers.BC = 0x00;
    cpu->registers.DE = 0x00;
    cpu->registers.HL = 0x00;
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;
    cpu->wait = 0;
    cpu->ROM_bank = 1;
    cpu->RAM_bank = 0;
    cpu->RAM_enable = false;
}

// Prints success or failed along with name of test
static void testing(char* name, bool success, test_state *state, cpu_state *cpu) {
    printf("TEST:\t%s\t[%s]\n", name, (success) ? "SUCCESS" : "FAIL");
    state->failled_tests += !success;
    state->passed_tests += success;
    // Reset cpu after test
    resetCPU(cpu);
}

// Assert that two uint8 as the same
static bool assertUint8(uint8 actual, uint8 expected) {
    bool result = (actual == expected);
    if (!result) {
        printf("Actual value: 0x%X does not match expected value: 0x%X\n", actual, expected);
    }
    return result;
}

// Assert that two uint8 as the same
static bool assertUint16(uint16 actual, uint16 expected) {
    bool result = (actual == expected);
    if (!result) {
        printf("Actual value: 0x%X does not match expected value: 0x%X\n", actual, expected);
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

// Test setFlag, clearFlag, readFlag methods
static bool testFlags(cpu_state *cpu) {
    setFlag(CF, cpu);
    bool result = assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, readFlag(CF, cpu), cpu);
    result &= assertFlag(ZF, readFlag(ZF, cpu), cpu);
    result &= assertFlag(NF, readFlag(NF, cpu), cpu);
    result &= assertFlag(HF, readFlag(HF, cpu), cpu);
    setFlag(ZF, cpu);
    setFlag(HF, cpu);
    setFlag(NF, cpu);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, readFlag(CF, cpu), cpu);
    result &= assertFlag(ZF, readFlag(ZF, cpu), cpu);
    result &= assertFlag(NF, readFlag(NF, cpu), cpu);
    result &= assertFlag(HF, readFlag(HF, cpu), cpu);
    clearFlag(CF, cpu);
    clearFlag(ZF, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, readFlag(CF, cpu), cpu);
    result &= assertFlag(ZF, readFlag(ZF, cpu), cpu);
    result &= assertFlag(NF, readFlag(NF, cpu), cpu);
    result &= assertFlag(HF, readFlag(HF, cpu), cpu);
    clearFlag(NF, cpu);
    clearFlag(HF, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, readFlag(CF, cpu), cpu);
    result &= assertFlag(ZF, readFlag(ZF, cpu), cpu);
    result &= assertFlag(NF, readFlag(NF, cpu), cpu);
    result &= assertFlag(HF, readFlag(HF, cpu), cpu);
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

//TODO: test LD HL, (SP+e)

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
static bool testADD_8(cpu_state *cpu) {
    // Simple add
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

// Test adc method
static bool testADC(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    setFlag(CF, cpu); // Will need to test this method too!
    uint8 testValue = 0xFF;
    adc(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test sub_8 method
static bool testSUB_8(cpu_state *cpu) {
    // Simple add
    cpu->registers.A = 0x01;
    uint8 testValue = 0x01;
    sub_8(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // General flag test
    cpu->registers.A = 0x10;
    testValue = 0x11;
    sub_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0xFF);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, true, cpu);
    // Carry flag test
    cpu->registers.A = 0x10;
    testValue = 0x20;
    sub_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0xF0);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Half-carry flag test
    cpu->registers.A = 0x10;
    testValue = 0x01;
    sub_8(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x0F);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test sbc method
static bool testSBC(cpu_state *cpu) {
    cpu->registers.A = 0x10;
    setFlag(CF, cpu); // Will need to test this method too!
    uint8 testValue = 0x0F;
    sbc(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test and method
static bool testAND(cpu_state *cpu) {
    // Test simple
    cpu->registers.A = 0xFF;
    uint8 testValue = 0xFF;
    and(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0xFF);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0x00;
    testValue = 0xFF;
    and(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test general
    cpu->registers.A = 0x10;
    testValue = 0xF0;
    and(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test or method
static bool testOR(cpu_state *cpu) {
    // Test simple
    cpu->registers.A = 0xFF;
    uint8 testValue = 0xFF;
    or(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0xFF);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0x00;
    testValue = 0x00;
    or(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test general
    cpu->registers.A = 0x10;
    testValue = 0x0F;
    or(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x1F);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test xor method
static bool testXOR(cpu_state *cpu) {
    // Test simple
    cpu->registers.A = 0x0F;
    uint8 testValue = 0xF0;
    xor(testValue, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0xFF);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0xFF;
    testValue = 0xFF;
    xor(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test general
    cpu->registers.A = 0x11;
    testValue = 0x3F;
    xor(testValue, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x2E);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test cp method
static bool testCP(cpu_state *cpu) {
    // Test no flag
    cpu->registers.A = 0x02;
    uint8 testValue = 0x01;
    cp(testValue, 0, cpu);
    bool result = true;
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0xF0;
    testValue = 0xF0;
    cp(testValue, 0, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test half-carry && carry flag
    cpu->registers.A = 0x11;
    testValue = 0x12;
    cp(testValue, 0, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, true, cpu);
    return result;
}

// Test inc_8 method
static bool testINC_8(cpu_state *cpu) {
    // Test simple
    cpu->registers.A = 0x11;
    inc_8(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x12);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0xFF;
    inc_8(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test half-carry flag
    cpu->registers.A = 0x0F;
    inc_8(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test dec_8 method
static bool testDEC_8(cpu_state *cpu) {
    // Test simple
    cpu->registers.A = 0x11;
    dec_8(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0x01;
    dec_8(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test half-carry flag
    cpu->registers.A = 0x10;
    dec_8(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x0F);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test add_16 method
static bool testADD_16(cpu_state *cpu) {
    // Simple add
    cpu->registers.BC = 0x0000;
    uint16 testValue = 0x0001;
    add_16(testValue, &cpu->registers.BC, 0, cpu);
    bool result = assertUint16(cpu->registers.BC, 0x0001);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // General flag test
    cpu->registers.BC = 0xFFFF;
    testValue = 0x0001;
    add_16(testValue, &cpu->registers.BC, 0, cpu);
    result &= assertUint16(cpu->registers.BC, 0x0000);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, true, cpu);
    // Carry flag test
    cpu->registers.BC = 0xF000;
    testValue = 0x2000;
    add_16(testValue, &cpu->registers.BC, 0, cpu);
    result &= assertUint16(cpu->registers.BC, 0x1000);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Half-carry flag test
    cpu->registers.BC = 0x0FFF;
    testValue = 0x0001;
    add_16(testValue, &cpu->registers.BC, 0, cpu);
    result &= assertUint16(cpu->registers.BC, 0x1000);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test inc_16 method
static bool testINC_16(cpu_state *cpu) {
    // Test simple
    cpu->registers.BC = 0x1111;
    inc_16(&cpu->registers.BC, 0, cpu);
    bool result = assertUint16(cpu->registers.BC, 0x1112);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test wrap
    cpu->registers.BC = 0xFFFF;
    inc_16(&cpu->registers.BC, 0, cpu);
    result &= assertUint16(cpu->registers.BC, 0x0000);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test dec_16 method
static bool testDEC_16(cpu_state *cpu) {
    // Test simple
    cpu->registers.BC = 0x1111;
    dec_16(&cpu->registers.BC, 0, cpu);
    bool result = assertUint16(cpu->registers.BC, 0x1110);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test wrap
    cpu->registers.BC = 0x0000;
    dec_16(&cpu->registers.BC, 0, cpu);
    result &= assertUint16(cpu->registers.BC, 0xFFFF);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test swap method
static bool testSWAP(cpu_state *cpu) {
    cpu->registers.A = 0x12;
    swap(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x21);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test zero flag
    cpu->registers.A = 0x00;
    swap(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test daa method. There are a lot of paths to test, so this is a little large.
static bool testDAA(cpu_state * cpu) {
    // Test simple lower nibble
    cpu->registers.A = 0x0A;
    daa(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x10);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test simple upper nibble
    cpu->registers.A = 0xA0;
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Test with carry flag
    cpu->registers.A = 0x01;
    clearFlag(ZF, cpu);
    setFlag(CF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x61);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Test with carry flag, half-carry flag
    cpu->registers.A = 0x01;
    clearFlag(ZF, cpu);
    setFlag(CF, cpu);
    setFlag(HF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x67);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Test with carry flag and a lower nibble that needs to be corrected
    cpu->registers.A = 0x0F;
    setFlag(CF, cpu);
    clearFlag(ZF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x75);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Test with negative flag and half-carry flag
    cpu->registers.A = 0x0F;
    clearFlag(CF, cpu);
    clearFlag(ZF, cpu);
    setFlag(NF, cpu);
    setFlag(HF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x09);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, false, cpu);
    // Test with negative flag, carry flag
    cpu->registers.A = 0x61;
    clearFlag(ZF, cpu);
    setFlag(NF, cpu);
    setFlag(CF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x01);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    // Test with negative flag, carry flag, and half-carry flag
    cpu->registers.A = 0x66;
    clearFlag(ZF, cpu);
    setFlag(NF, cpu);
    setFlag(HF, cpu);
    setFlag(CF, cpu);
    daa(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, false, cpu);
    result &= assertFlag(CF, true, cpu);
    return result;
}

// Test cpl method
static bool testCPL(cpu_state *cpu) {
    cpu->registers.A = 0xF0;
    cpl(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x0F);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, true, cpu);
    result &= assertFlag(HF, true, cpu);
    result &= assertFlag(CF, false, cpu);
    return result;
}

// Test ccf method
static bool testCCF(cpu_state *cpu) {
    ccf(0, cpu);
    bool result = assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    ccf(0, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test scf method
static bool testSCF(cpu_state *cpu) {
    scf(0, cpu);
    bool result = assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    // Check to make sure the flag is only set by method
    scf(0, cpu);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

//TODO: Test halt

//TODO: Test stop

//TODO: Test DI, EI - Will need a stronger testing harness for these along with halt and stop

// Test rlca method
static bool testRLCA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    setFlag(CF, cpu);
    rlca(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x81;
    rlca(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x03);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rla method
static bool testRLA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    rla(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x81;
    rla(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x02);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x21;
    setFlag(CF, cpu);
    rla(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x43);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rrca method
static bool testRRCA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    setFlag(CF, cpu);
    rrca(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x21;
    rrca(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x90);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rra method
static bool testRRA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    rra(0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x11;
    rra(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x08);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x22;
    setFlag(CF, cpu);
    rra(0, cpu);
    result &= assertUint8(cpu->registers.A, 0x91);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rlc method
static bool testRLC(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    setFlag(CF, cpu);
    rlc(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x81;
    rlc(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x03);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rl method
static bool testRL(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    rl(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x81;
    rl(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x02);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x21;
    setFlag(CF, cpu);
    rl(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x43);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rrc, rrc_m methods
static bool testRRC(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    setFlag(CF, cpu);
    rrc(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x21;
    rrc(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x90);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->MEM[cpu->registers.HL] = 0x00;
    setFlag(CF, cpu);
    rrc_m(0, cpu);
    result &= assertUint8(cpu->MEM[cpu->registers.HL], 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->MEM[cpu->registers.HL] = 0x21;
    rrc_m(0, cpu);
    result &= assertUint8(cpu->MEM[cpu->registers.HL], 0x90);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rr method
static bool testRR(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    rr(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x11;
    rr(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x08);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0x22;
    setFlag(CF, cpu);
    rr(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x91);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test sla method
static bool testSLA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    sla(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0xF0;
    sla(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0xE0);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test sra method
static bool testSRA(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    sra(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0xF1;
    sra(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0xF8);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test srl method
static bool testSRL(cpu_state *cpu) {
    cpu->registers.A = 0x00;
    srl(&cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x00);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->registers.A = 0xF1;
    srl(&cpu->registers.A, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x78);
    result &= assertFlag(CF, true, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test bit, bit_m methods
static bool testBIT(cpu_state *cpu) {
    cpu->registers.A = 0x0C;
    bit(3, &cpu->registers.A, 0, cpu);
    bool result = assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    bit(0, &cpu->registers.A, 0, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    cpu->MEM[cpu->registers.HL] = 0x0C;
    bit_m(3, 0, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    bit_m(0, 0, cpu);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, true, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, true, cpu);
    return result;
}

// Test set, set_m method
static bool testSET(cpu_state *cpu) {
    cpu->registers.A = 0x11;
    set(1, &cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x13);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->MEM[cpu->registers.HL] = 0x11;
    set_m(1, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x13);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test res, res_m method
static bool testRES(cpu_state *cpu) {
    cpu->registers.A = 0x13;
    res(1, &cpu->registers.A, 0, cpu);
    bool result = assertUint8(cpu->registers.A, 0x11);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    cpu->MEM[cpu->registers.HL] = 0x13;
    res_m(1, 0, cpu);
    result &= assertUint8(cpu->registers.A, 0x11);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test jp_c method
static bool testJP(cpu_state *cpu) {
    jp_c(false, 0x1234, 0, cpu);
    bool result = assertUint16(cpu->PC, 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    jp_c(true, 0x5678, 0, cpu);
    result &= assertUint16(cpu->PC, 0x5678);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test jr_c method
static bool testJR(cpu_state *cpu) {
    jr_c(false, 1, 0, cpu);
    bool result = assertUint16(cpu->PC, 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    jr_c(true, -1, 0, cpu);
    result &= assertUint16(cpu->PC, 0x00FF);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    jr_c(true, 1, 0, cpu);
    result &= assertUint16(cpu->PC, 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test call_c method
static bool testCALL(cpu_state *cpu) {
    call_c(false, 0x1234, 0, cpu);
    bool result = assertUint16(cpu->PC, 0x0100);
    result &= assertUint16(cpu->SP, 0xFFFE);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    call_c(true, 0x5678, 0, cpu);
    result &= assertUint16(cpu->PC, 0x5678);
    result &= assertUint16(cpu->SP, 0xFFFC);
    result &= assertUint16((cpu->MEM[cpu->SP+1] << 8) + cpu->MEM[cpu->SP], 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

// Test rst method
static bool testRST(cpu_state *cpu) {
    rst(0x00, 0, cpu);
    bool result = assertUint16(cpu->PC, 0x0000);
    result &= assertUint16(cpu->SP, 0xFFFC);
    result &= assertUint16((cpu->MEM[cpu->SP+1] << 8) + cpu->MEM[cpu->SP], 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

//TODO: test reti

// Test ret_c method
static bool testRET(cpu_state *cpu) {
    call_c(true, 0x2020, 0, cpu);
    ret_c(false, 0, cpu);
    bool result = assertUint16(cpu->PC, 0x2020);
    result &= assertUint16(cpu->SP, 0xFFFC);
    result &= assertUint16((cpu->MEM[cpu->SP+1] << 8) + cpu->MEM[cpu->SP], 0x0100);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    ret_c(true, 0, cpu);
    result &= assertUint16(cpu->PC, 0x0100);
    result &= assertUint16(cpu->SP, 0xFFFE);
    result &= assertFlag(CF, false, cpu);
    result &= assertFlag(ZF, false, cpu);
    result &= assertFlag(NF, false, cpu);
    result &= assertFlag(HF, false, cpu);
    return result;
}

int main(int argc, char *argv[]) {
    #ifndef WINDOWS
        // Create a new seed based off the clock
        srand(time(NULL));
    #endif
    printf("\n[START TESTING]\n");
    struct cpu_state *cpu = createCPU();
    // Reset cpu before starting tests
    resetCPU(cpu);
    // Setup struct to hold test information
    struct test_state *state = (struct test_state *) malloc(sizeof(struct test_state));
    state->failled_tests = 0;
    state->passed_tests = 0;

    /*    Tests    */

    // Utility methods
    testing("FLAGS", testFlags(cpu), state, cpu);

    // 8 bit loads
    testing("LD 8", testLD_8(cpu), state, cpu);
    testing("LDI", testLDI(cpu), state, cpu);
    testing("LDD", testLDD(cpu), state, cpu);

    // 16 bit loads
    testing("LD 16", testLD_16(cpu), state, cpu);
    testing("STK OP", testPUSH_POP(cpu), state, cpu);

    // 8 bit arithmetic
    testing("ADD 8", testADD_8(cpu), state, cpu);
    testing("ADC", testADC(cpu), state, cpu);
    testing("SUB 8", testSUB_8(cpu), state, cpu);
    testing("SBC", testSBC(cpu), state, cpu);
    testing("AND", testAND(cpu), state, cpu);
    testing("OR", testOR(cpu), state, cpu);
    testing("XOR", testXOR(cpu), state, cpu);
    testing("CP", testCP(cpu), state, cpu);
    testing("INC 8", testINC_8(cpu), state, cpu);
    testing("DEC 8", testDEC_8(cpu), state, cpu);

    // 16 bit arithmetic
    testing("ADD 16", testADD_16(cpu), state, cpu);
    testing("INC 16", testINC_16(cpu), state, cpu);
    testing("DEC 16", testDEC_16(cpu), state, cpu);

    // Misc
    testing("SWAP", testSWAP(cpu), state, cpu);
    testing("DAA", testDAA(cpu), state, cpu);
    testing("CPL", testCPL(cpu), state, cpu);
    testing("CCF", testCCF(cpu), state, cpu);
    testing("SCF", testSCF(cpu), state, cpu);

    // Shifts and Rotates
    testing("RLCA", testRLCA(cpu), state, cpu);
    testing("RLA", testRLA(cpu), state, cpu);
    testing("RRCA", testRRCA(cpu), state, cpu);
    testing("RRA", testRRA(cpu), state, cpu);
    testing("RLC", testRLC(cpu), state, cpu);
    testing("RL", testRL(cpu), state, cpu);
    testing("RRC", testRRC(cpu), state, cpu);
    testing("RR", testRRA(cpu), state, cpu);
    testing("SLA", testSLA(cpu), state, cpu);
    testing("SRA", testSRA(cpu), state, cpu);
    testing("SRL", testSRL(cpu), state, cpu);

    // Bit tests, sets, and resets
    testing("BIT", testBIT(cpu), state, cpu);
    testing("SET", testSET(cpu), state, cpu);
    testing("RES", testRES(cpu), state, cpu);

    // Jumps
    testing("JP", testJP(cpu), state, cpu);
    testing("JR", testJR(cpu), state, cpu);

    // Calls
    testing("CALL", testCALL(cpu), state, cpu);

    // Restarts
    testing("RST", testRST(cpu), state, cpu);

    // Returns
    testing("RET", testRET(cpu), state, cpu);

    // Print result
    printf("\n[TESTING COMPLETE]\n%d tests passed out of %d total tests!\n\n", state->passed_tests, state->failled_tests + state->passed_tests);

    // Free and end program
    free(cpu);
    free(state);
    exit(0);
}
