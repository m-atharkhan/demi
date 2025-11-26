; ==========================================
; Fibonacci Sequence Generator
; ==========================================
; Calculates the first 10 Fibonacci numbers
; and stores them in memory starting at 0x100
; ==========================================

.org 0x00
    ; Initialize registers
    LOAD_IMM R0, 0      ; First number (F0)
    LOAD_IMM R1, 1      ; Second number (F1)
    LOAD_IMM R2, 10     ; Counter (calculate 10 numbers)
    LOAD_IMM R3, 0x100  ; Memory pointer

    ; Store first two numbers
    STORE R0, [R3]      ; Store F0
    INC R3              ; Increment pointer
    STORE R1, [R3]      ; Store F1
    INC R3              ; Increment pointer
    
    SUB R2, 2           ; Decrement counter by 2 (since we did 2)

loop:
    ; Calculate next number: F(n) = F(n-1) + F(n-2)
    ; R0 holds F(n-2), R1 holds F(n-1)
    
    MOV R4, R1          ; Save F(n-1) to temp
    ADD R1, R0          ; R1 = F(n-1) + F(n-2) = F(n)
    MOV R0, R4          ; R0 = old F(n-1) = new F(n-2)
    
    ; Store result
    STORE R1, [R3]
    INC R3
    
    ; Loop control
    DEC R2
    JNZ loop

    HALT
