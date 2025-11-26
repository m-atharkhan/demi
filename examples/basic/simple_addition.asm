; Simple Addition Program
; This program adds two numbers and demonstrates basic arithmetic
; Shows register operations and data movement

main:
    ; Load first number (42) into EAX
    LOAD_IMM EAX, 42
    
    ; Load second number (13) into EBX  
    LOAD_IMM EBX, 13
    
    ; Add EAX + EBX, result goes into EAX
    ADD EAX, EBX
    
    ; Copy result to other registers for demonstration
    MOV ECX, EAX             ; Copy result to ECX
    MOV EDX, EAX             ; Copy result to EDX
    
    ; Perform additional operations on the result
    LOAD_IMM ESI, 10
    SUB ECX, ESI             ; ECX = result - 10 = 55 - 10 = 45
    
    ADD EDX, ESI             ; EDX = result + 10 = 55 + 10 = 65
    
    ; End program
    HALT

; The main result (55) is in EAX
; Modified results are in ECX (45) and EDX (65)
; Note: Memory STORE/LOAD operations are not yet implemented in assembler
