; ==========================================
; File Read Test (x64 64-bit)
; ==========================================
; Opens /tmp/demi_input.txt and reads its contents
; Uses 64-bit registers with .data and .text sections

.data
    filename: DB "/tmp/demi_input.txt", 0
    buffer: RESB 50

.text
_start:
    ; sys_open - open the file for reading
    LOAD_IMM RAX, 5         ; sys_open (syscall 5)
    LOAD_IMM RBX, filename  ; filename address
    LOAD_IMM RCX, 0         ; O_RDONLY (0)
    LOAD_IMM RDX, 0         ; mode (not used for read)
    INT 0x80                ; Call kernel - returns fd in RAX
    
    ; Save file descriptor
    MOV RSI, RAX            ; RSI = fd
    
    ; sys_read - read file contents into buffer
    LOAD_IMM RAX, 3         ; sys_read (syscall 3)
    MOV RBX, RSI            ; fd
    LOAD_IMM RCX, buffer    ; buffer address
    LOAD_IMM RDX, 50        ; max bytes to read
    INT 0x80                ; Call kernel - returns bytes read in RAX
    
    ; Save bytes read
    MOV RDI, RAX            ; RDI = bytes_read
    
    ; sys_write - write to stdout
    LOAD_IMM RAX, 4         ; sys_write (syscall 4)
    LOAD_IMM RBX, 1         ; stdout
    LOAD_IMM RCX, buffer    ; buffer (same as read buffer)
    MOV RDX, RDI            ; count = bytes_read
    INT 0x80                ; Call kernel
    
    ; sys_close
    LOAD_IMM RAX, 6         ; sys_close (syscall 6)
    MOV RBX, RSI            ; fd
    INT 0x80                ; Call kernel
    
    ; sys_exit
    LOAD_IMM RAX, 1         ; sys_exit
    LOAD_IMM RBX, 0         ; exit code 0
    INT 0x80
    
    HALT
