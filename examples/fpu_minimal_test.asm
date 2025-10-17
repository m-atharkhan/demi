; Ultra Simple FPU Test Program

main:
    ; Load a simple value first
    LOAD_IMM R0, 10
    
    ; Test FPU Load operation
    FLD R0          ; Load 10 to FPU stack (ST0)
    
    ; Test FPU Store operation  
    FST R1          ; Store FPU stack top to R1
    
    ; Output the result
    OUT R1, 1       ; This will output the raw value
    
    HALT