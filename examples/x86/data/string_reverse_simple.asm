; Simple string reverse test
.data
.org 0x50
mystring: DB "Hello", 0

.text
.org 0x100

_start:
    LOAD_IMM EAX, 0x50
    MOV EBX, EAX
    HALT
