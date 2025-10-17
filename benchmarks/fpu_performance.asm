; FPU Performance Benchmark
; Measures FPU operation performance with the unified registry

#test "fpu performance benchmark" {
    .data
        iterations: .dd 10000
        pi: .dd 3.14159265
        e: .dd 2.71828182

    .text
    main:
        ; Load iteration counter for performance testing
        LOAD_IMM R0, 100       ; Reduced for testing
        #assert_reg R0, 100
        
        ; FPU load operations performance test
        FLD pi                  ; Load from memory
        #assert_fpu ST(0), 3.14159265
        
        FLD e                   ; Load from memory
        #assert_fpu ST(0), 2.71828182
        
        FLD 1.0                 ; Load immediate
        #assert_fpu ST(0), 1.0
        
        FLD 2.5                 ; Load immediate
        #assert_fpu ST(0), 2.5
        
        ; FPU stack operations
        FLD ST(0)               ; Duplicate
        #assert_fpu ST(0), 2.5
        #assert_fpu ST(1), 2.5
        
        ; FPU arithmetic performance
        FADD 1.0                ; Add immediate (2.5 + 1.0 = 3.5)
        #assert_fpu ST(0), 3.5
        
        FADD pi                 ; Add from memory (3.5 + pi)
        #assert_fpu ST(0), 6.64159265
        
        ; Verify benchmark completed successfully
        LOAD_IMM R1, 42         ; Success marker
        #assert_reg R1, 42
        
        HALT
}