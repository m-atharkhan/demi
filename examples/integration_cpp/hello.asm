_start:
    ; sys_write: write to stdout
    LOAD_IMM RAX, 4         ; sys_write
    LOAD_IMM RBX, 1         ; stdout (fd=1)
    LOAD_IMM RCX, msg       ; buffer address
    LOAD_IMM RDX, 16        ; count (16 bytes)
    INT 0x80
    
    ; sys_exit
    LOAD_IMM RAX, 1         ; sys_exit
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80
    
    HALT

msg: DB "Hello Assembly!", 10
