; Memory Operations Test Suite
; Tests for memory load, store, and addressing operations

.test "basic load and store" {
    .description "Tests basic LOAD and STORE memory operations"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "basic"
    .tag "load"
    .tag "store"
    LOAD_IMM R0, 42
    STORE R0, 10
    LOAD_IMM R1, 0
    LOAD R1, 10
    .assert_reg R1, 42
    .assert_mem 10, 42
}

.test "multiple memory locations" {
    .description "Tests writing to multiple distinct memory addresses"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "store"
    .tag "multi-location"
    LOAD_IMM R0, 10
    STORE R0, 20
    LOAD_IMM R0, 20
    STORE R0, 21
    LOAD_IMM R0, 30
    STORE R0, 22
    .assert_mem 20, 10
    .assert_mem 21, 20
    .assert_mem 22, 30
}

.test "memory swap using registers" {
    .description "Tests swapping values between memory locations using register intermediates"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "load"
    .tag "store"
    .tag "swap"
    LOAD_IMM R0, 100
    STORE R0, 30
    LOAD_IMM R0, 200
    STORE R0, 31
    ; Load both values
    LOAD R1, 30
    LOAD R2, 31
    ; Store swapped
    STORE R2, 30
    STORE R1, 31
    .assert_mem 30, 200
    .assert_mem 31, 100
}

.test "LOADR indirect addressing" {
    .description "Test LOADR instruction for indirect addressing"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "indirect"
    .tag "loadr"
    ; Store value 42 at memory address 100
    LOAD_IMM R0, 42
    STORE R0, 100
    ; Store address 100 in register R2
    LOAD_IMM R2, 100
    ; Use LOADR to load from address stored in R2
    LOADR R3, R2
    ; Test: R3 should now contain 42
    .assert_reg R3, 42
}

.test "memory boundary access" {
    .description "Test memory access at boundaries"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "bounds"
    .tag "edge-cases"
    ; Test access at address 0
    LOAD_IMM R0, 123
    STORE R0, 0
    LOAD R1, 0
    .assert_reg R1, 123
    .assert_mem 0, 123
}

.test "memory initialization test" {
    .description "Test memory is properly initialized to zero"
    .author "DemiEngine Team"
    .category "Memory"
    .tag "initialization"
    .tag "zero"
    ; Load from an uninitialized location
    LOAD R0, 50
    .assert_reg R0, 0
}