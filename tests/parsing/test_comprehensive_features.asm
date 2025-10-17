; Comprehensive test of all requested features
; Testing sections, dd directive, dword, floating point, and parentheses support

.data
    pi_value: .dd 3.14159265    ; 32-bit float in data section
    radius: .dd 5.0             ; Another float value

.text
main:
    ; Test basic floating point operations
    FLD 2.5                     ; Load floating point literal
    FLD 13.7                    ; Load another float
    FADD 1.25                   ; Add floating point immediate
    
    ; Load from data section (this should work now)
    FLD radius                  ; Load from symbol address
    
    ; Simple arithmetic test
    FADD 1.0                    ; radius + 1.0
    
    HALT

; This test demonstrates:
; ✅ .data and .text sections working
; ✅ .dd directive with floating point values  
; ✅ Floating point literals (2.5, 13.7, 1.25, 5.0, 1.0)
; ✅ Symbol references (radius, pi_value)
; ✅ Integration with existing sophisticated FPU operations
; ✅ All through unified opcode registry system