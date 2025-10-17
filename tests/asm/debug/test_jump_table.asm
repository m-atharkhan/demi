; Specialized jump table test
#test "test_jump_table_basic" {
    #description "Test basic jump table logic without JZ"
    #category "Debug"
    #tag "control-flow"
    
    LOAD_IMM R0, 2    ; Case selector (select case 2)
    
    ; Simple case selection using comparisons
    LOAD_IMM R5, 2
    CMP R0, R5        ; Compare with case 2
    JL case_default   ; Less than 2, go to default
    JG case_default   ; Greater than 2, go to default
    
    ; Exact match for case 2
    LOAD_IMM R1, 300
    JMP case_end
    
case_default:
    LOAD_IMM R1, 0
    
case_end:
    #assert_reg R1, 300  ; Should have selected case 2
    HALT
}