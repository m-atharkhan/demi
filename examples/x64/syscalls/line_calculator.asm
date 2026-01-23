; ==========================================
; Line Calculator (x64 64-bit)
; ==========================================
; Interactive calculator that reads a single line in the form:
;   [first num] [operator] [second num]\n
; Example:
;   12 + 34
;   10*5
;   -7 / 2
;
; Supported operators: + - * /
; Quit: enter 'q' and press Enter.
;
; Uses Linux-like syscalls via INT 0x80:
;   sys_read  (RAX=3)  fd=0, buf=RCX, count=RDX
;   sys_write (RAX=4)  fd=1, buf=RCX, count=RDX

.data
    prompt: DB "calc> ", 0
    prompt_len EQU 6

    err_parse: DB "Error: expected 'a op b'", 10, 0
    err_parse_len EQU 26

    err_div0: DB "Error: division by zero", 10, 0
    err_div0_len EQU 25

    newline: DB 10
    minus: DB '-', 0

    input_buf: RESB 128
    num_buf: RESB 32

.text

_start:
repl_loop:
    ; print prompt
    LOAD_IMM RCX, prompt
    LOAD_IMM RDX, prompt_len
    CALL print_str

    ; read line
    LOAD_IMM RAX, 3      ; sys_read
    LOAD_IMM RBX, 0      ; fd=stdin
    LOAD_IMM RCX, input_buf
    LOAD_IMM RDX, 127
    INT 0x80

    ; EOF -> exit
    CMP RAX, 0
    JZ exit_program

    ; NUL-terminate: input_buf[RAX] = 0
    LOAD_IMM RDI, input_buf
    ADD RDI, RAX         ; RDI = input_buf + bytes_read
    LOAD_IMM RBX, 0
    STORER RDI, RBX

    ; RSI = cursor
    LOAD_IMM RSI, input_buf
    CALL skip_spaces

    ; if first non-space is 'q' or 'Q', exit
    LOADR RAX, RSI
    CMP RAX, 113         ; 'q'
    JZ exit_program
    CMP RAX, 81          ; 'Q'
    JZ exit_program

    ; parse first integer -> RAX, advances RSI
    CALL parse_signed_int
    MOV RDI, RAX         ; first operand

    ; parse operator
    CALL skip_spaces
    LOADR RBX, RSI        ; op char
    INC RSI

    ; parse second integer -> RAX, advances RSI
    CALL skip_spaces
    CALL parse_signed_int
    MOV RCX, RAX         ; second = RCX

    ; compute
    MOV RAX, RDI         ; result in RAX

    CMP RBX, 43          ; '+'
    JZ do_add
    CMP RBX, 45          ; '-'
    JZ do_sub
    CMP RBX, 42          ; '*'
    JZ do_mul
    CMP RBX, 47          ; '/'
    JZ do_div
    JMP parse_error

do_add:
    ADD RAX, RCX
    JMP print_result

do_sub:
    SUB RAX, RCX
    JMP print_result

do_mul:
    MUL64 RAX, RAX, RCX
    JMP print_result

do_div:
    CMP RCX, 0
    JZ div0_error
    CALL signed_div_i64
    JMP print_result

print_result:
    CALL print_i64
    CALL print_newline
    JMP repl_loop

parse_error:
    LOAD_IMM RCX, err_parse
    LOAD_IMM RDX, err_parse_len
    CALL print_str
    JMP repl_loop

div0_error:
    LOAD_IMM RCX, err_div0
    LOAD_IMM RDX, err_div0_len
    CALL print_str
    JMP repl_loop

exit_program:
    LOAD_IMM RAX, 1
    LOAD_IMM RBX, 0
    INT 0x80
    HALT

; ==========================================
; Output helpers
; ==========================================

print_str:
    ; RCX = pointer, RDX = length
    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    INT 0x80
    RET

print_newline:
    LOAD_IMM RCX, newline
    LOAD_IMM RDX, 1
    JMP print_str

print_i64:
    ; Input: RAX = signed integer
    ; Prints a leading '-' for negative values.
    MOV RDI, RAX         ; preserve original (avoid stack: PUSH/POP are 32-bit words)

    ; if negative, print '-' and negate
    CMP RDI, 0
    JGE print_i64_abs

    LOAD_IMM RCX, minus
    LOAD_IMM RDX, 1
    CALL print_str

    LOAD_IMM RAX, 0
    SUB64 RAX, RDI
    JMP print_i64_convert

print_i64_abs:
    MOV RAX, RDI

print_i64_convert:
    LOAD_IMM RDI, num_buf
    CALL itoa_u64

    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, num_buf
    MOV RDX, RSI
    INT 0x80
    RET

