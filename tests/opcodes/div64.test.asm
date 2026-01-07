; ==========================================
; DIV64 Opcode Test Suite
; Tests 64-bit division with remainder storage in RDX
; Critical: Tests the x86-64 DIV convention fix
; ==========================================

.test "div64_basic_with_remainder" {
    .description "Tests basic division where dividend < divisor, checking remainder in RDX"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "remainder"
    
    ; 8 / 10 = quotient:0, remainder:8
    LOAD_IMM RAX, 8
    LOAD_IMM RCX, 10
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 0    ; Quotient should be 0
    .assert_reg RDX, 8    ; Remainder should be 8 (THE BUG FIX!)
}

.test "div64_exact_division" {
    .description "Tests division with no remainder"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "exact"
    
    ; 42 / 6 = quotient:7, remainder:0
    LOAD_IMM RAX, 42
    LOAD_IMM RCX, 6
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 7    ; Quotient
    .assert_reg RDX, 0    ; No remainder
}

.test "div64_large_numbers" {
    .description "Tests division with large numbers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "large"
    
    ; 1000000 / 7 = quotient:142857, remainder:1
    LOAD_IMM RAX, 1000000
    LOAD_IMM RCX, 7
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 142857
    .assert_reg RDX, 1
}

.test "div64_by_one" {
    .description "Tests division by 1 (result equals dividend)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "edge"
    
    ; 12345 / 1 = quotient:12345, remainder:0
    LOAD_IMM RAX, 12345
    LOAD_IMM RCX, 1
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 12345
    .assert_reg RDX, 0
}

.test "div64_max_remainder" {
    .description "Tests division producing maximum possible remainder (divisor-1)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "edge"
    
    ; 999 / 1000 = quotient:0, remainder:999
    LOAD_IMM RAX, 999
    LOAD_IMM RCX, 1000
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 0
    .assert_reg RDX, 999
}

.test "div64_itoa_pattern" {
    .description "Tests the itoa pattern: divide by 10 to extract digit"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "itoa"
    
    ; This is the pattern used in calculator's itoa function
    ; 42 / 10 = quotient:4, remainder:2 (the digit!)
    LOAD_IMM RAX, 42
    LOAD_IMM RCX, 10
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 4    ; Next iteration value
    .assert_reg RDX, 2    ; Digit to convert to ASCII
}

.test "div64_zero_dividend" {
    .description "Tests division of zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "zero"
    
    ; 0 / 5 = quotient:0, remainder:0
    LOAD_IMM RAX, 0
    LOAD_IMM RCX, 5
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 0
    .assert_reg RDX, 0
}

.test "div64_power_of_two" {
    .description "Tests division by power of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "power2"
    
    ; 128 / 16 = quotient:8, remainder:0
    LOAD_IMM RAX, 128
    LOAD_IMM RCX, 16
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 8
    .assert_reg RDX, 0
}

.test "div64_self_division" {
    .description "Tests dividing a number by itself"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "edge"
    
    ; 777 / 777 = quotient:1, remainder:0
    LOAD_IMM RAX, 777
    LOAD_IMM RCX, 777
    DIV64 RAX, RAX, RCX
    
    .assert_reg RAX, 1
    .assert_reg RDX, 0
}

.test "div64_extended_registers" {
    .description "Tests DIV64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "div64"
    .tag "extended"
    
    ; Use R8-R15 to verify extended register support
    LOAD_IMM R8, 100
    LOAD_IMM R9, 7
    DIV64 R8, R8, R9
    
    .assert_reg R8, 14   ; Quotient
    .assert_reg RDX, 2   ; Remainder still goes to RDX
}
