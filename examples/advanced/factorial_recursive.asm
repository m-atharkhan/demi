; Recursive Factorial Calculator
; Calculates factorial of a number using recursion
; Demonstrates: Stack operations, CALL/RET, Recursion

.data
    input_msg: .string "Enter a number (0-12): "
    result_msg: .string "Factorial is: "
    newline: .string "\n"
    
.bss
    buffer: .memory 16

.text
    ; Main program
    LOAD_IMM R0, 5          ; Calculate 5!
    PUSH R0                 ; Push argument
    CALL factorial
    ADD R4, 1               ; Clean up stack (R4 is SP)
    
    ; Result is in R0
    ; (In a real implementation, we would print it here)
    HALT

factorial:
    ; Prologue
    PUSH R5                 ; Save frame pointer (R5 is BP)
    MOV R5, R4              ; Set new frame pointer
    
    ; Get argument
    ; Stack: [Old BP] [Ret Addr] [Arg]
    ;        BP       BP+1       BP+2
    LOADR R1, [R5 + 2]      ; Load argument into R1
    
    ; Base case: if n <= 1, return 1
    CMP R1, 1
    JLE base_case
    
    ; Recursive case: n * factorial(n-1)
    DEC R1                  ; n-1
    PUSH R1                 ; Push n-1
    CALL factorial
    ADD R4, 1               ; Clean up stack
    
    ; R0 has factorial(n-1)
    ; Multiply by n (original argument)
    LOADR R1, [R5 + 2]      ; Reload n
    MUL R0, R1              ; R0 = R0 * R1
    
    JMP end_factorial

base_case:
    LOAD_IMM R0, 1          ; Return 1

end_factorial:
    ; Epilogue
    MOV R4, R5              ; Restore stack pointer
    POP R5                  ; Restore old frame pointer
    RET
