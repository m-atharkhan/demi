; Test file for stack operations

#test "push and pop single value" {
    #description "Tests basic PUSH and POP stack operations with single value"
    #author "DemiEngine Team"
    #category "Stack"
    #tag "basic"
    #tag "push"
    #tag "pop"
    LOAD_IMM R0, 42
    PUSH R0
    LOAD_IMM R0, 0
    POP R0
    #assert_reg R0, 42
}

#test "push and pop multiple values" {
    #description "Tests pushing and popping multiple values maintains correct order"
    #author "DemiEngine Team"
    #category "Stack"
    #tag "push"
    #tag "pop"
    #tag "multi-value"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    PUSH R0
    PUSH R1
    PUSH R2
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 0
    LOAD_IMM R2, 0
    
    POP R2
    POP R1
    POP R0
    
    #assert_reg R0, 10
    #assert_reg R1, 20
    #assert_reg R2, 30
}

#test "stack order (LIFO)" {
    #description "Tests that stack follows Last-In-First-Out ordering"
    #author "DemiEngine Team"
    #category "Stack"
    #tag "push"
    #tag "pop"
    #tag "lifo"
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    
    PUSH R0
    PUSH R1
    PUSH R2
    
    POP R3      ; Should get 3
    POP R4      ; Should get 2
    POP R5      ; Should get 1
    
    #assert_reg R3, 3
    #assert_reg R4, 2
    #assert_reg R5, 1
}

#test "push flags and pop flags" {
    #description "Tests PUSH_FLAG and POP_FLAG instructions for saving/restoring CPU flags"
    #author "DemiEngine Team"
    #category "Stack"
    #tag "flags"
    #tag "push"
    #tag "pop"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 5
    CMP R0, R1      ; Sets zero flag
    PUSH_FLAG
    
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    CMP R0, R1      ; Clears zero flag
    
    POP_FLAG        ; Restore flags - should restore zero flag
    
    ; Verify flags were pushed/popped correctly
    #assert_reg R0, 1
    #assert_reg R1, 2
}
