; FPU Arithmetic Test

main:
    ; Load two values
    LOAD_IMM R0, 10
    LOAD_IMM R1, 5
    
    ; Load both to FPU stack
    FLD R0          ; Load 10 to FPU stack (ST0)
    FLD R1          ; Load 5 to FPU stack (ST0), 10 -> ST1
    
    ; Add them using immediate value (this should add 3.0 to ST0)
    FADD 3          ; Should add 3 to ST0 (5), result = 8
    
    ; Store result
    FST R2          ; Store result to R2
    
    ; Output result  
    OUT R2, 1       ; Output the result
    
    HALT