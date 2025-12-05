; ==========================================
; Simple Number Output (x86 32-bit)
; ==========================================
; Output the digit 5 to console
; Uses 32-bit registers

.text

_start:
    LOAD_IMM EAX, 5      ; Load the number 5
    LOAD_IMM EBX, 48     ; Load ASCII '0' value
    ADD EAX, EBX         ; Convert to ASCII ('0' + 5 = '5')
    OUT EAX, 1           ; Output '5'

    LOAD_IMM EAX, 10     ; Newline
    OUT EAX, 1

    HALT
