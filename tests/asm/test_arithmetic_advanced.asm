; Advanced Arithmetic Tests
; Tests edge cases, overflow, underflow, and complex operations

#test "overflow_detection" {
    #description "Test overflow flag on arithmetic operations"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "overflow"
    #tag "flags"
    
    ; Test addition overflow (127 + 1 = 128, but in signed 8-bit this overflows)
    LOAD_IMM R0, 127
    LOAD_IMM R1, 1
    ADD R0, R1
    ; Should set overflow flag
    #assert_reg R0, 128
    
    ; Test multiplication overflow
    LOAD_IMM R2, 200
    LOAD_IMM R3, 3
    MUL R2, R3
    ; 200 * 3 = 600, should set carry flag
    
    HALT
}

#test "underflow_detection" {
    #description "Test underflow in subtraction"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "underflow"
    #tag "flags"
    
    ; Test subtraction underflow (0 - 1)
    LOAD_IMM R0, 0
    LOAD_IMM R1, 1
    SUB R0, R1
    ; Should set carry flag and result wraps to 255
    #assert_reg R0, 255
    
    ; Test signed underflow
    LOAD_IMM R2, 128  ; -128 in signed 8-bit
    LOAD_IMM R3, 1
    SUB R2, R3
    ; Should set overflow flag
    #assert_reg R2, 127
    
    HALT
}

#test "division_edge_cases" {
    #description "Test division edge cases"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "division"
    #tag "edge-cases"
    
    ; Division by 1
    LOAD_IMM R0, 100
    LOAD_IMM R1, 1
    DIV R0, R1
    #assert_reg R0, 100
    
    ; Division resulting in 0
    LOAD_IMM R2, 5
    LOAD_IMM R3, 10
    DIV R2, R3
    #assert_reg R2, 0
    
    ; Division with remainder
    LOAD_IMM R4, 17
    LOAD_IMM R5, 5
    DIV R4, R5
    #assert_reg R4, 3
    
    HALT
}

#test "modulo_edge_cases" {
    #description "Test modulo operation edge cases"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "modulo"
    #tag "edge-cases"
    
    ; Modulo with result 0
    LOAD_IMM R0, 10
    LOAD_IMM R1, 5
    MOD R0, R1
    #assert_reg R0, 0
    
    ; Modulo by 1 (always 0)
    LOAD_IMM R2, 99
    LOAD_IMM R3, 1
    MOD R2, R3
    #assert_reg R2, 0
    
    ; Modulo where dividend < divisor
    LOAD_IMM R4, 3
    LOAD_IMM R5, 7
    MOD R4, R5
    #assert_reg R4, 3
    
    ; Modulo by power of 2
    LOAD_IMM R6, 23
    LOAD_IMM R7, 8
    MOD R6, R7
    #assert_reg R6, 7
    
    HALT
}

#test "chained_arithmetic" {
    #description "Test complex chained arithmetic operations"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "complex"
    #tag "chain"
    
    ; (10 + 5) * 2 - 3 = 27
    LOAD_IMM R0, 10
    LOAD_IMM R1, 5
    ADD R0, R1      ; R0 = 15
    LOAD_IMM R2, 2
    MUL R0, R2      ; R0 = 30
    LOAD_IMM R3, 3
    SUB R0, R3      ; R0 = 27
    #assert_reg R0, 27
    
    HALT
}

#test "bitwise_arithmetic_combo" {
    #description "Test combination of bitwise and arithmetic operations"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "bitwise"
    #tag "complex"
    
    ; (15 & 7) + (8 | 4) = 7 + 12 = 19
    LOAD_IMM R0, 15
    LOAD_IMM R1, 7
    AND R0, R1      ; R0 = 7
    
    LOAD_IMM R2, 8
    LOAD_IMM R3, 4
    OR R2, R3       ; R2 = 12
    
    ADD R0, R2      ; R0 = 19
    #assert_reg R0, 19
    
    HALT
}

#test "signed_unsigned_conversion" {
    #description "Test handling of signed vs unsigned values"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "signed"
    #tag "unsigned"
    
    ; Load what would be -1 in signed 8-bit
    LOAD_IMM R0, 255
    LOAD_IMM R1, 1
    ADD R0, R1
    ; Should wrap to 0
    #assert_reg R0, 0
    
    ; Test comparison with signed interpretation
    LOAD_IMM R2, 255  ; -1 signed
    LOAD_IMM R3, 1
    CMP R2, R3
    ; R2 > R3 in unsigned, R2 < R3 in signed
    
    HALT
}

#test "multiply_by_zero" {
    #description "Test multiplication by zero"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "multiplication"
    #tag "edge-cases"
    
    LOAD_IMM R0, 99
    LOAD_IMM R1, 0
    MUL R0, R1
    #assert_reg R0, 0
    
    LOAD_IMM R2, 0
    LOAD_IMM R3, 99
    MUL R2, R3
    #assert_reg R2, 0
    
    HALT
}
