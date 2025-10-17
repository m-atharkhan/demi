; Final test showing all originally problematic features now working
; This addresses all the lexer errors from the original file

.data
    pi_value: .dd 3.14159265    ; ✅ Floating point with .dd directive
    test_val: .dd 2.5           ; ✅ Another floating point value

.text
main:
    ; ✅ Floating point literals now work
    FLD 2.5                     ; Load 2.5 (was "Unknown directive: .5")
    FLD 3.14159265              ; Load pi approximation (was "Unknown directive: .14159265") 
    
    ; ✅ Basic FPU arithmetic with floating point
    FADD 1.5                    ; Add 1.5
    
    ; ✅ Memory references work
    FLD pi_value                ; Load from memory symbol
    
    HALT

; 🎉 SUCCESS SUMMARY:
; ✅ .data/.text sections implemented and working
; ✅ .dd directive supports floating point values  
; ✅ Floating point literals (2.5, 3.14159265, 1.5) parse correctly
; ✅ No more "Unknown directive" errors for decimal points
; ✅ Memory addressing with symbols works
; ✅ Integration with existing sophisticated FPU operations
; ✅ Parentheses support added to lexer (ready for future memory addressing)
; ✅ dword keyword support added to lexer (ready for use)

; Note: The original () syntax for ST(0) register references would need
; additional parser work for FPU register addressing, but the lexer
; now supports parentheses tokens for future implementation.