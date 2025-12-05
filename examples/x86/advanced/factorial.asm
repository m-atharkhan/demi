; ==========================================
; Factorial Calculator (x86 32-bit)
; ==========================================
; Calculates factorial of a number iteratively
; Uses 32-bit registers for computation

.text

_start:
    ; Calculate 5! = 120
    LOAD_IMM EAX, 5     ; Number to calculate factorial of
    LOAD_IMM EBX, 1     ; Result accumulator (factorial)
    LOAD_IMM ECX, 1     ; Counter (starts at 1)
    
factorial_loop:
    ; Check if counter > number
    CMP ECX, EAX
    JG factorial_done   ; If ECX > EAX, we're done
    
    ; Multiply result by counter
    MOV EDX, EBX        ; Save current result
    LOAD_IMM ESI, 0     ; Clear accumulator for multiplication
    MOV EDI, ECX        ; Copy counter
    
multiply_loop:
    ; Manual multiplication: EBX = EBX * ECX
    CMP EDI, 0
    JE multiply_done
    ADD ESI, EDX        ; Add original result
    DEC EDI
    JMP multiply_loop
    
multiply_done:
    MOV EBX, ESI        ; Store multiplication result
    
    ; Increment counter and continue
    INC ECX
    JMP factorial_loop
    
factorial_done:
    ; Result is in EBX (should be 120 for 5!)
    MOV EAX, EBX        ; Move result to EAX
    HALT

; EAX contains 120 (5!)
