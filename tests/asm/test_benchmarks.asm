; Performance Benchmark Tests
; Tests for measuring CPU performance with various algorithms

#test "benchmark_bubble_sort" {
    #description "Benchmark: Bubble sort algorithm"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "sorting"
    #tag "algorithm"
    #tag "performance"
    
    ; Array in memory: [64, 34, 25, 12, 22, 11, 90] at 0x50-0x56
    LOAD_IMM R0, 64
    STORE R0, 0x50
    LOAD_IMM R0, 34
    STORE R0, 0x51
    LOAD_IMM R0, 25
    STORE R0, 0x52
    LOAD_IMM R0, 12
    STORE R0, 0x53
    LOAD_IMM R0, 22
    STORE R0, 0x54
    LOAD_IMM R0, 11
    STORE R0, 0x55
    LOAD_IMM R0, 90
    STORE R0, 0x56
    
    ; TODO: Implement bubble sort
    ; For now, verify array was stored
    LOAD R1, 0x50
    #assert_reg R1, 64
    
    LOAD R2, 0x56
    #assert_reg R2, 90
    
    HALT
}

#test "benchmark_fibonacci" {
    #description "Benchmark: Fibonacci sequence calculation"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "fibonacci"
    #tag "recursive"
    #tag "performance"
    
    ; Calculate 10th Fibonacci number
    LOAD_IMM R0, 0  ; F(0) = 0
    LOAD_IMM R1, 1  ; F(1) = 1
    LOAD_IMM R2, 2  ; Counter
    LOAD_IMM R3, 10 ; Target
    
fib_loop:
    CMP R2, R3
    JZ fib_done
    
    ; R4 = R0 + R1
    MOV R4, R0
    ADD R4, R1
    
    ; Shift: R0 = R1, R1 = R4
    MOV R0, R1
    MOV R1, R4
    
    INC R2
    JMP fib_loop
    
fib_done:
    ; F(10) = 55
    #assert_reg R1, 55
    HALT
}

#test "benchmark_prime_check" {
    #description "Benchmark: Prime number checking"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "prime"
    #tag "algorithm"
    #tag "performance"
    
    LOAD_IMM R0, 17  ; Number to check
    LOAD_IMM R1, 2   ; Divisor
    LOAD_IMM R2, 1   ; Is prime flag (assume true)
    
prime_loop:
    ; Check if divisor^2 > number
    MOV R3, R1
    MUL R3, R1
    CMP R3, R0
    ; JG prime_done  ; TODO: Need greater-than
    
    ; Check if number % divisor == 0
    MOV R4, R0
    MOD R4, R1
    CMP R4, 0
    JZ not_prime
    
    INC R1
    JMP prime_loop
    
not_prime:
    LOAD_IMM R2, 0
    JMP prime_end
    
prime_done:
    ; R2 should still be 1
    
prime_end:
    #assert_reg R2, 1  ; 17 is prime
    HALT
}

#test "benchmark_sum_array" {
    #description "Benchmark: Sum elements of an array"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "array"
    #tag "sum"
    #tag "performance"
    
    ; Array: [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
    ; Store at 0x60-0x69
    LOAD_IMM R0, 10
    STORE R0, 0x60
    LOAD_IMM R0, 20
    STORE R0, 0x61
    LOAD_IMM R0, 30
    STORE R0, 0x62
    LOAD_IMM R0, 40
    STORE R0, 0x63
    LOAD_IMM R0, 50
    STORE R0, 0x64
    LOAD_IMM R0, 60
    STORE R0, 0x65
    LOAD_IMM R0, 70
    STORE R0, 0x66
    LOAD_IMM R0, 80
    STORE R0, 0x67
    LOAD_IMM R0, 90
    STORE R0, 0x68
    LOAD_IMM R0, 100
    STORE R0, 0x69
    
    ; Sum array
    LOAD_IMM R1, 0   ; Sum
    LOAD_IMM R2, 0x60  ; Address
    LOAD_IMM R3, 0x6A  ; End address
    
sum_loop:
    CMP R2, R3
    JZ sum_done
    
    LOAD R4, R2
    ADD R1, R4
    
    INC R2
    JMP sum_loop
    
sum_done:
    ; Sum should be 10+20+...+100 = 550
    ; But we're using 8-bit registers, so 550 % 256 = 38
    #assert_reg R1, 38
    HALT
}

