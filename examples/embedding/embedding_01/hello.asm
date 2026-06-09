_start:
    ; sys_write(fd=1, buf=msg, count=16)
    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, msg
    LOAD_IMM RDX, 16
    INT 0x80

    ; sys_exit(0)
    LOAD_IMM RAX, 1
    LOAD_IMM RBX, 0
    INT 0x80

    HALT

msg: DB "Hello Assembly!", 10
