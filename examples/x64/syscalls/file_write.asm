; ==========================================
; File Write Test (x64 64-bit)
; ==========================================
; Creates /tmp/demi_test.txt and writes to it
; Uses 64-bit registers with .data and .text sections

.data
    filename: DB "/tmp/demi_test.txt", 0
    message: DB "Hello from DemiEngine!", 10, 0
    success_msg: DB "File written successfully!", 10, 0

.text
_start:
    ; sys_open: open file for writing (create if doesn't exist)
    LOAD_IMM RAX, 5         ; sys_open (syscall number 5)
    LOAD_IMM RBX, filename  ; pathname address
    LOAD_IMM RCX, 65        ; flags: O_WRONLY | O_CREAT = 0x41
    LOAD_IMM RDX, 420       ; mode: 0644 (octal 0644 = 420 decimal)
    INT 0x80                ; Real syscall to kernel
    
    ; RAX now contains file descriptor
    MOV RDI, RAX            ; Save fd to RDI
    
    ; sys_write: write message to file
    LOAD_IMM RAX, 4         ; sys_write (syscall number 4)
    MOV RBX, RDI            ; fd (from RDI)
    LOAD_IMM RCX, message   ; buffer address
    LOAD_IMM RDX, 23        ; count
    INT 0x80                ; Real syscall to kernel
    
    ; sys_close: close the file
    LOAD_IMM RAX, 6         ; sys_close (syscall number 6)
    MOV RBX, RDI            ; fd
    INT 0x80                ; Real syscall to kernel
    
    ; sys_write: print success message to stdout
    LOAD_IMM RAX, 4         ; sys_write
    LOAD_IMM RBX, 1         ; stdout
    LOAD_IMM RCX, success_msg ; success message address
    LOAD_IMM RDX, 28        ; count
    INT 0x80
    
    ; sys_exit
    LOAD_IMM RAX, 1         ; sys_exit (syscall number 1)
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80                ; Stop VM
    
    HALT
