; ==========================================
; Data Labels Demo (x86 32-bit)
; ==========================================
; This example demonstrates:
; - Multiple labeled data blocks
; - Different string lengths
; - Sequential data access
; Uses 32-bit addressing with 32-bit immediates

.org 0x100

; Define multiple labeled strings
greeting: 
    DB 'Hi!\n', 3
message: 
    DB 'Labels work!\n', 12

_start:
    ; Use both labels
    LOAD_IMM EAX, greeting
    OUTSTR EAX, 1

    LOAD_IMM EAX, message  
    OUTSTR EAX, 1

    HALT
