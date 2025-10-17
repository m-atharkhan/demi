; Test FPU operations integration with unified registry
; Testing existing sophisticated FPU implementations

main:
    ; Test FLD operation (opcode 0xA0)
    ; Using simple immediate value first
    FLD 3.14159
    
    ; Test FLD with another value to build FPU stack
    FLD 2.5
    
    ; Test FADD operation (should be opcode 0xA1)
    FADD
    
    ; Test FST operation to store result
    FST 
    
    HALT

; Expected: FPU stack operations using existing sophisticated implementations
; FLD should push values onto FPU stack using existing fpu_push() methods
; FADD should perform IEEE 754 addition using existing logic
; Result should demonstrate proper integration of existing codebase