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
    LOAD_IMM EAX, 0    ; fib(0)
    LOAD_IMM EBX, 1    ; fib(1)
    LOAD_IMM ECX, 8    ; target
    LOAD_IMM EDX, 1    ; counter
    
benchmark_fib_loop:
    CMP EDX, ECX
    JGE benchmark_fib_done
    MOV ESI, EBX
    ADD EBX, EAX
    MOV EAX, ESI
    INC EDX
    JMP benchmark_fib_loop
    
benchmark_fib_done:
    .assert_reg EBX, 21  ; fib(8) = 21
}

.test "benchmark_simple_arithmetic" {
    .description "Benchmark: Simple arithmetic operations"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "arithmetic"
    .tag "performance"
    
    ; Simple arithmetic benchmark
    LOAD_IMM EAX, 17
    LOAD_IMM EBX, 2
    
    ; Check if 17 is odd (17 % 2 = 1)
    MOV ECX, EAX
    MOD ECX, EBX
    
    .assert_reg ECX, 1  ; 17 % 2 = 1
}

.test "benchmark_memory_ops" {
    .description "Benchmark: Memory operations"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "memory"
    .tag "performance"
    
    ; Simple memory operations
    LOAD_IMM EAX, 72   ; 'H'
    STORE EAX, 50
    LOAD_IMM EAX, 69   ; 'E'
    STORE EAX, 51
    
    ; Load back and verify
    LOAD EBX, 50
    LOAD ECX, 51
    
    .assert_reg EBX, 72  ; 'H'
    .assert_reg ECX, 69  ; 'E'
}

.test "benchmark_matrix_multiply_2x2" {
    .description "Benchmark: 2x2 matrix multiplication"
    .author "DemiEngine Team"
    .category "Benchmarks"
    .tag "matrix"
    .tag "multiplication"
    .tag "performance"
    
    ; Matrix A: [[1,2], [3,4]]
    LOAD_IMM EAX, 1    ; A[0][0]
    LOAD_IMM EBX, 2    ; A[0][1]
    LOAD_IMM ECX, 3    ; A[1][0]
    LOAD_IMM EDX, 4    ; A[1][1]
    
    ; Matrix B: [[5,6], [7,8]]
    LOAD_IMM ESI, 5    ; B[0][0]
    LOAD_IMM EDI, 6    ; B[0][1]
    LOAD_IMM ESP, 7    ; B[1][0]
    LOAD_IMM EBP, 8    ; B[1][1]
    
    ; Calculate C[0][0] = A[0][0]*B[0][0] + A[0][1]*B[1][0]
    ; Use memory to store intermediate results since we're limited to EAX-EBP
    MUL EAX, ESI        ; EAX = 1*5 = 5 (overwrites A[0][0])
    MUL EBX, ESP        ; EBX = 2*7 = 14 (overwrites A[0][1])
    ADD EAX, EBX        ; EAX = 5+14 = 19 (C[0][0])
    
    ; Store C[0][0] in memory and load fresh values for C[0][1]
    STORE EAX, 100     ; Store C[0][0] = 19
    
    ; Reload A values for next calculation
    LOAD_IMM EAX, 1    ; A[0][0] again
    LOAD_IMM EBX, 2    ; A[0][1] again
    
    ; Calculate C[0][1] = A[0][0]*B[0][1] + A[0][1]*B[1][1]
    MUL EAX, EDI        ; EAX = 1*6 = 6
    MUL EBX, EBP        ; EBX = 2*8 = 16
    ADD EAX, EBX        ; EAX = 6+16 = 22 (C[0][1])
    
    ; Store C[0][1] and verify results
    STORE EAX, 101     ; Store C[0][1] = 22
    LOAD ECX, 100      ; Load C[0][0]
    LOAD EDX, 101      ; Load C[0][1]
    
    .assert_reg ECX, 19   ; C[0][0] = 19
    .assert_reg EDX, 22   ; C[0][1] = 22
}
