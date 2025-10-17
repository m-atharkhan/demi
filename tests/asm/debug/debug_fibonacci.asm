; Debug fibonacci issue
LOAD_IMM R0, 0  ; F(0) = 0
LOAD_IMM R1, 1  ; F(1) = 1  
LOAD_IMM R2, 2  ; Counter
LOAD_IMM R3, 10 ; Target

fib_loop:
    CMP R2, R3
    JZ fib_done
    
    ; R4 = R0 + R1
    MOV R4, R0
    ADD R4, R1
    
    ; Shift: R0 = R1, R1 = R4
    MOV R0, R1
    MOV R1, R4
    
    INC R2
    JMP fib_loop
    
fib_done:
    ; F(10) = 55, but we got 34
    HALT