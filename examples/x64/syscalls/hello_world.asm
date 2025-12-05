; ==========================================
; System Call: Hello World (x64 64-bit)
; ==========================================
; Hello World using Linux syscall conventions (INT 0x80)
; Uses 64-bit registers following x64 calling convention

.org 0x00

_start:
    ; sys_write(stdout, message, length)
    LOAD_IMM RAX, 4         ; syscall 4 = sys_write
    LOAD_IMM RBX, 1         ; fd = 1 (stdout)
    LOAD_IMM RCX, msg       ; buffer address
    LOAD_IMM RDX, 14        ; message length
    INT 0x80                ; Call kernel
    
    ; sys_exit(0)
    LOAD_IMM RAX, 1         ; syscall 1 = sys_exit
    LOAD_IMM RBX, 0         ; exit code = 0
    INT 0x80                ; Call kernel
    
    HALT                    ; Should not reach here

msg:
    DB "Hello, World!", 10  ; Message with newline
