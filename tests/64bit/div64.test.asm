; 64-bit Division Tests
; Tests for DIV64 instruction with various scenarios

.test "div64 basic division" {
    .description "Basic 64-bit division test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    
    LOAD_IMM64 R0, 2000000
    LOAD_IMM64 R1, 1000
    DIV64 R2, R0, R1    ; R2 = 2,000,000 / 1,000 = 2,000
    HALT
    
    .assert_reg R2, 2000
}

.test "div64 exact division" {
    .description "Test exact division with no remainder"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    .tag "exact"
    
    LOAD_IMM R0, 144
    LOAD_IMM R1, 12
    DIV64 R2, R0, R1    ; R2 = 144 / 12 = 12
    HALT
    
    .assert_reg R2, 12
}

.test "div64 division by one" {
    .description "Test division by one (identity)"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    .tag "identity"
    
    LOAD_IMM64 R0, 987654
    LOAD_IMM R1, 1
    DIV64 R2, R0, R1    ; R2 = 987654 / 1 = 987654
    HALT
    
    .assert_reg R2, 987654
}

.test "div64 integer truncation" {
    .description "Test division with remainder (integer truncation)"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    .tag "truncation"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 7
    DIV64 R2, R0, R1    ; R2 = 100 / 7 = 14 (remainder 2, but we only get quotient)
    HALT
    
    .assert_reg R2, 14
}

.test "div64 zero dividend" {
    .description "Test division of zero"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    .tag "zero"
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 42
    DIV64 R2, R0, R1    ; R2 = 0 / 42 = 0
    HALT
    
    .assert_reg R2, 0
}

.test "div64 division by zero error" {
    .description "Test division by zero should cause error"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "div64"
    .tag "error"
    .expect_error
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 0
    DIV64 R2, R0, R1    ; This should cause a division by zero error
    HALT
}