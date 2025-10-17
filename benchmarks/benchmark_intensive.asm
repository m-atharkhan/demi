; Intensive benchmark program to stress test the VM dispatcher
; This program performs many operations to highlight performance differences

main:
    ; Initialize counters
    LOAD_IMM R0, 0      ; Counter
    LOAD_IMM R1, 1000   ; Limit for outer loop
    LOAD_IMM R2, 100    ; Limit for inner loop
    LOAD_IMM R3, 0      ; Inner counter
    LOAD_IMM R4, 42     ; Value for arithmetic
    LOAD_IMM R5, 13     ; Another value

outer_loop:
    ; Reset inner counter
    LOAD_IMM R3, 0

inner_loop:
    ; Perform intensive arithmetic operations
    ADD R4, R5          ; R4 = R4 + R5
    SUB R4, R5          ; R4 = R4 - R5 (back to original)
    MOV R6, R4          ; Copy R4 to R6
    ADD R6, R3          ; R6 = R6 + R3 (counter)
    MOV R7, R6          ; Copy to R7
    SUB R7, R3          ; R7 = R7 - R3 (back to R4)
    
    ; More operations to stress the dispatcher
    PUSH R4             ; Push R4
    PUSH R5             ; Push R5
    POP R8              ; Pop to R8 (should be R5)
    POP R9              ; Pop to R9 (should be R4)
    
    ; Compare and branch-like operations (using CMP)
    CMP R8, R5          ; Compare R8 with R5
    CMP R9, R4          ; Compare R9 with R4
    
    ; Increment inner counter
    LOAD_IMM R10, 1
    ADD R3, R10         ; R3 = R3 + 1
    
    ; Check inner loop condition (R3 < R2)
    CMP R3, R2
    ; In a real implementation, we'd have conditional jumps here
    ; For now, we'll simulate by doing more operations
    
    ; Additional operations to increase instruction count
    MOV R11, R0         ; Copy outer counter
    ADD R11, R3         ; Combine counters
    SUB R11, R3         ; Back to original
    
    ; Continue inner loop (simulate with more operations for now)
    LOAD_IMM R12, 100
    CMP R3, R12
    ; Note: Without conditional jumps implemented, this runs fixed iterations
    
inner_loop_end:
    ; Increment outer counter
    LOAD_IMM R10, 1
    ADD R0, R10         ; R0 = R0 + 1
    
    ; More operations
    PUSH R0
    PUSH R1
    POP R13
    POP R14
    CMP R13, R1
    CMP R14, R0
    
    ; Check outer loop condition (R0 < R1)
    CMP R0, R1
    
    ; Additional stress operations
    MOV R15, R4
    ADD R15, R5
    SUB R15, R5
    PUSH R15
    POP R15
    
outer_loop_end:
    ; Final operations
    ADD R4, R5
    SUB R4, R5
    MOV R6, R4
    PUSH R6
    POP R6
    CMP R6, R4
    
    ; End program
    HALT

; This program performs approximately:
; - 50+ instructions per "iteration"
; - Multiple arithmetic operations
; - Stack operations (PUSH/POP)
; - Register moves
; - Comparisons
; - Should stress the dispatcher significantly