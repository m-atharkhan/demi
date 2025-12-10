; 64-bit Multiplication Tests
; Tests for MUL64 instruction with various scenarios

.test "mul64 basic multiplication" {
    .description "Basic 64-bit multiplication test"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "mul64"
    
    LOAD_IMM64 EAX, 1000
    LOAD_IMM64 EBX, 2000
    MUL64 ECX, EAX, EBX    ; ECX = 1000 * 2000 = 2,000,000
    HALT
    
    .assert_reg ECX, 2000000
}

.test "mul64 edge case small values" {
    .description "Test multiplication of small values"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "edge-case"
    
    LOAD_IMM EAX, 1
    LOAD_IMM EBX, 1
    MUL64 ECX, EAX, EBX    ; ECX = 1 * 1 = 1
    HALT
    
    .assert_reg ECX, 1
}

.test "mul64 zero multiplication" {
    .description "Test multiplication by zero"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "zero"
    
    LOAD_IMM64 EAX, 12345
    LOAD_IMM EBX, 0
    MUL64 ECX, EAX, EBX    ; ECX = 12345 * 0 = 0
    HALT
    
    .assert_reg ECX, 0
}

.test "mul64 large numbers" {
    .description "Test multiplication of large numbers (within 64-bit range)"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "large-values"
    
    LOAD_IMM64 EAX, 1000000
    LOAD_IMM64 EBX, 1000
    MUL64 ECX, EAX, EBX    ; ECX = 1,000,000 * 1,000 = 1,000,000,000
    HALT
    
    .assert_reg ECX, 1000000000
}

.test "mul64 signed numbers" {
    .description "Test multiplication with signed numbers (positive result)"
    .author "bobrossrtx"
    .category "64-bit Operations"
    .tag "mul64"
    .tag "signed"
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 50
    MUL64 ECX, EAX, EBX    ; ECX = 100 * 50 = 5000
    HALT
    
    .assert_reg ECX, 5000
}