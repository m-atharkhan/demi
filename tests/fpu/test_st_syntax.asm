; Test ST() register syntax support
; Testing the new ST(i) parsing and encoding

.text
main:
    ; Load some values onto FPU stack
    FLD 3.14159         ; ST(0) = 3.14159
    FLD 2.5             ; ST(0) = 2.5, ST(1) = 3.14159
    
    ; Test ST(i) syntax
    FLD ST(0)           ; Duplicate ST(0) -> ST(0) = 2.5, ST(1) = 2.5, ST(2) = 3.14159
    FLD ST(1)           ; Duplicate ST(1) -> ST(0) = 2.5, ST(1) = 2.5, ST(2) = 2.5, ST(3) = 3.14159
    
    ; Test different case
    FLD st(0)           ; Should also work with lowercase
    
    HALT

; This tests:
; ✅ ST(0), ST(1), etc. parsing
; ✅ Case insensitive (ST vs st)
; ✅ Proper operand_type 0x03 encoding  
; ✅ Integration with existing sophisticated FLD implementation