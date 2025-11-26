; db.asm - Store and print strings using DB and OUTSTR

; Store "Hello, DemiEngine!\n" at address 0x50 (null-terminated)
DB 'Hello, DemiEngine!', 18, 0x50

; Start instructions at address 0x60
.org 0x60

; Print the string using OUTSTR
LOAD_IMM EAX, 0x50      ; EAX = address of string
OUTSTR EAX, 1           ; Output string at [EAX] to console

HALT