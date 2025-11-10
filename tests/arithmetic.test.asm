; Arithmetic Operations Test Suite
; Tests for basic arithmetic operations

.test "addition" {
    .description "Tests basic addition of two numbers"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "basic"
    .tag "addition"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    .assert_reg R0, 15
}

.test "subtraction" {
    .description "Tests basic subtraction of two numbers"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "basic"
    .tag "subtraction"
    LOAD_IMM R0, 20
    LOAD_IMM R1, 7
    SUB R0, R1
    .assert_reg R0, 13
}

.test "multiplication" {
    .description "Tests basic multiplication of two numbers"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "basic"
    .tag "multiplication"
    LOAD_IMM R0, 6
    LOAD_IMM R1, 4
    MUL R0, R1
    .assert_reg R0, 24
}

.test "division" {
    .description "Tests basic division of two numbers"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "basic"
    .tag "division"
    LOAD_IMM R0, 20
    LOAD_IMM R1, 4
    DIV R0, R1
    .assert_reg R0, 5
}

.test "modulo operation" {
    .description "Tests modulo (remainder) operation"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "modulo"
    .tag "division"
    LOAD_IMM R0, 17
    LOAD_IMM R1, 5
    MOD R0, R1
    .assert_reg R0, 2
}

.test "increment operation" {
    .description "Tests increment operation"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "increment"
    LOAD_IMM R0, 42
    INC R0
    .assert_reg R0, 43
}

.test "decrement operation" {
    .description "Tests decrement operation"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "decrement"
    LOAD_IMM R0, 42
    DEC R0
    .assert_reg R0, 41
}

.test "overflow handling" {
    .description "Tests arithmetic overflow behavior"
    .author "DemiEngine Team"
    .category "Arithmetic"
    .tag "overflow"
    .tag "boundary"
    LOAD_IMM R0, 255
    LOAD_IMM R1, 1
    ADD R0, R1
    ; Should wrap around to 0
    .assert_reg R0, 0
}