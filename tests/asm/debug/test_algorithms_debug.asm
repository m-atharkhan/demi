; Exact copies of benchmark algorithms for debugging
#test "test_factorial_debug" {
    #description "Debug version of factorial - exact copy from benchmarks"
    #category "Debug"
    #tag "algorithms"
    
    ; Calculate 5! = 120 - exact same as benchmark
    LOAD_IMM R0, 5   ; N
    LOAD_IMM R1, 1   ; Result
    
factorial_loop:
    CMP R0, 0
    JGE factorial_done  ; Fixed: was JZ
    
    MUL R1, R0
    DEC R0
    JMP factorial_loop
    
factorial_done:
    #assert_reg R1, 120
    HALT
}

#test "test_fibonacci_debug" {  
    #description "Debug version of fibonacci - exact copy from benchmarks"
    #category "Debug"
    #tag "algorithms"
    
    ; Calculate F(10) = 55 - exact same as benchmark  
    LOAD_IMM R0, 0   ; F(0) = 0
    LOAD_IMM R1, 1   ; F(1) = 1
    LOAD_IMM R2, 2   ; Counter
    LOAD_IMM R3, 10  ; Target
    
fib_loop:
    CMP R2, R3
    JG fib_done
    
    ; R4 = R0 + R1
    MOV R4, R0
    ADD R4, R1
    
    ; Shift: R0 = R1, R1 = R4
    MOV R0, R1
    MOV R1, R4
    
    INC R2
    JMP fib_loop
    
fib_done:
    #assert_reg R1, 55
    HALT
}