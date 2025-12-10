; Memory Operations Test Suite
; Tests for memory load, store, and addressing operations

.test "basic load and store" {
    .description "Tests basic LOAD and STORE memory operations"
    .author "bobrossrtx"
    .category "Memory"
    .tag "basic"
    .tag "load"
    .tag "store"
    LOAD_IMM EAX, 42
    STORE EAX, 10
    LOAD_IMM EBX, 0
    LOAD EBX, 10
    .assert_reg EBX, 42
    .assert_mem 10, 42
}

.test "multiple memory locations" {
    .description "Tests writing to multiple distinct memory addresses"
    .author "bobrossrtx"
    .category "Memory"
    .tag "store"
    .tag "multi-location"
    LOAD_IMM EAX, 10
    STORE EAX, 20
    LOAD_IMM EAX, 20
    STORE EAX, 21
    LOAD_IMM EAX, 30
    STORE EAX, 22
    .assert_mem 20, 10
    .assert_mem 21, 20
    .assert_mem 22, 30
}

.test "memory swap using registers" {
    .description "Tests swapping values between memory locations using register intermediates"
    .author "bobrossrtx"
    .category "Memory"
    .tag "load"
    .tag "store"
    .tag "swap"
    LOAD_IMM EAX, 100
    STORE EAX, 30
    LOAD_IMM EAX, 200
    STORE EAX, 31
    ; Load both values
    LOAD EBX, 30
    LOAD ECX, 31
    ; Store swapped
    STORE ECX, 30
    STORE EBX, 31
    .assert_mem 30, 200
    .assert_mem 31, 100
}

.test "LOADR indirect addressing" {
    .description "Test LOADR instruction for indirect addressing"
    .author "bobrossrtx"
    .category "Memory"
    .tag "indirect"
    .tag "loadr"
    ; Store value 42 at memory address 100
    LOAD_IMM EAX, 42
    STORE EAX, 100
    ; Store address 100 in register ECX
    LOAD_IMM ECX, 100
    ; Use LOADR to load from address stored in ECX
    LOADR EDX, ECX
    ; Test: EDX should now contain 42
    .assert_reg EDX, 42
}

.test "memory boundary access" {
    .description "Test memory access at boundaries"
    .author "bobrossrtx"
    .category "Memory"
    .tag "bounds"
    .tag "edge-cases"
    ; Test access at address 0
    LOAD_IMM EAX, 123
    STORE EAX, 0
    LOAD EBX, 0
    .assert_reg EBX, 123
    .assert_mem 0, 123
}

.test "memory initialization test" {
    .description "Test memory is properly initialized to zero"
    .author "bobrossrtx"
    .category "Memory"
    .tag "initialization"
    .tag "zero"
    ; Load from an uninitialized location
    LOAD EAX, 50
    .assert_reg EAX, 0
}