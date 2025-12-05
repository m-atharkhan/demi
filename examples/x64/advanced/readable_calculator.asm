; ==========================================
; Readable Calculator (x64 64-bit)
; ==========================================
; Performs arithmetic and outputs readable results
; Demonstrates: 5 + 3 = 8
; Uses 64-bit registers

.text

_start:
    ; Define two single-digit numbers for simplicity
    LOAD_IMM RAX, 5         ; First number
    LOAD_IMM RBX, 3         ; Second number

    ; Output first number
    LOAD_IMM RCX, 48        ; ASCII '0'
    ADD RAX, RCX            ; Convert to ASCII
    OUT RAX, 1              ; Output '5'

    ; Output " + "
    LOAD_IMM RAX, 32        ; Space
    OUT RAX, 1
    LOAD_IMM RAX, 43        ; '+'
    OUT RAX, 1
    LOAD_IMM RAX, 32        ; Space
    OUT RAX, 1

    ; Output second number
    MOV RAX, RBX            ; Get second number (3)
    ADD RAX, RCX            ; Convert to ASCII
    OUT RAX, 1              ; Output '3'

    ; Output " = "
    LOAD_IMM RAX, 32        ; Space
    OUT RAX, 1
    LOAD_IMM RAX, 61        ; '='
    OUT RAX, 1
    LOAD_IMM RAX, 32        ; Space
    OUT RAX, 1

    ; Calculate and output result
    LOAD_IMM RAX, 5         ; Reload first number
    LOAD_IMM RBX, 3         ; Reload second number
    ADD RAX, RBX            ; 5 + 3 = 8
    ADD RAX, RCX            ; Convert to ASCII
    OUT RAX, 1              ; Output '8'

    ; Output newline
    LOAD_IMM RAX, 10
    OUT RAX, 1

    HALT
