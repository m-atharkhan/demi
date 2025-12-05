; ==========================================
; Hello World Program (x64 64-bit)
; ==========================================
; Demonstrates string output using DB directive
; Uses 64-bit addressing

.data
; Store "Hello, World!\n" at address 0x50
.org 0x50
hello_msg: DB 'Hello, World!', 10, 0

.text
; Start instructions at address 0x70
.org 0x70

_start:
    ; Switch to 64-bit mode
    MODE64

    ; Print the string using OUTSTR
    LOAD_IMM RAX, hello_msg      ; RAX = address of string
    OUTSTR RAX, 1                ; Output string at [RAX] to console

    HALT
