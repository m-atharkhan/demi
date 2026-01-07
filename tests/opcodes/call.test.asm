; ==========================================
; CALL/RET Opcode Test Suite
; Tests subroutine calls and returns
; ==========================================

.test "call_simple_function" {
    .description "Tests basic CALL and RET"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "basic"
    
    LOAD_IMM RAX, 0
    CALL increment_rax
    .assert_reg RAX, 1
    JMP test_end
increment_rax:
    INC64 RAX
    RET
test_end:
}

.test "call_with_parameters" {
    .description "Tests CALL with parameter passing via registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "parameters"
    
    LOAD_IMM RAX, 10
    LOAD_IMM RBX, 20
    CALL add_registers
    .assert_reg RAX, 30
    JMP test_end
add_registers:
    ADD64 RAX, RBX
    RET
test_end:
}

.test "call_nested" {
    .description "Tests nested function calls"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "nested"
    
    LOAD_IMM RAX, 5
    CALL double_and_increment
    .assert_reg RAX, 11  ; (5 * 2) + 1
    JMP test_end
double_and_increment:
    ADD64 RAX, RAX
    CALL add_one
    RET
add_one:
    INC64 RAX
    RET
test_end:
}

.test "call_multiple_times" {
    .description "Tests calling same function multiple times"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "multiple"
    
    LOAD_IMM RCX, 0
    CALL inc_rcx
    CALL inc_rcx
    CALL inc_rcx
    .assert_reg RCX, 3
    JMP test_end
inc_rcx:
    INC64 RCX
    RET
test_end:
}

.test "call_with_return_value" {
    .description "Tests CALL with return value in register"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "return"
    
    LOAD_IMM RAX, 7
    CALL square
    .assert_reg RAX, 49
    JMP test_end
square:
    MOV64 RBX, RAX
    MUL64 RAX, RAX, RBX
    RET
test_end:
}

.test "call_extended_registers" {
    .description "Tests CALL with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "extended"
    
    LOAD_IMM R8, 100
    CALL double_r8
    .assert_reg R8, 200
    JMP test_end
double_r8:
    ADD64 R8, R8
    RET
test_end:
}

.test "call_chain" {
    .description "Tests chain of function calls"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "chain"
    
    LOAD_IMM R9, 1
    CALL func_a
    .assert_reg R9, 4  ; 1+1+1+1
    JMP test_end
func_a:
    INC64 R9
    CALL func_b
    RET
func_b:
    INC64 R9
    CALL func_c
    RET
func_c:
    INC64 R9
    RET
test_end:
}

.test "call_conditional" {
    .description "Tests conditional function call"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "conditional"
    
    LOAD_IMM R10, 10
    LOAD_IMM R11, 10
    CMP64 R10, R11
    JZ call_it
    JMP skip_call
call_it:
    CALL set_value
skip_call:
    .assert_reg R12, 777
    JMP test_end
set_value:
    LOAD_IMM R12, 777
    RET
test_end:
}

.test "call_preserve_registers" {
    .description "Tests that CALL preserves non-modified registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "preserve"
    
    LOAD_IMM R13, 100
    LOAD_IMM R14, 200
    CALL modify_r13
    .assert_reg R13, 101  ; Modified
    .assert_reg R14, 200  ; Preserved
    JMP test_end
modify_r13:
    INC64 R13
    RET
test_end:
}

.test "call_return_immediately" {
    .description "Tests function that returns immediately"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "call"
    .tag "immediate"
    
    LOAD_IMM R15, 42
    CALL noop
    .assert_reg R15, 42
    JMP test_end
noop:
    RET
test_end:
}
