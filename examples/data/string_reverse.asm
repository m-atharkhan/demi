; ==========================================
; String Reverse
; ==========================================
; Reverses a string in place
; ==========================================

.org 0x00
    JMP start

.org 0x100
string_data:
    DB "Hello World!", 0

start:
    ; Find string length
    LOAD_IMM R0, 0x100  ; Start address
    MOV R1, R0          ; End address scanner

find_end:
    LOAD R2, [R1]       ; Load char
    CMP R2, 0           ; Check for null terminator
    JZ found_end
    INC R1
    JMP find_end

found_end:
    DEC R1              ; Point to last char (not null)

reverse_loop:
    ; Check if pointers crossed
    CMP R0, R1
    JGE done            ; If start >= end, we are done
    
    ; Swap characters
    LOAD R2, [R0]       ; Load char from start
    LOAD R3, [R1]       ; Load char from end
    
    STORE R3, [R0]      ; Store end char at start
    STORE R2, [R1]      ; Store start char at end
    
    ; Move pointers
    INC R0
    DEC R1
    JMP reverse_loop

done:
    HALT
