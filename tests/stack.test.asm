; Stack Operations Test Suite
; Tests for stack push, pop, and stack pointer operations

.test "push and pop single value" {
    .description "Tests basic PUSH and POP stack operations with single value"
    .author "DemiEngine Team"
    .category "Stack"
    .tag "basic"
    .tag "push"
    .tag "pop"
    LOAD_IMM R0, 42
    PUSH R0
    LOAD_IMM R0, 0   ; Clear R0
    POP R1
    .assert_reg R1, 42
}

.test "push and pop multiple values" {
    .description "Tests pushing and popping multiple values maintains correct order"
    .author "DemiEngine Team"
    .category "Stack"
    .tag "push"
    .tag "pop"
    .tag "multi-value"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    
    PUSH R0
    PUSH R1
    PUSH R2
    
    POP R3   ; Should be 30 (last in, first out)
    POP R4   ; Should be 20
    POP R5   ; Should be 10
    
    .assert_reg R3, 30
    .assert_reg R4, 20
    .assert_reg R5, 10
}

.test "stack LIFO order" {
    .description "Tests that stack follows Last-In-First-Out ordering"
    .author "DemiEngine Team"
    .category "Stack"
    .tag "push"
    .tag "pop"
    .tag "lifo"
    LOAD_IMM R0, 100
    LOAD_IMM R1, 200
    
    PUSH R0
    PUSH R1
    
    ; Pop in reverse order
    POP R2   ; Should get 200
    POP R3   ; Should get 100
    
    .assert_reg R2, 200
    .assert_reg R3, 100
}

.test "push and pop flags" {
    .description "Tests PUSH_FLAG and POP_FLAG instructions for saving/restoring CPU flags"
    .author "DemiEngine Team"
    .category "Stack"
    .tag "flags"
    .tag "push"
    .tag "pop"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 5
    CMP R0, R1      ; Set flags (R0 > R1)
    PUSH_FLAG       ; Save current flags
    
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    CMP R0, R1      ; Change flags (R0 < R1)
    
    POP_FLAG        ; Restore original flags (R0 > R1)
    JG greater      ; Should jump since original comparison was greater
    LOAD_IMM R2, 99
    JMP end
greater:
    LOAD_IMM R2, 42
end:
    .assert_reg R2, 42
}

.test "call and return" {
    .description "Tests CALL and RET instructions for subroutine functionality"
    .author "DemiEngine Team"
    .category "Stack Operations"
    .tag "call"
    .tag "return"
    .tag "subroutine"
    
    LOAD_IMM R0, 10
    CALL subroutine
    .assert_reg R0, 20  ; Should be doubled by subroutine
    JMP end_test
    
subroutine:
    ADD R0, R0      ; Double the value in R0
    RET
    
end_test:
    ; Test completed
}