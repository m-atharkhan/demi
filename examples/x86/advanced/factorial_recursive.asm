; ==========================================
; Recursive Factorial Calculator (x86 32-bit)
; ==========================================
; Calculates factorial of a number using recursion
; Demonstrates: Stack operations, CALL/RET, Recursion
; Uses 32-bit registers

.text

_start:
    ; Calculate 5!
    LOAD_IMM EAX, 5         ; Calculate 5!
    PUSH EAX                ; Push argument
    CALL factorial
    ADD ESP, 4              ; Clean up stack
    
    ; Result is in EAX
    HALT

factorial:
    ; Prologue
    PUSH EBP                ; Save frame pointer
    MOV EBP, ESP            ; Set new frame pointer
    
    ; Get argument
    MOV EAX, [EBP + 8]      ; Load argument into EAX
    
    ; Base case: if n <= 1, return 1
    CMP EAX, 1
    JLE base_case
    
    ; Recursive case: n * factorial(n-1)
    DEC EAX                 ; n-1
    PUSH EAX                ; Push n-1
    CALL factorial
    ADD ESP, 4              ; Clean up stack
    
    ; EAX has factorial(n-1)
    ; Multiply by n (original argument)
    MOV EBX, [EBP + 8]      ; Reload n
    MUL EAX, EBX            ; EAX = EAX * EBX
    
    JMP end_factorial

base_case:
    LOAD_IMM EAX, 1         ; Return 1

end_factorial:
    ; Epilogue
    MOV ESP, EBP            ; Restore stack pointer
    POP EBP                 ; Restore old frame pointer
    RET
