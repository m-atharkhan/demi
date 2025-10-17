; Benchmark program for threaded code performance testing
; Performs repetitive operations to stress-test the dispatcher

; Simple loop that executes many instructions
main:
    LOAD_IMM R0, 0        ; Initialize counter
    LOAD_IMM R1, 200      ; Loop limit (200 iterations, within byte range)
    LOAD_IMM R2, 1        ; Increment value
    
loop:
    ADD R0, R2            ; R0++
    ADD R3, R0            ; Accumulate in R3
    SUB R4, R0            ; Some arithmetic
    MOV R5, R0            ; Register move
    CMP R0, R1            ; Compare counter to limit
    JL loop               ; Jump if less than limit
    
    ; Final operations
    MOV R6, R3            ; Store final result
    HALT
