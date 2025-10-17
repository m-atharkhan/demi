; Section and Directive Test  
; Tests .data/.text sections and various data directives

#test "sections and data directives" {
    .data
        ; Test .dd directive with different value types
        int_value: .dd 42
        float_value: .dd 3.14159
        zero_value: .dd 0
        negative_value: .dd -123
        
        ; Test multiple values in one directive
        array_values: .dd 1, 2, 3, 4, 5
        mixed_values: .dd 10, 3.14, -5, 0.5
        
        ; Test other data directives
        byte_val: .db 255
        word_val: .dw 65535
        string_val: .string "Hello World"

    .text
    start:
        ; Test loading from different data types
        FLD int_value           ; Load integer as float
        #assert_fpu ST(0), 42
        
        FLD float_value         ; Load float
        #assert_fpu ST(0), 3.14159
        
        FLD zero_value          ; Load zero
        #assert_fpu ST(0), 0
        
        FLD negative_value      ; Load negative
        #assert_fpu ST(0), -123
        
        ; Test array access (first element)
        FLD array_values        ; Should load first value (1)
        #assert_fpu ST(0), 1
        
        FLD mixed_values        ; Should load first value (10)
        #assert_fpu ST(0), 10
        
        ; Test arithmetic with loaded values
        FADD float_value        ; Add pi to current value (10 + 3.14159)
        #assert_fpu ST(0), 13.14159
        
        HALT
}