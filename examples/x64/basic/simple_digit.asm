; ==========================================
; Simple Number Output (x64 64-bit)
; ==========================================
; Output the digit 5 to console
; Uses 64-bit registers

.text

_start:
    LOAD_IMM RAX, 5      ; Load the number 5
    LOAD_IMM RBX, 48     ; Load ASCII '0' value
    ADD RAX, RBX         ; Convert to ASCII ('0' + 5 = '5')
    OUT RAX, 1           ; Output '5'

    LOAD_IMM RAX, 10     ; Newline
    OUT RAX, 1

    HALT
