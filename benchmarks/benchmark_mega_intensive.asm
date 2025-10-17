; Very intensive benchmark for measuring dispatcher performance
; This program runs many operations to stress-test the VM

main:
    ; Initialize registers with test values
    LOAD_IMM R0, 1000000    ; Very large loop counter
    LOAD_IMM R1, 42         ; Test value 1
    LOAD_IMM R2, 13         ; Test value 2
    LOAD_IMM R3, 0          ; Accumulator
    LOAD_IMM R4, 1          ; Increment value

    ; This simulates a very long loop since we don't have conditional jumps yet
    ; Each "iteration" performs multiple dispatcher calls

    ; Iteration 1-1000: Arithmetic stress test
    ADD R3, R1              ; R3 = R3 + 42
    SUB R3, R2              ; R3 = R3 - 13 = R3 + 29
    ADD R1, R4              ; R1 = R1 + 1 = 43
    SUB R1, R4              ; R1 = R1 - 1 = 42 (restore)
    MOV R5, R3              ; Copy accumulator
    PUSH R1                 ; Save R1
    PUSH R2                 ; Save R2
    POP R6                  ; Load to R6 (should be 13)
    POP R7                  ; Load to R7 (should be 42)
    CMP R6, R2              ; Compare
    CMP R7, R1              ; Compare
    ADD R3, R1              ; More arithmetic
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Repeat the pattern many times to create intensive load
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; More iterations
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 4
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 5
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 6
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 7
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 8
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 9
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Pattern 10
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Now let's repeat this entire block multiple times to really stress the dispatcher
    ; Block 2
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Block 3 - Even more operations
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9
    ADD R3, R1
    SUB R3, R2
    ADD R1, R4
    SUB R1, R4
    MOV R5, R3
    PUSH R1
    PUSH R2
    POP R6
    POP R7
    CMP R6, R2
    CMP R7, R1
    ADD R3, R1
    SUB R3, R2
    MOV R8, R3
    PUSH R8
    POP R9
    CMP R8, R9

    ; Final operations
    MOV R10, R3             ; Save final result
    ADD R10, R1
    SUB R10, R2
    PUSH R10
    POP R11
    CMP R10, R11

    HALT

; This program executes approximately 300+ instructions
; Each instruction goes through the dispatcher
; Should provide measurable performance differences between dispatchers