; SIMD-style operations using current instruction set
; These tests simulate what SIMD operations would do using regular instructions

#test "simulate_vector_addition" {
    #description "Simulate parallel vector addition using regular instructions"
    #category "SIMD"
    #tag "simulation"
    
    ; Vector A: [10, 20, 30, 40]
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    
    ; Vector B: [1, 2, 3, 4]
    LOAD_IMM R4, 1
    LOAD_IMM R5, 2
    LOAD_IMM R6, 3
    LOAD_IMM R7, 4
    
    ; Parallel addition simulation
    ADD R0, R4  ; 10 + 1 = 11
    ADD R1, R5  ; 20 + 2 = 22
    ADD R2, R6  ; 30 + 3 = 33
    ADD R3, R7  ; 40 + 4 = 44
    
    #assert_reg R0, 11
    #assert_reg R1, 22
    #assert_reg R2, 33
    #assert_reg R3, 44
    
    HALT
}

#test "simulate_vector_multiplication" {
    #description "Simulate parallel vector multiplication using regular instructions"  
    #category "SIMD"
    #tag "simulation"
    
    ; Vector A: [2, 3, 4, 5]
    LOAD_IMM R0, 2
    LOAD_IMM R1, 3
    LOAD_IMM R2, 4
    LOAD_IMM R3, 5
    
    ; Scalar: 10
    LOAD_IMM R4, 10
    
    ; Parallel scalar multiplication simulation
    MUL R0, R4  ; 2 * 10 = 20
    MUL R1, R4  ; 3 * 10 = 30
    MUL R2, R4  ; 4 * 10 = 40
    MUL R3, R4  ; 5 * 10 = 50
    
    #assert_reg R0, 20
    #assert_reg R1, 30
    #assert_reg R2, 40
    #assert_reg R3, 50
    
    HALT
}

#test "simulate_dot_product" {
    #description "Simulate vector dot product using regular instructions"
    #category "SIMD"
    #tag "simulation"
    
    ; Vector A: [1, 2, 3, 4] 
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    LOAD_IMM R3, 4
    
    ; Vector B: [5, 6, 7, 8]
    LOAD_IMM R4, 5
    LOAD_IMM R5, 6
    LOAD_IMM R6, 7
    LOAD_IMM R7, 8
    
    ; Element-wise multiplication and sum
    MUL R0, R4  ; 1*5 = 5
    MUL R1, R5  ; 2*6 = 12
    MUL R2, R6  ; 3*7 = 21
    MUL R3, R7  ; 4*8 = 32
    
    ; Sum all products: 5+12+21+32 = 70
    ADD R0, R1  ; 5+12 = 17
    ADD R0, R2  ; 17+21 = 38
    ADD R0, R3  ; 38+32 = 70
    
    #assert_reg R0, 70
    
    HALT
}