; ==========================================
; DEC64 Opcode Test Suite
; Tests 64-bit decrement operation
; ==========================================

.test "dec64_to_zero" {
    .description "Tests decrementing to zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "zero"
    
    LOAD_IMM RAX, 1
    DEC64 RAX
    
    .assert_reg RAX, 0
}

.test "dec64_basic" {
    .description "Tests basic decrement"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "basic"
    
    LOAD_IMM RBX, 100
    DEC64 RBX
    
    .assert_reg RBX, 99
}

.test "dec64_multiple" {
    .description "Tests multiple sequential decrements"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "sequential"
    
    LOAD_IMM RCX, 10
    DEC64 RCX
    DEC64 RCX
    DEC64 RCX
    
    .assert_reg RCX, 7
}

.test "dec64_countdown_loop" {
    .description "Tests DEC64 in countdown loop pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "loop"
    
    LOAD_IMM RDX, 10
    LOAD_IMM R8, 0
countdown:
    INC64 R8
    DEC64 RDX
    CMP RDX, 0
    JNZ countdown
    
    .assert_reg R8, 10
}

.test "dec64_extended_registers" {
    .description "Tests DEC64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "extended"
    
    LOAD_IMM R9, 1000
    DEC64 R9
    
    .assert_reg R9, 999
}

.test "dec64_large_value" {
    .description "Tests decrementing large value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "large"
    
    LOAD_IMM R10, 1000000
    DEC64 R10
    
    .assert_reg R10, 999999
}

.test "dec64_power_of_two_boundary" {
    .description "Tests decrementing at power of 2 boundary"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "power2"
    
    LOAD_IMM R11, 256
    DEC64 R11
    
    .assert_reg R11, 255
}

.test "dec64_loop_counter" {
    .description "Tests DEC64 as loop counter"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "counter"
    
    LOAD_IMM R12, 5
loop_start:
    DEC64 R12
    CMP R12, 0
    JNZ loop_start
    
    .assert_reg R12, 0
}

.test "dec64_consecutive_values" {
    .description "Tests decrementing to produce consecutive descending values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "consecutive"
    
    LOAD_IMM R13, 100
    DEC64 R13  ; 99
    DEC64 R13  ; 98
    DEC64 R13  ; 97
    
    .assert_reg R13, 97
}

.test "dec64_from_ten" {
    .description "Tests decrementing from ten"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "dec64"
    .tag "basic"
    
    LOAD_IMM R14, 10
    DEC64 R14
    
    .assert_reg R14, 9
}
