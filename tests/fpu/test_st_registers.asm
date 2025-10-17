; Comprehensive FPU Stack Register Test
; Tests all ST() register operations and syntax variations

#test "st register syntax and operations" {
    .data
        test_val1: .dd 3.14159265
        test_val2: .dd 2.71828182
        result_storage: .dd 0.0

    .text
    main:
        ; Basic FPU stack operations
        FLD 1.0                 ; ST(0) = 1.0
        #assert_fpu ST(0), 1.0
        
        FLD 2.0                 ; ST(0) = 2.0, ST(1) = 1.0
        #assert_fpu ST(0), 2.0
        
        FLD 3.0                 ; ST(0) = 3.0, ST(1) = 2.0, ST(2) = 1.0
        #assert_fpu ST(0), 3.0
        
        ; Test ST() duplication operations
        FLD ST(0)               ; Duplicate top: ST(0) = 3.0, ST(1) = 3.0
        #assert_fpu ST(0), 3.0
        #assert_fpu ST(1), 3.0
        
        ; Test case variations
        FLD st(0)               ; Lowercase should work
        FLD St(1)               ; Mixed case should work
        FLD ST(0)               ; Standard case
        
        ; Test with memory operations
        FLD test_val1           ; Load from memory
        #assert_fpu ST(0), 3.14159265
        
        ; Test arithmetic with ST registers loaded
        FADD 1.0                ; Add to current ST(0)
        #assert_fpu ST(0), 4.14159265
        
        HALT
}