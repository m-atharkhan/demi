; Improved Calculator Example
; This program performs arithmetic and outputs readable results
; Demonstrates: 5 + 3 = 8

main:
    ; Define two single-digit numbers for simplicity
    LOAD_IMM EAX, 5     ; First number
    LOAD_IMM EBX, 3     ; Second number

    ; Output first number
    LOAD_IMM ECX, 48    ; ASCII '0'
    ADD EAX, ECX         ; Convert to ASCII
    OUT EAX, 1          ; Output '5'

    ; Output " + "
    LOAD_IMM EAX, 32    ; Space
    OUT EAX, 1
    LOAD_IMM EAX, 43    ; '+'
    OUT EAX, 1
    LOAD_IMM EAX, 32    ; Space
    OUT EAX, 1

    ; Output second number
    MOV EAX, EBX         ; Get second number (3)
    ADD EAX, ECX         ; Convert to ASCII
    OUT EAX, 1          ; Output '3'

    ; Output " = "
    LOAD_IMM EAX, 32    ; Space
    OUT EAX, 1
    LOAD_IMM EAX, 61    ; '='
    OUT EAX, 1
    LOAD_IMM EAX, 32    ; Space
    OUT EAX, 1

    ; Calculate and output result
    LOAD_IMM EAX, 5     ; Reload first number
    LOAD_IMM EBX, 3     ; Reload second number
    ADD EAX, EBX         ; 5 + 3 = 8
    ADD EAX, ECX         ; Convert to ASCII
    OUT EAX, 1          ; Output '8'

    ; Output newline
    LOAD_IMM EAX, 10
    OUT EAX, 1

    HALT

; This demonstrates readable arithmetic output
; Result: "5 + 3 = 8"