; ==========================================
; Signed division helper
; Input:  RAX = dividend (signed), RCX = divisor (signed, non-zero)
; Output: RAX = quotient (signed)
; Notes:  DIV is unsigned; implement sign manually.
; ==========================================

signed_div_i64:
    ; Input:  RAX = dividend (signed), RCX = divisor (signed, non-zero)
    ; Output: RAX = quotient (signed)

    MOV RDI, RAX         ; a
    MOV RSI, RCX         ; b
    LOAD_IMM RBX, 0      ; sign_res (0=positive, 1=negative)

    ; if a < 0: sign_res = 1, a = -a
    CMP RDI, 0
    JGE signed_div_a_done
    LOAD_IMM RBX, 1
    LOAD_IMM RAX, 0
    SUB64 RAX, RDI
    MOV RDI, RAX
signed_div_a_done:

    ; if b < 0: sign_res = 1 - sign_res, b = -b
    CMP RSI, 0
    JGE signed_div_b_done
    LOAD_IMM RDX, 1
    SUB64 RDX, RBX
    MOV RBX, RDX
    LOAD_IMM RAX, 0
    SUB64 RAX, RSI
    MOV RSI, RAX
signed_div_b_done:

    ; unsigned division
    DIV RAX, RDI, RSI

    ; apply sign
    CMP RBX, 0
    JZ signed_div_done
    LOAD_IMM RDX, 0
    SUB64 RDX, RAX
    MOV RAX, RDX

signed_div_done:
    RET

; ==========================================
; Parsing helpers
; ==========================================

skip_spaces:
    ; Input/Output: RSI = cursor
skip_spaces_loop:
    LOADR RAX, RSI
    CMP RAX, 32          ; ' '
    JZ skip_spaces_inc
    CMP RAX, 9           ; '\t'
    JZ skip_spaces_inc
    RET
skip_spaces_inc:
    INC RSI
    JMP skip_spaces_loop

parse_signed_int:
    ; Input/Output: RSI = cursor
    ; Output: RAX = parsed int
    ; Accepts optional leading '-'
    LOAD_IMM RAX, 0
    LOAD_IMM RDX, 0      ; sign flag: 0=positive, 1=negative

    LOADR RCX, RSI
    CMP RCX, 45          ; '-'
    JNZ parse_int_digits
    LOAD_IMM RDX, 1
    INC RSI

parse_int_digits:
    LOADR RCX, RSI

    ; stop on newline, NUL, or space
    CMP RCX, 10
    JZ parse_int_done
    CMP RCX, 0
    JZ parse_int_done
    CMP RCX, 32
    JZ parse_int_done

    ; digit check
    CMP RCX, 48
    JL parse_int_done
    CMP RCX, 57
    JG parse_int_done

    ; digit = RCX - '0'
    LOAD_IMM RBP, 48
    SUB64 RCX, RBP

    ; RAX = RAX*10 + digit
    LOAD_IMM RBP, 10
    MUL64 RAX, RAX, RBP
    ADD RAX, RCX

    INC RSI
    JMP parse_int_digits

parse_int_done:
    CMP RDX, 0
    JZ parse_int_ret

    ; negate
    LOAD_IMM RBP, 0
    SUB64 RBP, RAX
    MOV RAX, RBP

parse_int_ret:
    RET

; ==========================================
; itoa (unsigned)
; Input: RAX = integer value (>=0), RDI = destination buffer
; Output: RSI = length of string
; ==========================================
itoa_u64:
    LOAD_IMM RSI, 0
    LOAD_IMM RBP, 10

    CMP RAX, 0
    JNZ itoa_u64_convert
    LOAD_IMM RBX, 48
    STORER RDI, RBX
    LOAD_IMM RSI, 1
    RET

itoa_u64_convert:
    MOV RBX, RDI          ; start

itoa_u64_digit_loop:
    DIV RAX, RAX, RBP     ; quotient in RAX, remainder in RDX
    LOAD_IMM RCX, 48
    ADD RDX, RCX
    STORER RDI, RDX
    INC RDI
    INC RSI

    CMP RAX, 0
    JNZ itoa_u64_digit_loop

    ; reverse in place
    MOV RCX, RBX
    DEC RDI

itoa_u64_reverse_loop:
    CMP RCX, RDI
    JGE itoa_u64_done

    LOADR RAX, RCX
    LOADR RBX, RDI
    STORER RCX, RBX
    STORER RDI, RAX

    INC RCX
    DEC RDI
    JMP itoa_u64_reverse_loop

itoa_u64_done:
    RET
