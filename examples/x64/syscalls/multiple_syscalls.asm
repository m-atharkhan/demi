; ==========================================
; Multiple System Calls Demo (x64 64-bit)
; ==========================================
; Demonstrates REAL Linux syscalls via INT 0x80
; Shows sys_write to stdout/stderr and sys_close
; Uses 64-bit registers with .data and .text sections

.data
    msg1: DB "Using REAL Linux syscalls from DemiEngine VM!", 10
    msg2: DB "[stderr] This is a real write()!", 10
    msg3: DB "All syscalls were authentic!", 10

.text
_start:
    ; Example 1: sys_write to stdout
    LOAD_IMM RAX, 4         ; sys_write (syscall number 4)
    LOAD_IMM RBX, 1         ; fd = stdout
    LOAD_IMM RCX, msg1      ; buffer address
    LOAD_IMM RDX, 47        ; message length
    INT 0x80                ; REAL KERNEL SYSCALL
    
    ; Example 2: Write to stderr  
    LOAD_IMM RAX, 4         ; sys_write
    LOAD_IMM RBX, 2         ; fd = stderr
    LOAD_IMM RCX, msg2      ; buffer address
    LOAD_IMM RDX, 34        ; message length  
    INT 0x80                ; REAL KERNEL SYSCALL
    
    ; Example 3: Close a file descriptor (safe to call on invalid fd)
    LOAD_IMM RAX, 6         ; sys_close (syscall number 6)
    LOAD_IMM RBX, 100       ; fd = 100 (doesn't exist, will return EBADF)
    INT 0x80                ; REAL KERNEL SYSCALL
    ; RAX now contains -9 (EBADF error code)
    
    ; Final message
    LOAD_IMM RAX, 4         ; sys_write
    LOAD_IMM RBX, 1         ; stdout
    LOAD_IMM RCX, msg3      ; buffer
    LOAD_IMM RDX, 29        ; length
    INT 0x80
    
    ; sys_exit (stops VM)
    LOAD_IMM RAX, 1         ; sys_exit (syscall number 1)
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80                ; Stop VM execution
    
    HALT
