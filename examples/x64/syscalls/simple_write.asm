; ==========================================
; Simple System Write (x64 64-bit)
; ==========================================
; Simple test - write to stdout to verify syscalls work
; Uses 64-bit registers

_start:
    ; sys_write: write to stdout
    LOAD_IMM RAX, 4         ; sys_write
    LOAD_IMM RBX, 1         ; stdout (fd=1)
    LOAD_IMM RCX, 15        ; buffer address
    LOAD_IMM RDX, 14        ; count
    INT 0x80
    
    ; sys_exit
    LOAD_IMM RAX, 1         ; sys_exit
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80
    
    HALT

    ; Message data
    DB "Hello, World!", 10
