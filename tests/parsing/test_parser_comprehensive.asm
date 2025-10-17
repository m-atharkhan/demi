; Comprehensive Assembly Parser Test
; Tests all newly implemented parsing features

#test "comprehensive parser features" {
    .data
        ; Test floating point in data section
        test_pi: .dd 3.14159265
        test_e: .dd 2.71828
        test_negative: .dd -42.5

    .text
    main:
        ; Test 1: Floating point literal parsing
        FLD 2.5                     ; Should parse 2.5 correctly
        #assert_fpu ST(0), 2.5
        
        FLD 3.14159265             ; Should parse high precision
        #assert_fpu ST(0), 3.14159265
        
        FLD 0.1                     ; Should parse decimal fractions
        #assert_fpu ST(0), 0.1
        
        FLD 1.0                     ; Should parse integer.0 format
        #assert_fpu ST(0), 1.0
        
        ; Test 2: ST() register syntax
        FLD 5.0
        FLD ST(0)                   ; Should parse ST(0) correctly - duplicate 5.0
        #assert_fpu ST(0), 5.0
        #assert_fpu ST(1), 5.0
        
        FLD st(0)                   ; Should parse lowercase st(0)
        #assert_fpu ST(0), 5.0
        
        ; Test 3: Memory references with symbols
        FLD test_pi                 ; Should load from data section
        #assert_fpu ST(0), 3.14159265
        
        FLD test_negative           ; Should resolve symbol addresses
        #assert_fpu ST(0), -42.5
        
        ; Test 4: Mixed operations
        FLD 1.5                     ; Float literal
        FADD 0.5                    ; Float arithmetic (1.5 + 0.5 = 2.0)
        #assert_fpu ST(0), 2.0
        
        ; Test 5: Edge cases
        FLD 0.0                     ; Zero
        #assert_fpu ST(0), 0.0
        
        FLD 123456.789              ; Large precision number
        #assert_fpu ST(0), 123456.789
        
        HALT
}