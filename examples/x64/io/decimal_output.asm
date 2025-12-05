; ==========================================
; Decimal Output (x64 64-bit)
; ==========================================
; This program demonstrates how to output numbers as decimal digits
; Shows the number 123 as individual decimal digits
; Uses 64-bit registers

.text

_start:
    ; We want to output the number 123
    LOAD_IMM RAX, 123

    ; Extract hundreds digit: 123 / 100 = 1
    LOAD_IMM RBX, 100
    MOV RCX, RAX            ; Copy original number
    DIV RCX, RBX            ; RCX = 123 / 100 = 1
    LOAD_IMM RDI, 48        ; ASCII '0'
    ADD RCX, RDI            ; Convert to ASCII ('0' + 1 = '1')
    OUT RCX, 1              ; Output '1'

    ; Calculate remainder: 123 - (1 * 100) = 23
    MOV RDX, RCX            ; Copy quotient (1 + 48 = 49)
    SUB RDX, RDI            ; Convert back to number (49 - 48 = 1)
    MUL RDX, RBX            ; 1 * 100 = 100
    MOV RSI, RAX            ; Copy original (123)
    SUB RSI, RDX            ; 123 - 100 = 23

    ; Extract tens digit: 23 / 10 = 2
    LOAD_IMM RBX, 10
    MOV RCX, RSI            ; Copy remainder (23)
    DIV RCX, RBX            ; RCX = 23 / 10 = 2
    ADD RCX, RDI            ; Convert to ASCII
    OUT RCX, 1              ; Output '2'

    ; Calculate final remainder: 23 - (2 * 10) = 3
    MOV RDX, RCX            ; Copy quotient (2 + 48 = 50)
    SUB RDX, RDI            ; Convert back to number (50 - 48 = 2)
    MUL RDX, RBX            ; 2 * 10 = 20
    SUB RSI, RDX            ; 23 - 20 = 3

    ; Output ones digit
    ADD RSI, RDI            ; Convert to ASCII
    OUT RSI, 1              ; Output '3'

    ; Output newline
    LOAD_IMM RAX, 10
    OUT RAX, 1

    HALT
