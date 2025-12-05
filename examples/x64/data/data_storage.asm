; ==========================================
; Data Storage and String Output (x64 64-bit)
; ==========================================
; Demonstrates DB directive and string operations
; Uses 64-bit addressing

; Store string data at address 0x50
DB 'Hello, DemiEngine!', 10, 0x50

; Store another string at 0x80
DB 'x64 64-bit mode', 10, 0x80

; Start program at address 0xA0
.org 0xA0

_start:
    ; Print first string
    LOAD_IMM RAX, 0x50      ; RAX = address of first string
    OUTSTR RAX, 1           ; Output string at [RAX]

    ; Print second string
    LOAD_IMM RAX, 0x80      ; RAX = address of second string
    OUTSTR RAX, 1           ; Output string at [RAX]

    HALT
