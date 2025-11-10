; Logical Operations Test Suite
; Tests for bitwise and logical operations

.test "bitwise AND" {
    .description "Tests bitwise AND operation"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM R0, 0b11110000  ; 240
    LOAD_IMM R1, 0b10101010  ; 170
    AND R0, R1
    .assert_reg R0, 0b10100000  ; 160
}

.test "bitwise OR" {
    .description "Tests bitwise OR operation"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM R0, 0b11110000  ; 240
    LOAD_IMM R1, 0b00001111  ; 15
    OR R0, R1
    .assert_reg R0, 0b11111111  ; 255
}

.test "bitwise XOR" {
    .description "Tests bitwise XOR operation"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "basic"
    LOAD_IMM R0, 0b11110000  ; 240
    LOAD_IMM R1, 0b10101010  ; 170
    XOR R0, R1
    .assert_reg R0, 0b01011010  ; 90
}

.test "bitwise NOT" {
    .description "Tests bitwise NOT unary operation"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "unary"
    LOAD_IMM R0, 0b10101010  ; 170
    NOT R0
    .assert_reg R0, 0b01010101  ; 85
}

.test "shift left" {
    .description "Tests left shift operation (multiply by power of 2)"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "shift"
    .tag "basic"
    LOAD_IMM R0, 1
    SHL R0, 3
    .assert_reg R0, 8
}

.test "shift right" {
    .description "Tests right shift operation (divide by power of 2)"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "shift"
    .tag "basic"
    LOAD_IMM R0, 64
    SHR R0, 2
    .assert_reg R0, 16
}

.test "XOR swap trick" {
    .description "Tests XOR swap for swapping values without temp register"
    .author "DemiEngine Team"
    .category "Logical Operations"
    .tag "bitwise"
    .tag "xor-swap"
    LOAD_IMM R0, 42
    LOAD_IMM R1, 73
    ; XOR swap: a^=b, b^=a, a^=b
    XOR R0, R1
    XOR R1, R0  
    XOR R0, R1
    .assert_reg R0, 73
    .assert_reg R1, 42
}