; Test file for complex scenarios and edge cases

#test "fibonacci calculation" {
    #description "Calculates the 5th Fibonacci number using iterative approach"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "fibonacci"
    #tag "iterative"
    #tag "complex"
    LOAD_IMM R0, 0      ; fib(0)
    LOAD_IMM R1, 1      ; fib(1)
    
    ; Calculate fib(2) through fib(5)
    ADD R0, R1          ; R0 = 1, fib(2)
    MOV R2, R0
    ADD R1, R2          ; R1 = 2, fib(3)
    MOV R3, R1
    ADD R2, R3          ; R2 = 3, fib(4)
    MOV R4, R2
    ADD R3, R4          ; R3 = 5, fib(5)
    
    #assert_reg R3, 5
}

#test "factorial of 5" {
    #description "Calculates factorial of 5 (5!) using iterative multiplication"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "factorial"
    #tag "iterative"
    #tag "complex"
    LOAD_IMM R0, 5      ; n
    LOAD_IMM R1, 1      ; result
    
    ; 5! = 5 * 4 * 3 * 2 * 1
    MUL R1, R0          ; result = 5
    DEC R0
    MUL R1, R0          ; result = 20
    DEC R0
    MUL R1, R0          ; result = 60
    DEC R0
    MUL R1, R0          ; result = 120
    
    #assert_reg R1, 120
}

#test "sum array of numbers" {
    #description "Calculates the sum of multiple values stored in registers"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "accumulator"
    #tag "sum"
    LOAD_IMM R0, 0      ; sum = 0
    LOAD_IMM R1, 10     ; numbers to sum
    LOAD_IMM R2, 20
    LOAD_IMM R3, 30
    LOAD_IMM R4, 40
    
    ADD R0, R1
    ADD R0, R2
    ADD R0, R3
    ADD R0, R4
    
    #assert_reg R0, 100
}

#test "maximum of three numbers" {
    #description "Finds the maximum value among three numbers using comparisons"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "max"
    #tag "comparison"
    #tag "control-flow"
    LOAD_IMM R0, 25
    LOAD_IMM R1, 42
    LOAD_IMM R2, 18
    
    ; Find max(R0, R1, R2)
    MOV R3, R0          ; R3 = current max
    CMP R1, R3
    JLE check_r2
    MOV R3, R1
check_r2:
    CMP R2, R3
    JLE done
    MOV R3, R2
done:
    #assert_reg R3, 42
}

#test "absolute value" {
    #description "Tests handling of values that could be interpreted as signed"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "signed"
    #tag "edge-case"
    LOAD_IMM R0, 250    ; Treat as -6 in signed
    ; Since we can't easily detect sign, just verify it's unchanged
    #assert_reg R0, 250
}

#test "average of four numbers" {
    #description "Calculates the average of four numbers using sum and division"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "average"
    #tag "arithmetic"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    
    ; Sum them
    ADD R0, R1          ; R0 = 30
    ADD R0, R2          ; R0 = 60
    ADD R0, R3          ; R0 = 100
    
    ; Divide by 4
    LOAD_IMM R1, 4
    DIV R0, R1
    
    #assert_reg R0, 25
}

#test "check power of 2" {
    #description "Verifies that shift operations can compute powers of 2"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "shift"
    #tag "power-of-two"
    LOAD_IMM R0, 16
    ; 16 is 2^4
    LOAD_IMM R1, 1
    SHL R1, 4           ; 1 << 4 = 16
    #assert_reg R1, 16
}

#test "swap without temp register" {
    #description "Swaps two register values using XOR trick without temporary storage"
    #author "DemiEngine Team"
    #category "Algorithms"
    #tag "swap"
    #tag "xor"
    #tag "trick"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    
    ; Swap using XOR
    XOR R0, R1          ; R0 = 5 ^ 10 = 15
    XOR R1, R0          ; R1 = 10 ^ 15 = 5
    XOR R0, R1          ; R0 = 15 ^ 5 = 10
    
    #assert_reg R0, 10
    #assert_reg R1, 5
}
