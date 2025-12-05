; ==========================================
; Factorial Calculator (x64 64-bit)
; ==========================================
; Calculates factorial of a number iteratively
; Uses 64-bit registers for larger factorial values

.text

_start:
    ; Calculate 5! = 120
    LOAD_IMM RAX, 5     ; Number to calculate factorial of
    LOAD_IMM RBX, 1     ; Result accumulator (factorial)
    LOAD_IMM RCX, 1     ; Counter (starts at 1)
    
factorial_loop:
    ; Check if counter > number
    CMP RCX, RAX
    JG factorial_done   ; If RCX > RAX, we're done
    
    ; Multiply result by counter
    MOV RDX, RBX        ; Save current result
    LOAD_IMM RSI, 0     ; Clear accumulator for multiplication
    MOV RDI, RCX        ; Copy counter
    
multiply_loop:
    ; Manual multiplication: RBX = RBX * RCX
    CMP RDI, 0
    JE multiply_done
    ADD RSI, RDX        ; Add original result
    DEC RDI
    JMP multiply_loop
    
multiply_done:
    MOV RBX, RSI        ; Store multiplication result
    
    ; Increment counter and continue
    INC RCX
    JMP factorial_loop
    
factorial_done:
    ; Result is in RBX (should be 120 for 5!)
    MOV RAX, RBX        ; Move result to RAX
    HALT

; RAX contains 120 (5!)
; x64 advantage: Can calculate larger factorials (up to 20!)
