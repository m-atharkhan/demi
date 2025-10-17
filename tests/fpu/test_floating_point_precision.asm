; Floating Point Parsing and Precision Test
; Tests floating point literal parsing, precision, and .dd directive

#test "floating point precision and parsing" {
    .data
        pi: .dd 3.14159265358979323846
        e: .dd 2.71828182845904523536
        small_float: .dd 0.000001
        large_float: .dd 1234567.89
        negative_float: .dd -42.5

    .text
    main:
        ; Test various floating point literal formats
        FLD 0.0                     ; Zero
        #assert_fpu ST(0), 0.0
        
        FLD 1.0                     ; Integer as float
        #assert_fpu ST(0), 1.0
        
        FLD 3.14159                 ; Pi approximation
        #assert_fpu ST(0), 3.14159
        
        FLD 2.71828                 ; E approximation
        #assert_fpu ST(0), 2.71828
        
        FLD 0.5                     ; Simple fraction
        #assert_fpu ST(0), 0.5
        
        FLD 123.456789              ; Multiple decimal places
        #assert_fpu ST(0), 123.456789
        
        ; Test precision handling
        FLD 3.1415926535897932      ; High precision
        #assert_fpu ST(0), 3.1415926535897932
        
        ; Test edge cases
        FLD 0.1                     ; Fraction that's hard to represent in binary
        #assert_fpu ST(0), 0.1
        
        ; Load from memory to test .dd directive
        FLD pi                      ; Should load high precision pi
        #assert_fpu ST(0), 3.14159265358979323846
        
        FLD negative_float          ; Test negative numbers
        #assert_fpu ST(0), -42.5
        
        ; Test arithmetic with various precisions
        FLD 0.1
        FADD 0.2                    ; 0.1 + 0.2 = 0.3
        #assert_fpu ST(0), 0.3
        
        HALT
}