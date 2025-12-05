; ==========================================
; Data Storage and String Output (x86 32-bit)
; ==========================================
; Demonstrates DB directive and string operations
; Uses 32-bit addressing

; Store string data at address 0x50
DB 'Hello, DemiEngine!', 10, 0x50

; Store another string at 0x80
DB 'x86 32-bit mode', 10, 0x80

; Start program at address 0xA0
.org 0xA0

_start:
    ; Print first string
    LOAD_IMM EAX, 0x50      ; EAX = address of first string
    OUTSTR EAX, 1           ; Output string at [EAX]

    ; Print second string
    LOAD_IMM EAX, 0x80      ; EAX = address of second string
    OUTSTR EAX, 1           ; Output string at [EAX]

    HALT
