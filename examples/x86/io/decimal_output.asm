; ==========================================
; Decimal Output (x86 32-bit)
; ==========================================
; This program demonstrates how to output numbers as decimal digits
; Shows the number 123 as individual decimal digits
; Uses 32-bit registers

.text

_start:
    ; We want to output the number 123
    LOAD_IMM EAX, 123

    ; Extract hundreds digit: 123 / 100 = 1
    LOAD_IMM EBX, 100
    MOV ECX, EAX            ; Copy original number
    DIV ECX, EBX            ; ECX = 123 / 100 = 1
    LOAD_IMM EDI, 48        ; ASCII '0'
    ADD ECX, EDI            ; Convert to ASCII ('0' + 1 = '1')
    OUT ECX, 1              ; Output '1'

    ; Calculate remainder: 123 - (1 * 100) = 23
    MOV EDX, ECX            ; Copy quotient (1 + 48 = 49)
    SUB EDX, EDI            ; Convert back to number (49 - 48 = 1)
    MUL EDX, EBX            ; 1 * 100 = 100
    MOV ESI, EAX            ; Copy original (123)
    SUB ESI, EDX            ; 123 - 100 = 23

    ; Extract tens digit: 23 / 10 = 2
    LOAD_IMM EBX, 10
    MOV ECX, ESI            ; Copy remainder (23)
    DIV ECX, EBX            ; ECX = 23 / 10 = 2
    ADD ECX, EDI            ; Convert to ASCII
    OUT ECX, 1              ; Output '2'

    ; Calculate final remainder: 23 - (2 * 10) = 3
    MOV EDX, ECX            ; Copy quotient (2 + 48 = 50)
    SUB EDX, EDI            ; Convert back to number (50 - 48 = 2)
    MUL EDX, EBX            ; 2 * 10 = 20
    SUB ESI, EDX            ; 23 - 20 = 3

    ; Output ones digit
    ADD ESI, EDI            ; Convert to ASCII
    OUT ESI, 1              ; Output '3'

    ; Output newline
    LOAD_IMM EAX, 10
    OUT EAX, 1

    HALT
