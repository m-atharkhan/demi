; Simple benchmark - just arithmetic operations
main:
    LOAD_IMM R0, 10       ; Load initial value
    LOAD_IMM R1, 5        ; Load operand
    
    ; Perform multiple operations
    ADD R0, R1            ; 10 + 5 = 15
    SUB R0, R1            ; 15 - 5 = 10  
    ADD R0, R1            ; 10 + 5 = 15
    SUB R0, R1            ; 15 - 5 = 10
    ADD R0, R1            ; 10 + 5 = 15
    SUB R0, R1            ; 15 - 5 = 10
    ADD R0, R1            ; 10 + 5 = 15
    SUB R0, R1            ; 15 - 5 = 10
    ADD R0, R1            ; 10 + 5 = 15
    SUB R0, R1            ; 15 - 5 = 10
    
    ; More operations
    MOV R2, R0            ; Copy result
    MOV R3, R1            ; Copy operand
    ADD R2, R3            ; More arithmetic
    SUB R2, R3            ; More arithmetic
    MOV R4, R2            ; More copies
    
    HALT