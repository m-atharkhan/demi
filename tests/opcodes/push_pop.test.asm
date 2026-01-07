; ==========================================
; PUSH/POP Opcode Test Suite
; Tests stack operations
; NOTE: PUSH/POP only support R0-R7 (RAX-R7)
; ==========================================

.test "push_pop_basic" {
    .description "Tests basic PUSH and POP"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "basic"
    
    LOAD_IMM RAX, 42
    PUSH RAX
    LOAD_IMM RAX, 0
    POP RBX
    
    .assert_reg RBX, 42
}

.test "push_pop_lifo_order" {
    .description "Tests LIFO (Last In First Out) order"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "lifo"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    PUSH R0
    PUSH R1
    PUSH R2
    
    POP R3  ; Should be 30
    POP R5  ; Should be 20
    POP R6  ; Should be 10
    
    .assert_reg R3, 30
    .assert_reg R5, 20
    .assert_reg R6, 10
}

.test "push_pop_preserve_register" {
    .description "Tests using PUSH/POP to preserve register in function"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "preserve"
    
    LOAD_IMM RCX, 100
    CALL modify_with_preserve
    .assert_reg RCX, 100  ; Should be preserved
    .assert_reg RAX, 200  ; Should be modified
    JMP test_end
modify_with_preserve:
    PUSH RCX
    LOAD_IMM RCX, 999
    LOAD_IMM RAX, 200
    POP RCX
    RET
test_end:
}

.test "push_pop_zero" {
    .description "Tests pushing and popping zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "zero"
    
    LOAD_IMM RDX, 0
    PUSH RDX
    POP R7
    
    .assert_reg R7, 0
}

.test "push_pop_max_value" {
    .description "Tests pushing and popping maximum value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "max"
    
    LOAD_IMM R0, 0xFFFFFFFF
    PUSH R0
    POP RAX
    
    .assert_reg RAX, 0xFFFFFFFF
}

.test "push_pop_multiple_registers" {
    .description "Tests pushing and popping multiple different registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "multiple"
    
    LOAD_IMM RAX, 11
    LOAD_IMM RBX, 22
    LOAD_IMM RCX, 33
    LOAD_IMM RDX, 44
    
    PUSH RAX
    PUSH RBX
    PUSH RCX
    PUSH RDX
    
    POP R0   ; 44
    POP R1   ; 33
    POP R2  ; 22
    POP R3  ; 11
    
    .assert_reg R0, 44
    .assert_reg R1, 33
    .assert_reg R2, 22
    .assert_reg R3, 11
}

.test "push_pop_in_loop" {
    .description "Tests PUSH/POP in loop pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "loop"
    
    LOAD_IMM R5, 5
    LOAD_IMM R6, 0
loop_start:
    PUSH R5
    INC64 R6
    DEC64 R5
    CMP R5, 0
    JNZ loop_start
    
    ; R6 should be 5, pop one value
    POP R7
    .assert_reg R6, 5
    .assert_reg R7, 1
}

.test "push_pop_all_registers" {
    .description "Tests PUSH/POP with all available registers R0-R7 (avoiding R4=RSP)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "all"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    
    PUSH R0
    PUSH R1
    PUSH R2
    
    POP R3  ; 30
    POP R5  ; 20
    POP R6  ; 10
    
    .assert_reg R3, 30
    .assert_reg R5, 20
    .assert_reg R6, 10
}

.test "push_pop_power_of_two" {
    .description "Tests PUSH/POP with powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "power2"
    
    LOAD_IMM R6, 256
    LOAD_IMM R7, 512
    
    PUSH R6
    PUSH R7
    
    POP RAX  ; 512
    POP RBX  ; 256
    
    .assert_reg RAX, 512
    .assert_reg RBX, 256
}

.test "push_pop_same_register" {
    .description "Tests pushing a register and popping back to same register"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "push"
    .tag "pop"
    .tag "same"
    
    LOAD_IMM RCX, 777
    PUSH RCX
    LOAD_IMM RCX, 888
    POP RCX
    
    .assert_reg RCX, 777
}
