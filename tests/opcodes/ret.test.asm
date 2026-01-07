; ==========================================
; RET Opcode Test Suite
; Tests return from subroutine
; ==========================================

.test "ret_basic" {
    .description "Tests basic RET functionality"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
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

.test "ret_nested_calls" {
    .description "Tests RET from nested function calls"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "nested"
    
    LOAD_IMM RBX, 5
    CALL double_value
    .assert_reg RBX, 10
    JMP test_end
double_value:
    ADD64 RBX, RBX  ; RBX *= 2
    RET
test_end:
}

.test "ret_multiple_returns" {
    .description "Tests multiple RET instructions in sequence"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "multiple"
    
    LOAD_IMM RCX, 1
    CALL func1
    CALL func2
    CALL func3
    .assert_reg RCX, 4
    JMP test_end
func1:
    INC64 RCX
    RET
func2:
    INC64 RCX
    RET
func3:
    INC64 RCX
    RET
test_end:
}

.test "ret_with_stack_cleanup" {
    .description "Tests RET properly manages stack pointer"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "stack"
    
    LOAD_IMM RDX, 100
    PUSH RDX
    CALL stack_func
    POP R0
    .assert_reg R0, 100
    .assert_reg RDX, 101
    JMP test_end
stack_func:
    INC64 RDX
    RET
test_end:
}

.test "ret_preserves_registers" {
    .description "Tests RET preserves all non-modified registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "preserve"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    CALL modify_r0
    .assert_reg R0, 11
    .assert_reg R1, 20
    JMP test_end
modify_r0:
    INC64 R0
    RET
test_end:
}

.test "ret_from_conditional_call" {
    .description "Tests RET from conditionally called function"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "conditional"
    
    LOAD_IMM R2, 5
    LOAD_IMM R3, 5
    CMP R2, R3
    JZ call_func
    JMP skip_func
call_func:
    CALL set_value
skip_func:
    .assert_reg R2, 99
    JMP test_end
set_value:
    LOAD_IMM R2, 99
    RET
test_end:
}

.test "ret_deep_nesting" {
    .description "Tests RET from deeply nested calls"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "deep"
    
    LOAD_IMM R6, 0
    CALL level1
    .assert_reg R6, 3
    JMP test_end
level1:
    INC64 R6
    CALL level2
    RET
level2:
    INC64 R6
    CALL level3
    RET
level3:
    INC64 R6
    RET
test_end:
}

.test "ret_with_value_passing" {
    .description "Tests RET with value passing through registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "ret"
    .tag "passing"
    
    LOAD_IMM R6, 7
    LOAD_IMM R7, 3
    CALL multiply_values
    .assert_reg RAX, 21
    JMP test_end
multiply_values:
    MUL64 RAX, R6, R7
    RET
test_end:
}
