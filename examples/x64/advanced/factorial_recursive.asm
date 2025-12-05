; ==========================================
; Recursive Factorial Calculator (x64 64-bit)
; ==========================================
; Calculates factorial of a number using recursion
; Demonstrates: Stack operations, CALL/RET, Recursion
; Uses 64-bit registers

.text

_start:
    ; Calculate 5!
    LOAD_IMM RAX, 5         ; Calculate 5!
    PUSH RAX                ; Push argument
    CALL factorial
    ADD RSP, 8              ; Clean up stack
    
    ; Result is in RAX
    HALT

factorial:
    ; Prologue
    PUSH RBP                ; Save frame pointer
    MOV RBP, RSP            ; Set new frame pointer
    
    ; Get argument
    MOV RAX, [RBP + 16]     ; Load argument into RAX
    
    ; Base case: if n <= 1, return 1
    CMP RAX, 1
    JLE base_case
    
    ; Recursive case: n * factorial(n-1)
    DEC RAX                 ; n-1
    PUSH RAX                ; Push n-1
    CALL factorial
    ADD RSP, 8              ; Clean up stack
    
    ; RAX has factorial(n-1)
    ; Multiply by n (original argument)
    MOV RBX, [RBP + 16]     ; Reload n
    MUL RAX, RBX            ; RAX = RAX * RBX
    
    JMP end_factorial

base_case:
    LOAD_IMM RAX, 1         ; Return 1

end_factorial:
    ; Epilogue
    MOV RSP, RBP            ; Restore stack pointer
    POP RBP                 ; Restore old frame pointer
    RET
