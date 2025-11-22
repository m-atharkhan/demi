; 64-bit Multiplication Tests
; Tests for MUL64 instruction with various scenarios

.test "mul64 basic multiplication" {
    .description "Basic 64-bit multiplication test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "mul64"
    
    LOAD_IMM64 R0, 1000
    LOAD_IMM64 R1, 2000
    MUL64 R2, R0, R1    ; R2 = 1000 * 2000 = 2,000,000
    HALT
    
    .assert_reg R2, 2000000
}

.test "mul64 edge case small values" {
    .description "Test multiplication of small values"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "edge-case"
    
    LOAD_IMM R0, 1
    LOAD_IMM R1, 1
    MUL64 R2, R0, R1    ; R2 = 1 * 1 = 1
    HALT
    
    .assert_reg R2, 1
}

.test "mul64 zero multiplication" {
    .description "Test multiplication by zero"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "zero"
    
    LOAD_IMM64 R0, 12345
    LOAD_IMM R1, 0
    MUL64 R2, R0, R1    ; R2 = 12345 * 0 = 0
    HALT
    
    .assert_reg R2, 0
}

.test "mul64 large numbers" {
    .description "Test multiplication of large numbers (within 64-bit range)"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "large-values"
    
    LOAD_IMM64 R0, 1000000
    LOAD_IMM64 R1, 1000
    MUL64 R2, R0, R1    ; R2 = 1,000,000 * 1,000 = 1,000,000,000
    HALT
    
    .assert_reg R2, 1000000000
}

.test "mul64 signed numbers" {
    .description "Test multiplication with signed numbers (positive result)"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "signed"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 50
    MUL64 R2, R0, R1    ; R2 = 100 * 50 = 5000
    HALT
    
    .assert_reg R2, 5000
}