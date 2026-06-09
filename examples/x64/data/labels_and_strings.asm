; ==========================================
; Labels and Strings Demo (x64 64-bit)
; ==========================================
; Demonstrates DB labels with LOAD_IMM for address referencing
; Uses 64-bit addressing with .data and .text sections

.data
    mystring: DB 'Hello from labeled data!', 10, 0

.text
.org 0x100

_start:
    ; Load address of string and output it
    LOAD_IMM RAX, mystring  ; RAX = address of string data
    OUTSTR RAX, 1           ; Output string at [RAX] to console
    HALT
