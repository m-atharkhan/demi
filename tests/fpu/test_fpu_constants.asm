; Test existing FPU operations using constants that are supported
; Testing integration of sophisticated existing FPU implementations

main:
    ; Load FPU constants (no operands needed)
    FLDZ        ; Load 0.0 onto FPU stack (opcode should be in registry)
    FLD1        ; Load 1.0 onto FPU stack  
    FLDPI       ; Load π (pi) onto FPU stack
    
    ; Now FPU stack has: ST(0)=π, ST(1)=1.0, ST(2)=0.0
    
    ; Test FADD - should use existing sophisticated implementation
    FADD        ; Add ST(1) to ST(0): π + 1.0, pop ST(1)
    
    ; Test FST - store ST(0) to register/memory
    FST         ; Store result using existing FST handler
    
    HALT

; This tests the existing sophisticated FPU implementations:
; - FLDZ, FLD1, FLDPI should use existing constant loading logic
; - FADD should use existing IEEE 754 arithmetic with proper stack handling  
; - FST should use existing store operation with multiple operand support
; All should work through unified registry (opcodes 0xAC, 0xAD, 0xAE, 0xA6, 0xA1)