; Logical Operations Test Suite
; Tests for bitwise and logical operations

.test "bitwise AND" {
    .description "Tests bitwise AND operation"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM EAX, 0b11110000  ; 240
    LOAD_IMM EBX, 0b10101010  ; 170
    AND EAX, EBX
    .assert_reg EAX, 0b10100000  ; 160
}

.test "bitwise OR" {
    .description "Tests bitwise OR operation"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM EAX, 0b11110000  ; 240
    LOAD_IMM EBX, 0b00001111  ; 15
    OR EAX, EBX
    .assert_reg EAX, 0b11111111  ; 255
}

.test "bitwise XOR" {
    .description "Tests bitwise XOR operation"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM EAX, 0b11110000  ; 240
    LOAD_IMM EBX, 0b10101010  ; 170
    XOR EAX, EBX
    .assert_reg EAX, 0b01011010  ; 90
}

.test "bitwise NOT" {
    .description "Tests bitwise NOT unary operation"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "unary"
    LOAD_IMM EAX, 0b10101010  ; 170
    NOT EAX
    .assert_reg EAX, 0b01010101  ; 85
}

.test "shift left" {
    .description "Tests left shift operation (multiply by power of 2)"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "shift"
    .tag "basic"
    LOAD_IMM EAX, 1
    SHL EAX, 3
    .assert_reg EAX, 8
}

.test "shift right" {
    .description "Tests right shift operation (divide by power of 2)"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "shift"
    .tag "basic"
    LOAD_IMM EAX, 64
    SHR EAX, 2
    .assert_reg EAX, 16
}

.test "XOR swap trick" {
    .description "Tests XOR swap for swapping values without temp register"
    .author "bobrossrtx"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "xor-swap"
    LOAD_IMM EAX, 42
    LOAD_IMM EBX, 73
    ; XOR swap: a^=b, b^=a, a^=b
    XOR EAX, EBX
    XOR EBX, EAX  
    XOR EAX, EBX
    .assert_reg EAX, 73
    .assert_reg EBX, 42
}