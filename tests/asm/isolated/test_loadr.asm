; Test LOADR instruction
#test "test_loadr" {
    #description "Test LOADR instruction"
    #category "Debug"
    
    ; Store value 42 at memory address 0x50
    LOAD_IMM R0, 42
    STORE R0, 0x50
    
    ; Store address 0x50 in R1
    LOAD_IMM R1, 0x50
    
    ; Use LOADR to load from address in R1
    LOADR R2, R1
    
    ; R2 should now contain 42
    #assert_reg R2, 42
    
    HALT
}