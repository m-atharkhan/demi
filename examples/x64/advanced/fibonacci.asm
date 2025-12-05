; ==========================================
; Fibonacci Sequence Generator (x64 64-bit)
; ==========================================
; Calculates the first 10 Fibonacci numbers
; and stores them in memory starting at 0x100
; Uses 64-bit general purpose registers

.text
.org 0x00

_start:
    ; Initialize registers
    LOAD_IMM RAX, 0      ; First number (F0)
    LOAD_IMM RBX, 1      ; Second number (F1)
    LOAD_IMM RCX, 10     ; Counter (calculate 10 numbers)
    LOAD_IMM RDX, 0x100  ; Memory pointer

    ; Store first two numbers
    STORE RAX, [RDX]     ; Store F0
    INC RDX              ; Increment pointer
    STORE RBX, [RDX]     ; Store F1
    INC RDX              ; Increment pointer
    
    SUB RCX, 2           ; Decrement counter by 2

loop:
    ; Calculate next number: F(n) = F(n-1) + F(n-2)
    ; RAX holds F(n-2), RBX holds F(n-1)
    
    MOV RSI, RBX         ; Save F(n-1) to temp
    ADD RBX, RAX         ; RBX = F(n-1) + F(n-2) = F(n)
    MOV RAX, RSI         ; RAX = old F(n-1) = new F(n-2)
    
    ; Store result
    STORE RBX, [RDX]
    INC RDX
    
    ; Loop control
    DEC RCX
    JNZ loop

    HALT

; Memory at 0x100+ contains: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34
