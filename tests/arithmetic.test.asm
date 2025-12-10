; Arithmetic Operations Test Suite
; Tests for basic arithmetic operations

.test "addition" {
    .description "Tests basic addition of two numbers"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "basic"
    .tag "addition"
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 10
    ADD EAX, EBX
    .assert_reg EAX, 15
}

.test "subtraction" {
    .description "Tests basic subtraction of two numbers"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "basic"
    .tag "subtraction"
    LOAD_IMM EAX, 20
    LOAD_IMM EBX, 7
    SUB EAX, EBX
    .assert_reg EAX, 13
}

.test "multiplication" {
    .description "Tests basic multiplication of two numbers"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "basic"
    .tag "multiplication"
    LOAD_IMM EAX, 6
    LOAD_IMM EBX, 4
    MUL EAX, EBX
    .assert_reg EAX, 24
}

.test "division" {
    .description "Tests basic division of two numbers"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "basic"
    .tag "division"
    LOAD_IMM EAX, 20
    LOAD_IMM EBX, 4
    DIV EAX, EBX
    .assert_reg EAX, 5
}

.test "modulo operation" {
    .description "Tests modulo (remainder) operation"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "modulo"
    .tag "division"
    LOAD_IMM EAX, 17
    LOAD_IMM EBX, 5
    MOD EAX, EBX
    .assert_reg EAX, 2
}

.test "increment operation" {
    .description "Tests increment operation"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "increment"
    LOAD_IMM EAX, 42
    INC EAX
    .assert_reg EAX, 43
}

.test "decrement operation" {
    .description "Tests decrement operation"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "decrement"
    LOAD_IMM EAX, 42
    DEC EAX
    .assert_reg EAX, 41
}

.test "overflow handling" {
    .description "Tests arithmetic overflow behavior"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "overflow"
    .tag "boundary"
    LOAD_IMM EAX, 255
    LOAD_IMM EBX, 1
    ADD EAX, EBX
    ; Should wrap around to 0
    .assert_reg EAX, 0
}