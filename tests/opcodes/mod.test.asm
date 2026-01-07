; ==========================================
; MOD Opcode Test Suite
; Tests modulo (remainder) operation
; ==========================================

.test "mod_basic" {
    .description "Tests basic modulo operation"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "basic"
    
    LOAD_IMM RAX, 10
    LOAD_IMM RBX, 3
    MOD RAX, RAX, RBX
    
    .assert_reg RAX, 1  ; 10 % 3 = 1
}

.test "mod_zero_remainder" {
    .description "Tests modulo with zero remainder"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "zero"
    
    LOAD_IMM RCX, 15
    LOAD_IMM RDX, 5
    MOD R0, RCX, RDX
    
    .assert_reg R0, 0  ; 15 % 5 = 0
}

.test "mod_by_one" {
    .description "Tests modulo by 1 (always 0)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "one"
    
    LOAD_IMM R1, 42
    LOAD_IMM R2, 1
    MOD R3, R1, R2
    
    .assert_reg R3, 0
}

.test "mod_larger_dividend" {
    .description "Tests modulo with larger dividend"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "large"
    
    LOAD_IMM R5, 100
    LOAD_IMM R6, 7
    MOD R7, R5, R6
    
    .assert_reg R7, 2  ; 100 % 7 = 2
}

.test "mod_power_of_two" {
    .description "Tests modulo with power of 2 divisor"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "power2"
    
    LOAD_IMM RAX, 25
    LOAD_IMM RBX, 8
    MOD RCX, RAX, RBX
    
    .assert_reg RCX, 1  ; 25 % 8 = 1
}

.test "mod_same_value" {
    .description "Tests modulo of same value (result is 0)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "same"
    
    LOAD_IMM RDX, 17
    MOD R0, RDX, RDX
    
    .assert_reg R0, 0
}

.test "mod_divisor_larger" {
    .description "Tests modulo when divisor > dividend"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "larger"
    
    LOAD_IMM R1, 5
    LOAD_IMM R2, 10
    MOD R3, R1, R2
    
    .assert_reg R3, 5  ; 5 % 10 = 5
}

.test "mod_digit_extraction" {
    .description "Tests modulo for digit extraction (itoa pattern)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "digit"
    
    LOAD_IMM R5, 123
    LOAD_IMM R6, 10
    MOD R7, R5, R6
    
    .assert_reg R7, 3  ; Extract last digit
}

.test "mod_even_odd_check" {
    .description "Tests modulo for even/odd checking"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "check"
    
    LOAD_IMM RAX, 42
    LOAD_IMM RBX, 2
    MOD RCX, RAX, RBX
    
    .assert_reg RCX, 0  ; 42 is even
}

.test "mod_sequential" {
    .description "Tests sequential modulo operations"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mod"
    .tag "sequential"
    
    LOAD_IMM RDX, 20
    LOAD_IMM R0, 6
    MOD R1, RDX, R0    ; 20 % 6 = 2
    MOD R2, R1, R0     ; 2 % 6 = 2
    
    .assert_reg R1, 2
    .assert_reg R2, 2
}
