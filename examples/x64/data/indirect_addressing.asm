; ==========================================
; Indirect Addressing Demo (x64 64-bit)
; ==========================================
; Test LOADR instruction - demonstrates indirect addressing
; Uses 64-bit registers

.text

_start:
    ; Store value 42 at memory address 100
    LOAD_IMM RAX, 42
    STORE RAX, 100

    ; Store address 100 in register RCX
    LOAD_IMM RCX, 100

    ; Use LOADR to load from address stored in RCX
    LOADR RDX, RCX

    ; RDX should now contain 42
    HALT
