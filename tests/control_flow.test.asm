; Control Flow Test Suite
; Tests for jumps, conditionals, and control flow operations

.test "unconditional jump" {
    .description "Tests unconditional JMP instruction"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "jump"
    .tag "basic"
    LOAD_IMM EAX, 0
    JMP skip
    LOAD_IMM EAX, 99  ; This should be skipped
skip:
    LOAD_IMM EBX, 42
    .assert_reg EAX, 0
    .assert_reg EBX, 42
}

.test "compare and conditional jump" {
    .description "Tests CMP instruction with conditional jumps"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "conditional"
    .tag "comparison"
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 10
    CMP EAX, EBX
    JZ equal
    LOAD_IMM ECX, 99  ; Should not execute
    JMP end
equal:
    LOAD_IMM ECX, 42
end:
    .assert_reg ECX, 42
}

.test "jump if greater" {
    .description "Tests JG conditional jump when first operand is greater"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "conditional"
    .tag "comparison"
    LOAD_IMM EAX, 15
    LOAD_IMM EBX, 10
    CMP EAX, EBX
    JG greater
    LOAD_IMM ECX, 99
    JMP end
greater:
    LOAD_IMM ECX, 42
end:
    .assert_reg ECX, 42
}

.test "jump if less" {
    .description "Tests JL conditional jump when first operand is less"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "conditional"
    .tag "comparison"
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 10
    CMP EAX, EBX
    JL less
    LOAD_IMM ECX, 99
    JMP end
less:
    LOAD_IMM ECX, 42
end:
    .assert_reg ECX, 42
}

.test "simple loop with counter" {
    .description "Test loop with iteration counter"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "loop"
    .tag "counter"
    LOAD_IMM EAX, 0   ; Counter
    LOAD_IMM EBX, 0   ; Sum
    LOAD_IMM ECX, 5   ; Limit
loop:
    ADD EBX, EAX       ; Add counter to sum
    INC EAX           ; Increment counter
    CMP EAX, ECX       ; Compare with limit
    JL loop          ; Continue if less
    ; Final sum should be 0+1+2+3+4 = 10
    .assert_reg EBX, 10
    .assert_reg EAX, 5
}

.test "halt instruction" {
    .description "Tests HALT instruction to stop execution"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "halt"
    .tag "termination"
    LOAD_IMM EAX, 42
    HALT
    LOAD_IMM EBX, 99  ; This should never execute
    .assert_reg EAX, 42
}

.test "label definition and usage" {
    .description "Test label definition and usage in code"
    .author "DemiEngine Team"
    .category "Control Flow"
    .tag "labels"
    .tag "jumps"
    .tag "control-flow"

    LOAD_IMM EAX, 10
    JMP end_label    ; Jump to end label
    
    ; This should be skipped
    LOAD_IMM EAX, 99
    
end_label:
    LOAD_IMM EBX, 20
    
    .assert_reg EAX, 10  ; Should still be 10 (not 99)
    .assert_reg EBX, 20
}