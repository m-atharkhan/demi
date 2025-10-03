; Comprehensive test showcasing all test framework features

#test "showcase - all assertions" {
    #description "Demonstrates multiple assertion types in a single test"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "comprehensive"
    #tag "assertions"
    ; Test register assertions
    LOAD_IMM R0, 42
    
    ; Test memory assertions
    STORE R0, 100
    #assert_mem 100, 42
    
    ; Multiple operations
    LOAD_IMM R1, 10
    ADD R0, R1
    
    ; Assertions checked at end
    #assert_reg R0, 52
}

#test "showcase - complex workflow" {
    #description "Shows a complete workflow with initialization, storage, and processing"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "comprehensive"
    #tag "workflow"
    ; Initialize data
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    
    ; Store to memory
    STORE R0, 50
    STORE R1, 51  
    STORE R2, 52
    
    ; Verify memory
    #assert_mem 50, 10
    #assert_mem 51, 20
    #assert_mem 52, 30
    
    ; Process data
    LOAD R3, 50
    LOAD R4, 51
    ADD R3, R4
    
    ; Verify result
    #assert_reg R3, 30
}

#test "showcase - error handling" {
    #description "Demonstrates testing expected error conditions"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "error-handling"
    #tag "expect-error"
    LOAD_IMM R0, 100
    LOAD_IMM R1, 0
    DIV R0, R1
    #expect_error
}

#test "showcase - control flow" {
    #description "Demonstrates conditional jumps and control flow"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "control-flow"
    #tag "jumps"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    CMP R0, R1
    JL less_than
    LOAD_IMM R2, 0
    JMP done
less_than:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "showcase - stack operations" {
    #description "Demonstrates stack push and pop operations"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "stack"
    #tag "push-pop"
    LOAD_IMM R0, 111
    LOAD_IMM R1, 222
    
    PUSH R0
    PUSH R1
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 0
    
    POP R1
    POP R0
    
    #assert_reg R0, 111
    #assert_reg R1, 222
}

#test "showcase - bitwise magic" {
    #description "Demonstrates XOR swap trick for swapping values without temp register"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "bitwise"
    #tag "xor-swap"
    ; XOR swap without temp variable
    LOAD_IMM R0, 42
    LOAD_IMM R1, 99
    
    XOR R0, R1
    XOR R1, R0
    XOR R0, R1
    
    #assert_reg R0, 99
    #assert_reg R1, 42
}

#test "showcase - data processing" {
    #description "Demonstrates data processing with multiple arithmetic operations"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "arithmetic"
    #tag "average"
    ; Calculate average of 4 numbers: 10, 20, 30, 40
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1      ; 30
    
    LOAD_IMM R2, 30
    ADD R0, R2      ; 60
    
    LOAD_IMM R3, 40
    ADD R0, R3      ; 100
    
    LOAD_IMM R4, 4
    DIV R0, R4      ; 25
    
    #assert_reg R0, 25
}

#test "showcase - extended registers" {
    #description "Demonstrates using extended registers beyond R0-R3"
    #author "DemiEngine Team"
    #category "Showcase"
    #tag "registers"
    #tag "extended"
    LOAD_IMM R4, 111
    LOAD_IMM R5, 222
    LOAD_IMM R6, 99
    
    #assert_reg R4, 111
    #assert_reg R5, 222
    #assert_reg R6, 99
}
