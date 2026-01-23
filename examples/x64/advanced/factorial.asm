; ==========================================
; Factorial Calculator (x64 64-bit)
; ==========================================
; Calculates factorial of a number iteratively
; Uses 64-bit registers for larger factorial values

.data
    result_msg: DB "factorial(5) = ", 0
    result_msg_len EQU 14
    newline: DB 10

    num_buf: RESB 32

.text

_start:
    ; Calculate 5! = 120
    LOAD_IMM RAX, 5     ; Number to calculate factorial of
    LOAD_IMM RBX, 1     ; Result accumulator (factorial)
    LOAD_IMM RCX, 1     ; Counter (starts at 1)
    LOAD_IMM RBP, 0     ; Zero constant for comparisons
    
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
    CMP RDI, RBP        ; Compare with zero
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
    MOV RDI, RAX        ; Preserve result across syscalls

    ; Print result using sys_write
    LOAD_IMM RCX, result_msg
    LOAD_IMM RDX, result_msg_len
    CALL print_str
    MOV RAX, RDI
    CALL print_u64
    CALL print_newline

    HALT

; RAX contains 120 (5!)
; x64 advantage: Can calculate larger factorials (up to 20!)

; ==========================================
; Output helpers (Linux syscalls via INT 0x80)
; ==========================================

print_str:
    ; RCX = pointer, RDX = length
    LOAD_IMM RAX, 4      ; sys_write
    LOAD_IMM RBX, 1      ; fd = stdout
    INT 0x80
    RET

print_newline:
    LOAD_IMM RCX, newline
    LOAD_IMM RDX, 1
    JMP print_str

print_u64:
    ; Input: RAX = value
    LOAD_IMM RDI, num_buf
    CALL itoa

    LOAD_IMM RAX, 4      ; sys_write
    LOAD_IMM RBX, 1      ; stdout
    LOAD_IMM RCX, num_buf
    MOV RDX, RSI         ; length
    INT 0x80
    RET

; ==========================================
; Function: itoa (Integer to ASCII)
; Input: RAX = integer value, RDI = destination buffer
; Output: RSI = length of string
; ==========================================
itoa:
    LOAD_IMM RSI, 0         ; Length counter
    LOAD_IMM RBP, 10        ; Base 10

    ; Handle zero special case
    CMP RAX, 0
    JNZ itoa_convert
    LOAD_IMM RBX, 48        ; '0'
    STORER RDI, RBX         ; Store at address in RDI
    LOAD_IMM RSI, 1
    RET

itoa_convert:
    ; Convert digits in reverse, then reverse in place
    MOV RBX, RDI            ; Save start position

itoa_digit_loop:
    DIV RAX, RAX, RBP       ; Quotient in RAX, remainder in RDX
    LOAD_IMM RCX, 48
    ADD RDX, RCX            ; Convert remainder to ASCII
    STORER RDI, RDX         ; Store digit
    INC RDI
    INC RSI

    CMP RAX, 0
    JNZ itoa_digit_loop

    ; Reverse the string
    MOV RCX, RBX            ; Start position
    DEC RDI                 ; End position

itoa_reverse_loop:
    CMP RCX, RDI
    JGE itoa_done

    LOADR RAX, RCX          ; Load from start
    LOADR RBX, RDI          ; Load from end
    STORER RCX, RBX         ; Store at start
    STORER RDI, RAX         ; Store at end

    INC RCX
    DEC RDI
    JMP itoa_reverse_loop

itoa_done:
    RET
