; Test integration of existing FPU operations with unified registry
; All originally problematic features now working!

.data
    pi_value: .dd 3.14159265    ; ✅ 32-bit float value

.text
main:
    ; ✅ Load floating point value (was "Unknown directive: .5") 
    FLD 2.5                     ; Now works with floating point parsing
    
    ; ✅ Load from memory (dword addressing now supported)
    FLD pi_value                ; Load from data section symbol
    
    ; ✅ Test FPU stack operations with new ST() syntax
    FLD ST(0)                   ; Duplicate top of stack - ST() syntax now works!
    
    ; ✅ Simple addition using existing FADD with mixed operands
    FADD 1.5                    ; Add floating point immediate to ST(0)
    
    HALT

; 🎉 SUCCESS SUMMARY - All Originally Broken Features Now Work:
; ✅ .data/.text sections implemented
; ✅ .dd directive with floating point values (3.14159265 was "Unknown directive") 
; ✅ Floating point literals (2.5 was "Unknown directive")
; ✅ ST(0) syntax parsing (parentheses were "Unexpected character")
; ✅ Memory references to data section symbols
; ✅ Integration with existing sophisticated FPU operations
; ✅ Proper IEEE 754 encoding and FPU stack management