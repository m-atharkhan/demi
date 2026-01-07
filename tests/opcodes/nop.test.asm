; ==========================================
; NOP Opcode Test Suite
; Tests no operation instruction
; ==========================================

.test "nop_basic" {
    .description "Tests basic NOP does nothing"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "basic"
    
    LOAD_IMM RAX, 42
    NOP
    .assert_reg RAX, 42
}

.test "nop_multiple" {
    .description "Tests multiple consecutive NOPs"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "multiple"
    
    LOAD_IMM RBX, 100
    NOP
    NOP
    NOP
    NOP
    NOP
    .assert_reg RBX, 100
}

.test "nop_between_operations" {
    .description "Tests NOP between operations doesn't affect result"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "operations"
    
    LOAD_IMM RCX, 5
    NOP
    INC64 RCX
    NOP
    INC64 RCX
    NOP
    .assert_reg RCX, 7
}

.test "nop_after_jump" {
    .description "Tests NOP after jump target"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "jump"
    
    LOAD_IMM RDX, 1
    JMP target
    HALT
target:
    NOP
    INC64 RDX
    .assert_reg RDX, 2
}

.test "nop_in_loop" {
    .description "Tests NOP inside loop"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "loop"
    
    LOAD_IMM R0, 3
    LOAD_IMM R1, 0
loop_start:
    NOP
    INC64 R1
    DEC64 R0
    CMP R0, 0
    JNZ loop_start
    
    .assert_reg R1, 3
}

.test "nop_with_all_registers" {
    .description "Tests NOP doesn't affect any registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "registers"
    
    LOAD_IMM R2, 10
    LOAD_IMM R3, 20
    LOAD_IMM R5, 30
    NOP
    NOP
    .assert_reg R2, 10
    .assert_reg R3, 20
    .assert_reg R5, 30
}

.test "nop_before_call" {
    .description "Tests NOP before function call"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "call"
    
    LOAD_IMM R6, 0
    NOP
    CALL increment
    .assert_reg R6, 1
    JMP end
increment:
    INC64 R6
    RET
end:
}

.test "nop_alignment" {
    .description "Tests NOP for code alignment patterns"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "nop"
    .tag "alignment"
    
    LOAD_IMM R7, 255
    NOP
    NOP
    NOP
    NOP
    INC64 R7
    .assert_reg R7, 256
}
