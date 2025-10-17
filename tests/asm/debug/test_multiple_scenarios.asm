; Multiple test scenarios in one file
#test "test_simple_immediate" {
    #description "Test simple immediate load"
    #category "Debug"
    #tag "basic"
    
    LOAD_IMM R0, 42
    #assert_reg R0, 42
    HALT
}

#test "test_countdown_loop" {
    #description "Test countdown loop with comparison"
    #category "Debug"
    #tag "loops"
    
    LOAD_IMM R0, 3    ; Start at 3
    
countdown_loop:
    DEC R0             ; Decrement counter
    CMP R0, 0          ; Compare with 0
    JG countdown_loop  ; Jump if greater than 0
    
    #assert_reg R0, 0  ; Should end at 0
    HALT
}