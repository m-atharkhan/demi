; ==========================================
; JMP (Unconditional Jump) Opcode Test Suite
; Tests unconditional jumps and control flow
; ==========================================

.test "jmp_forward" {
    .description "Tests forward unconditional jump"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "forward"
    
    JMP target
    LOAD_IMM RAX, 999  ; Should be skipped
    HALT
target:
    LOAD_IMM RAX, 1
    .assert_reg RAX, 1
}

.test "jmp_skip_block" {
    .description "Tests jumping over code block"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "skip"
    
    JMP after_block
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, 2
    LOAD_IMM RDX, 3
after_block:
    LOAD_IMM R8, 100
    .assert_reg R8, 100
}

.test "jmp_if_else_structure" {
    .description "Tests JMP implementing if-else structure"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "if_else"
    
    LOAD_IMM RDX, 10
    CMP RDX, 10
    JZ then_branch
    LOAD_IMM R9, 100  ; else
    JMP after_if
then_branch:
    LOAD_IMM R9, 200  ; then
after_if:
    .assert_reg R9, 200
}

.test "jmp_loop_exit" {
    .description "Tests JMP for loop exit"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "loop"
    
    LOAD_IMM R10, 0
    LOAD_IMM R11, 5
loop_start:
    INC64 R10
    DEC64 R11
    CMP R11, 0
    JZ loop_end
    JMP loop_start
loop_end:
    .assert_reg R10, 5
}

.test "jmp_switch_case_pattern" {
    .description "Tests JMP in switch/case pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "switch"
    
    LOAD_IMM R12, 2
    CMP R12, 1
    JZ case_1
    CMP R12, 2
    JZ case_2
    CMP R12, 3
    JZ case_3
    JMP default_case
case_1:
    LOAD_IMM R13, 100
    JMP case_end
case_2:
    LOAD_IMM R13, 200
    JMP case_end
case_3:
    LOAD_IMM R13, 300
    JMP case_end
default_case:
    LOAD_IMM R13, 999
case_end:
    .assert_reg R13, 200
}

.test "jmp_nested_branches" {
    .description "Tests nested branching with JMP"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "nested"
    
    LOAD_IMM R14, 1
    CMP R14, 0
    JZ skip1
    LOAD_IMM R15, 2
    CMP R15, 0
    JZ skip2
    LOAD_IMM RAX, 42
    JMP done
skip2:
    LOAD_IMM RAX, 99
    JMP done
skip1:
    LOAD_IMM RAX, 88
done:
    .assert_reg RAX, 42
}

.test "jmp_sequential_jumps" {
    .description "Tests sequential unconditional jumps"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "sequential"
    
    JMP step1
step1:
    JMP step2
step2:
    JMP step3
step3:
    LOAD_IMM RBX, 77
    .assert_reg RBX, 77
}

.test "jmp_far_target" {
    .description "Tests jumping to distant target"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "far"
    
    JMP far_target
    LOAD_IMM RCX, 1
    LOAD_IMM RDX, 2
    LOAD_IMM R8, 3
    LOAD_IMM R9, 4
    LOAD_IMM R10, 5
far_target:
    LOAD_IMM R11, 888
    .assert_reg R11, 888
}

.test "jmp_return_pattern" {
    .description "Tests JMP simulating function return pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jmp"
    .tag "return"
    
    LOAD_IMM R12, 10
    JMP function
after_call:
    .assert_reg R12, 11
    JMP test_end
function:
    INC64 R12
    JMP after_call
test_end:
}
