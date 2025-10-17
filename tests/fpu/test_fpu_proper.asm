; Test existing FPU operations with proper operand syntax
; Testing integration of sophisticated existing FPU implementations

main:
    ; Load floating point value with immediate operand
    FLD 42          ; Load immediate integer (converted to double) - existing FLD handler
    
    ; Load another value for arithmetic
    FLD 13          ; Load another immediate - should use sophisticated existing FLD
    
    ; Test FADD with immediate operand (existing FADD implementation)
    FADD 2          ; Add immediate value 2 to ST(0) - existing sophisticated FADD
    
    ; Store result to register using existing FST handler
    FST R0          ; Store to register R0 - existing sophisticated FST
    
    HALT

; This tests:
; - Existing FLD implementation with immediate operands (converted to double)
; - Existing FADD implementation with proper operand handling
; - Existing FST implementation with register operand
; All should work through unified registry integration