#test "benchmark_matrix_multiply_2x2" {
    #description "Benchmark: 2x2 matrix multiplication"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "matrix"
    #tag "multiplication"
    #tag "performance"
    
    ; Matrix A: [[2, 3], [4, 5]] at 0x70-0x73
    LOAD_IMM R0, 2
    STORE R0, 0x70
    LOAD_IMM R0, 3
    STORE R0, 0x71
    LOAD_IMM R0, 4
    STORE R0, 0x72
    LOAD_IMM R0, 5
    STORE R0, 0x73
    
    ; Matrix B: [[1, 0], [0, 1]] (identity) at 0x74-0x77
    LOAD_IMM R0, 1
    STORE R0, 0x74
    LOAD_IMM R0, 0
    STORE R0, 0x75
    LOAD_IMM R0, 0
    STORE R0, 0x76
    LOAD_IMM R0, 1
    STORE R0, 0x77
    
    ; Result should be A * I = A
    ; Result at 0x78-0x7B
    
    ; C[0][0] = A[0][0]*B[0][0] + A[0][1]*B[1][0]
    LOAD R0, 0x70  ; A[0][0]
    LOAD R1, 0x74  ; B[0][0]
    MUL R0, R1
    LOAD R2, 0x71  ; A[0][1]
    LOAD R3, 0x76  ; B[1][0]
    MUL R2, R3
    ADD R0, R2
    STORE R0, 0x78
    
    ; Verify first element
    LOAD R4, 0x78
    #assert_reg R4, 2
    
    HALT
}

#test "benchmark_string_length" {
    #description "Benchmark: Calculate string length"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "string"
    #tag "length"
    #tag "performance"
    
    ; String "HELLO" at 0x80-0x85 (null-terminated)
    LOAD_IMM R0, 72  ; 'H'
    STORE R0, 0x80
    LOAD_IMM R0, 69  ; 'E'
    STORE R0, 0x81
    LOAD_IMM R0, 76  ; 'L'
    STORE R0, 0x82
    LOAD_IMM R0, 76  ; 'L'
    STORE R0, 0x83
    LOAD_IMM R0, 79  ; 'O'
    STORE R0, 0x84
    LOAD_IMM R0, 0   ; null terminator
    STORE R0, 0x85
    
    ; Calculate length
    LOAD_IMM R1, 0   ; Length
    LOAD_IMM R2, 0x80  ; Address
    
strlen_loop:
    LOAD R3, R2
    CMP R3, 0
    JZ strlen_done
    
    INC R1
    INC R2
    JMP strlen_loop
    
strlen_done:
    #assert_reg R1, 5
    HALT
}

#test "benchmark_binary_search" {
    #description "Benchmark: Binary search in sorted array"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "search"
    #tag "binary"
    #tag "performance"
    
    ; Sorted array: [10, 20, 30, 40, 50, 60, 70, 80] at 0x90-0x97
    LOAD_IMM R0, 10
    STORE R0, 0x90
    LOAD_IMM R0, 20
    STORE R0, 0x91
    LOAD_IMM R0, 30
    STORE R0, 0x92
    LOAD_IMM R0, 40
    STORE R0, 0x93
    LOAD_IMM R0, 50
    STORE R0, 0x94
    LOAD_IMM R0, 60
    STORE R0, 0x95
    LOAD_IMM R0, 70
    STORE R0, 0x96
    LOAD_IMM R0, 80
    STORE R0, 0x97
    
    ; Search for value 50
    LOAD_IMM R0, 50   ; Target
    LOAD_IMM R1, 0x90 ; Start
    LOAD_IMM R2, 0x97 ; End
    LOAD_IMM R3, 0    ; Result index
    
    ; Binary search would go here
    ; For now, linear search
bsearch_loop:
    CMP R1, R2
    JZ bsearch_not_found
    
    LOAD R4, R1
    CMP R4, R0
    JZ bsearch_found
    
    INC R1
    JMP bsearch_loop
    
bsearch_found:
    MOV R3, R1
    
bsearch_not_found:
    ; Should find at index 0x94
    #assert_reg R3, 0x94
    HALT
}

#test "benchmark_factorial" {
    #description "Benchmark: Factorial calculation"
    #author "DemiEngine Team"
    #category "Benchmarks"
    #tag "factorial"
    #tag "recursive"
    #tag "performance"
    
    ; Calculate 5! = 120
    LOAD_IMM R0, 5   ; N
    LOAD_IMM R1, 1   ; Result
    
factorial_loop:
    CMP R0, 0
    JZ factorial_done
    
    MUL R1, R0
    DEC R0
    JMP factorial_loop
    
factorial_done:
    #assert_reg R1, 120
    HALT
}
