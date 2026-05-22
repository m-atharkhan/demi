_start:
    LOAD_IMM RAX, 4         ; syscall id
    LOAD_IMM RBX, 0x1041    ; Use the register the VM is actually picking up (a2)
    LOAD_IMM RCX, 16        ; Use the register the VM is actually picking up (a3)
    INT 0x80
    
    ; sys_exit
    LOAD_IMM RAX, 1         ; sys_exit
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80
    
    HALT

msg: DB "Hello Assembly!", 10