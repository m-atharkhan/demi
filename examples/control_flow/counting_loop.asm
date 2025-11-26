; Simple Loop Example
; This program demonstrates a counting loop
; Counts from 1 to 5 and outputs each number

main:
    ; Initialize counter in EAX
    load_imm EAX, 1     ; Start from 1
    load_imm EBX, 5     ; Loop limit
    
loop_start:
    ; Output current counter value (will show as ASCII character)
    ; Add 48 to convert number to ASCII digit
    mov ECX, EAX         ; Copy counter to ECX
    load_imm EDX, 48    ; ASCII offset for '0'
    add ECX, EDX         ; Convert to ASCII digit
    out ECX, 1          ; Output to console
    
    ; Output a space
    load_imm ECX, 32    ; ASCII space
    out ECX, 1
    
    ; Increment counter
    inc EAX
    
    ; Check if we've reached the limit
    cmp EAX, EBX
    jle loop_start     ; Jump if EAX <= EBX
    
    ; Output newline at end
    load_imm EAX, 10    ; ASCII newline
    out EAX, 1
    
    ; End program
    halt

; Expected output: "1 2 3 4 5 " followed by newline
