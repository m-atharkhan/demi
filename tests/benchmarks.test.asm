; Benchmark Test Suite
; Performance benchmarks for various operations

.test "benchmark_fibonacci" {
    .description "Benchmark: Fibonacci sequence calculation"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "fibonacci"
    .tag "recursive"
    .tag "performance"
    
    ; Calculate fib(8) = 21
    LOAD_IMM R0, 0    ; fib(0)
    LOAD_IMM R1, 1    ; fib(1)
    LOAD_IMM R2, 8    ; target
    LOAD_IMM R3, 1    ; counter
    
benchmark_fib_loop:
    CMP R3, R2
    JGE benchmark_fib_done
    MOV R4, R1
    ADD R1, R0
    MOV R0, R4
    INC R3
    JMP benchmark_fib_loop
    
benchmark_fib_done:
    .assert_reg R1, 21  ; fib(8) = 21
}

.test "benchmark_simple_arithmetic" {
    .description "Benchmark: Simple arithmetic operations"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "arithmetic"
    .tag "performance"
    
    ; Simple arithmetic benchmark
    LOAD_IMM R0, 17
    LOAD_IMM R1, 2
    
    ; Check if 17 is odd (17 % 2 = 1)
    MOV R2, R0
    MOD R2, R1
    
    .assert_reg R2, 1  ; 17 % 2 = 1
}

.test "benchmark_memory_ops" {
    .description "Benchmark: Memory operations"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "memory"
    .tag "performance"
    
    ; Simple memory operations
    LOAD_IMM R0, 72   ; 'H'
    STORE R0, 50
    LOAD_IMM R0, 69   ; 'E'
    STORE R0, 51
    
    ; Load back and verify
    LOAD R1, 50
    LOAD R2, 51
    
    .assert_reg R1, 72  ; 'H'
    .assert_reg R2, 69  ; 'E'
}

.test "benchmark_matrix_multiply_2x2" {
    .description "Benchmark: 2x2 matrix multiplication"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "matrix"
    .tag "multiplication"
    .tag "performance"
    
    ; Matrix A: [[1,2], [3,4]]
    LOAD_IMM R0, 1    ; A[0][0]
    LOAD_IMM R1, 2    ; A[0][1]
    LOAD_IMM R2, 3    ; A[1][0]
    LOAD_IMM R3, 4    ; A[1][1]
    
    ; Matrix B: [[5,6], [7,8]]
    LOAD_IMM R4, 5    ; B[0][0]
    LOAD_IMM R5, 6    ; B[0][1]
    LOAD_IMM R6, 7    ; B[1][0]
    LOAD_IMM R7, 8    ; B[1][1]
    
    ; Calculate C[0][0] = A[0][0]*B[0][0] + A[0][1]*B[1][0]
    ; Use memory to store intermediate results since we're limited to R0-R7
    MUL R0, R4        ; R0 = 1*5 = 5 (overwrites A[0][0])
    MUL R1, R6        ; R1 = 2*7 = 14 (overwrites A[0][1])
    ADD R0, R1        ; R0 = 5+14 = 19 (C[0][0])
    
    ; Store C[0][0] in memory and load fresh values for C[0][1]
    STORE R0, 100     ; Store C[0][0] = 19
    
    ; Reload A values for next calculation
    LOAD_IMM R0, 1    ; A[0][0] again
    LOAD_IMM R1, 2    ; A[0][1] again
    
    ; Calculate C[0][1] = A[0][0]*B[0][1] + A[0][1]*B[1][1]
    MUL R0, R5        ; R0 = 1*6 = 6
    MUL R1, R7        ; R1 = 2*8 = 16
    ADD R0, R1        ; R0 = 6+16 = 22 (C[0][1])
    
    ; Store C[0][1] and verify results
    STORE R0, 101     ; Store C[0][1] = 22
    LOAD R2, 100      ; Load C[0][0]
    LOAD R3, 101      ; Load C[0][1]
    
    .assert_reg R2, 19   ; C[0][0] = 19
    .assert_reg R3, 22   ; C[0][1] = 22
}
