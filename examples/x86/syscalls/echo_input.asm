; ==========================================
; Echo User Input (x86 32-bit)
; ==========================================
; Prompts user for input, reads from stdin, and echoes it back
; Demonstrates sys_read and sys_write syscalls
; Uses 32-bit registers with .data and .text sections

.data
    prompt: DB "Enter your message: ", 0
    prompt_len EQU 20       ; Define constant for prompt length
    newline: DB 10          ; Newline character
    buffer: RESB 128        ; Buffer for user input (128 bytes)

.text
_start:
    ; sys_write - display prompt
    LOAD_IMM EAX, 4         ; sys_write (syscall 4)
    LOAD_IMM EBX, 1         ; fd = 1 (stdout)
    LOAD_IMM ECX, prompt    ; buffer address
    LOAD_IMM EDX, prompt_len ; message length using EQU constant
    INT 0x80                ; Call kernel
    
    ; sys_read - read user input from stdin
    LOAD_IMM EAX, 3         ; sys_read (syscall 3)
    LOAD_IMM EBX, 0         ; fd = 0 (stdin)
    LOAD_IMM ECX, buffer    ; buffer address to store input
    LOAD_IMM EDX, 128       ; max bytes to read
    INT 0x80                ; Call kernel - returns bytes read in EAX
    
    ; Check if read was successful (EAX > 0)
    CMP EAX, 0              ; Compare with immediate 0
    JLE exit_program        ; If EAX <= 0, exit
    
    ; Save bytes read
    MOV ESI, EAX            ; ESI = bytes_read
    
    ; sys_write - echo the input back to stdout
    LOAD_IMM EAX, 4         ; sys_write (syscall 4)
    LOAD_IMM EBX, 1         ; fd = 1 (stdout)
    LOAD_IMM ECX, buffer    ; buffer address (same as read buffer)
    MOV EDX, ESI            ; count = bytes_read
    INT 0x80                ; Call kernel
    
    ; Print newline for cleaner output
    LOAD_IMM EAX, 4         ; sys_write (syscall 4)
    LOAD_IMM EBX, 1         ; fd = 1 (stdout)
    LOAD_IMM ECX, newline   ; newline character
    LOAD_IMM EDX, 1         ; 1 byte
    INT 0x80                ; Call kernel
    
exit_program:
    ; sys_exit - terminate program
    LOAD_IMM EAX, 1         ; sys_exit (syscall 1)
    LOAD_IMM EBX, 0         ; exit code = 0
    INT 0x80                ; Call kernel
    
    HALT                    ; Should not reach here
