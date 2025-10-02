; db.asm - Store and print strings using DB and OUTSTR

; Store "Hello, DemiEngine!\n" at address 0x50 (null-terminated)
DB 'Hello, DemiEngine!', 18, 0x50

; Start instructions at address 0x60
.org 0x60

; Print the string using OUTSTR
LOAD_IMM R0, 0x50      ; R0 = address of string
OUTSTR R0, 1           ; Output string at [R0] to console

HALT