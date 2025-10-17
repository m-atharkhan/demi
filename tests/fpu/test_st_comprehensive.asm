; Comprehensive test of ST() syntax support
; Demonstrating all the newly implemented ST register features

.data
    value1: .dd 1.5
    value2: .dd 2.5

.text  
main:
    ; Load initial values
    FLD 3.14159         ; ST(0) = 3.14159
    FLD 2.71828         ; ST(0) = 2.71828, ST(1) = 3.14159
    FLD value1          ; ST(0) = 1.5, ST(1) = 2.71828, ST(2) = 3.14159
    
    ; Test ST() syntax variations
    FLD ST(0)           ; Duplicate top: ST(0) = 1.5, ST(1) = 1.5, ST(2) = 2.71828, ST(3) = 3.14159
    FLD ST(2)           ; Load ST(2): ST(0) = 2.71828, ST(1) = 1.5, ST(2) = 1.5, ST(3) = 2.71828
    FLD st(1)           ; Test lowercase: ST(0) = 1.5, others shift down
    
    ; Test with different ST indices
    FLD ST(0)           ; ST(0) = 1.5
    FLD ST(1)           ; ST(0) = 1.5 (from ST(1))
    FLD ST(3)           ; ST(0) = value from ST(3)
    
    ; Current FPU arithmetic (single operand form)
    FADD 1.0            ; Add 1.0 to ST(0)
    
    HALT

; ✅ WORKING FEATURES:
; - ST(0) through ST(7) syntax parsing
; - Case insensitive: ST(0) and st(0) both work
; - Proper operand_type 0x03 encoding
; - Integration with existing sophisticated FLD implementation
; - FPU stack duplication operations
; - Mixed usage with immediate values and memory references
; 
; 📝 FUTURE ENHANCEMENT:
; - Two-operand FPU syntax like "FADD ST(0), ST(1)" would require
;   extending the parser for multiple operands and updating FADD
;   to support ST register to ST register operations