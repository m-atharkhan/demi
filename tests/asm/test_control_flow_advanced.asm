; Advanced Control Flow Tests
; Tests complex jump patterns, nested calls, and recursion

#test "nested_conditional_jumps" {
    #description "Test nested conditional jump logic"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "nested"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 5
    
    CMP R0, R1
    JZ equal_branch    ; Not taken
    
    ; Greater than path
    LOAD_IMM R2, 1
    JMP end_test
    
equal_branch:
    LOAD_IMM R2, 2
    
end_test:
    #assert_reg R2, 1
    HALT
}

#test "jump_table_simulation" {
    #description "Test simulating a jump table"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump-table"
    #tag "switch"
    
    LOAD_IMM R0, 2    ; Case selector
    
    CMP R0, 0
    JZ case_0
    
    CMP R0, 1
    JZ case_1
    
    CMP R0, 2
    JZ case_2
    
    JMP default_case
    
case_0:
    LOAD_IMM R1, 100
    JMP end_switch
    
case_1:
    LOAD_IMM R1, 200
    JMP end_switch
    
case_2:
    LOAD_IMM R1, 300
    JMP end_switch
    
default_case:
    LOAD_IMM R1, 0
    
end_switch:
    #assert_reg R1, 300
    HALT
}

#test "loop_with_counter" {
    #description "Test loop with iteration counter"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "loop"
    #tag "counter"
    
    LOAD_IMM R0, 0     ; Counter
    LOAD_IMM R1, 5     ; Limit
    LOAD_IMM R2, 0     ; Accumulator
    
loop_start:
    CMP R0, R1
    JZ loop_end
    
    ; Loop body: add counter to accumulator
    ADD R2, R0
    
    ; Increment counter
    INC R0
    JMP loop_start
    
loop_end:
    ; R2 should be 0+1+2+3+4 = 10
    #assert_reg R2, 10
    #assert_reg R0, 5
    HALT
}

#test "nested_loops" {
    #description "Test nested loop structure"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "nested"
    #tag "loop"
    
    LOAD_IMM R0, 0     ; Outer counter
    LOAD_IMM R1, 3     ; Outer limit
    LOAD_IMM R2, 0     ; Total iterations
    
outer_loop:
    CMP R0, R1
    JZ outer_end
    
    LOAD_IMM R3, 0     ; Inner counter
    LOAD_IMM R4, 2     ; Inner limit
    
inner_loop:
    CMP R3, R4
    JZ inner_end
    
    INC R2             ; Count iteration
    INC R3
    JMP inner_loop
    
inner_end:
    INC R0
    JMP outer_loop
    
outer_end:
    ; Should have 3*2 = 6 total iterations
    #assert_reg R2, 6
    HALT
}

#test "early_exit_loop" {
    #description "Test breaking out of loop early"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "loop"
    #tag "break"
    
    LOAD_IMM R0, 0     ; Counter
    LOAD_IMM R1, 100   ; Large limit
    LOAD_IMM R2, 7     ; Early exit condition
    
search_loop:
    CMP R0, R1
    JZ not_found
    
    CMP R0, R2
    JZ found
    
    INC R0
    JMP search_loop
    
found:
    LOAD_IMM R3, 1
    JMP search_end
    
not_found:
    LOAD_IMM R3, 0
    
search_end:
    #assert_reg R0, 7
    #assert_reg R3, 1
    HALT
}

#test "conditional_chain" {
    #description "Test chain of conditional statements"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "conditional"
    #tag "chain"
    
    LOAD_IMM R0, 15
    LOAD_IMM R1, 0     ; Result
    
    ; Check if R0 < 10
    LOAD_IMM R2, 10
    CMP R0, R2
    ; JS less_than_10   ; TODO: Need signed comparison
    
    ; Check if R0 < 20
    LOAD_IMM R2, 20
    CMP R0, R2
    ; JS less_than_20   ; TODO: Need signed comparison
    
    ; Check if R0 < 30
    LOAD_IMM R2, 30
    CMP R0, R2
    ; JS less_than_30   ; TODO: Need signed comparison
    
    JMP range_end
    
less_than_10:
    LOAD_IMM R1, 1
    JMP range_end
    
less_than_20:
    LOAD_IMM R1, 2
    JMP range_end
    
less_than_30:
    LOAD_IMM R1, 3
    
range_end:
    ; R0 is 15, should be in 10-20 range
    ; #assert_reg R1, 2
    HALT
}

#test "backward_forward_jumps" {
    #description "Test mixing backward and forward jumps"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "mixed"
    
    LOAD_IMM R0, 0
    JMP forward_1
    
back_target:
    LOAD_IMM R0, 5
    JMP forward_2
    
forward_1:
    LOAD_IMM R0, 3
    JMP back_target
    
forward_2:
    #assert_reg R0, 5
    HALT
}

#test "jump_over_code" {
    #description "Test jumping over code blocks"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "skip"
    
    LOAD_IMM R0, 100
    JMP skip_bad_code
    
    ; This code should be skipped
    LOAD_IMM R0, 0
    LOAD_IMM R0, 0
    LOAD_IMM R0, 0
    
skip_bad_code:
    #assert_reg R0, 100
    HALT
}
