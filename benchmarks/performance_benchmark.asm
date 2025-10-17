; Performance benchmark - lots of arithmetic operations
; This will stress-test the dispatcher performance

main:
    LOAD_IMM R0, 1
    LOAD_IMM R1, 1
    
    ; Loop counter in R2
    LOAD_IMM R2, 100
    
loop:
    ADD R0, R1      ; R0 = R0 + R1
    SUB R2, R1      ; R2 = R2 - 1 (decrement counter)
    MOV R3, R2      ; Copy R2 to R3
    CMP R3, R0      ; Compare (this will be optimized as nop for now)
    
    ; Simple conditional: if R2 != 0, continue loop
    ; For now, just manually unroll the loop
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    
    ; More operations to stress test
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    ADD R0, R1
    SUB R2, R1
    
    HALT