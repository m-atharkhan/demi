; Intensive performance benchmark - more operations
; This will stress-test the dispatcher performance with many instruction dispatches

main:
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    
    ; Lots of arithmetic operations - each one requires dispatcher overhead
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    
    ; More ops
    MOV R3, R0
    MOV R0, R1
    MOV R1, R2
    MOV R2, R3
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    
    ; Even more ops
    MOV R3, R0
    MOV R0, R1
    MOV R1, R2
    MOV R2, R3
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    ADD R0, R1
    SUB R0, R2
    
    HALT