; Core Instructions Demo
; This program demonstrates the basic instructions that are currently
; implemented in the Demi Engine assembler

main:
    ; Basic immediate loading
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 50
    LOAD_IMM ECX, 25

    ; Arithmetic operations
    ADD EAX, EBX             ; EAX = 100 + 50 = 150
    SUB EBX, ECX             ; EBX = 50 - 25 = 25
    MUL ECX, ECX             ; ECX = 25 * 25 = 625

    ; Data movement
    MOV EDX, EAX             ; Copy EAX to EDX
    MOV ESI, EBX             ; Copy EBX to ESI
    MOV EDI, ECX             ; Copy ECX to EDI

    ; Comparison (sets flags but doesn't jump yet)
    CMP EAX, EBX             ; Compare 150 vs 25

    ; Logical operations
    LOAD_IMM ESP, 0xFF
    LOAD_IMM EBP, 0x0F
    AND ESP, EBP             ; ESP = 0xFF & 0x0F = 0x0F

    OR ESP, EAX              ; ESP = 0x0F | 150 = bitwise OR
    XOR EBP, EBX             ; EBP = 0x0F ^ 25 = bitwise XOR

    ; Stack operations
    PUSH EAX                ; Push 150 onto stack
    PUSH EBX                ; Push 25 onto stack
    PUSH ECX                ; Push 625 onto stack

    ; Pop in reverse order
    POP EDX                 ; EDX = 625
    POP ESI                 ; ESI = 25
    POP EDI                 ; EDI = 150

    ; Increment/Decrement
    INC EAX                 ; EAX = 151
    DEC EBX                 ; EBX = 24

    ; Bitwise NOT
    NOT EBP                 ; EBP = ~EBP

    ; End program
    HALT

; This program demonstrates all the core instructions that are
; currently implemented in the assembler:
; - LOAD_IMM: Load immediate values
; - ADD, SUB, MUL, DIV: Arithmetic operations
; - MOV: Data movement between registers
; - CMP: Comparison (sets flags)
; - AND, OR, XOR: Logical operations
; - PUSH, POP: Stack operations
; - INC, DEC: Increment/Decrement
; - NOT: Bitwise complement
; - HALT: End program
