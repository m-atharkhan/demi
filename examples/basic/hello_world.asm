; Hello World Assembly Example
; This program prints "Hello World!" to the console using OUT instructions

main:
    ; Print "Hello World!"
    LOAD_IMM EAX, 72        ; ASCII 'H'
    OUT EAX, 1              ; Output to console

    LOAD_IMM EAX, 101       ; ASCII 'e'
    OUT EAX, 1

    LOAD_IMM EAX, 108       ; ASCII 'l'
    OUT EAX, 1

    LOAD_IMM EAX, 108       ; ASCII 'l'
    OUT EAX, 1

    LOAD_IMM EAX, 111       ; ASCII 'o'
    OUT EAX, 1

    LOAD_IMM EAX, 32        ; ASCII ' ' (space)
    OUT EAX, 1

    LOAD_IMM EAX, 87        ; ASCII 'W'
    OUT EAX, 1

    LOAD_IMM EAX, 111       ; ASCII 'o'
    OUT EAX, 1

    LOAD_IMM EAX, 114       ; ASCII 'r'
    OUT EAX, 1

    LOAD_IMM EAX, 108       ; ASCII 'l'
    OUT EAX, 1

    LOAD_IMM EAX, 100       ; ASCII 'd'
    OUT EAX, 1

    LOAD_IMM EAX, 33        ; ASCII '!'
    OUT EAX, 1

    LOAD_IMM EAX, 10        ; ASCII '\n' (newline)
    OUT EAX, 1

    ; End program
    HALT
