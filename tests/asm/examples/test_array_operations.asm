; Array summation with overflow handling
#test "test_array_sum_overflow" {
    #description "Test array summation with 8-bit register overflow"
    #category "Debug"
    #tag "arrays"
    #tag "overflow"
    
    ; Array: [10, 20, 30, 40, 50] - sum = 150 (fits in 8-bit)
    LOAD_IMM R0, 10
    STORE R0, 0x70
    LOAD_IMM R0, 20
    STORE R0, 0x71
    LOAD_IMM R0, 30
    STORE R0, 0x72
    LOAD_IMM R0, 40
    STORE R0, 0x73
    LOAD_IMM R0, 50
    STORE R0, 0x74
    
    ; Sum array elements
    LOAD_IMM R1, 0      ; Sum accumulator
    LOAD_IMM R2, 0x70   ; Current address
    LOAD_IMM R3, 0x75   ; End address (exclusive)
    
sum_loop:
    CMP R2, R3
    JGE sum_done
    
    LOAD R4, R2         ; Load array element
    ADD R1, R4          ; Add to sum
    INC R2              ; Next address
    JMP sum_loop
    
sum_done:
    #assert_reg R1, 150  ; 10+20+30+40+50 = 150
    HALT
}