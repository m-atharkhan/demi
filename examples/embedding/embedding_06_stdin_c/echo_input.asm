; Echo input using sys_read/sys_write

.data
    prompt: DB "Enter your message: ", 0
    prompt_len EQU 20
    newline: DB 10
    buffer: RESB 128

.text
_start:
    ; sys_write(fd=1, buf=prompt, count=prompt_len)
    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, prompt
    LOAD_IMM RDX, prompt_len
    INT 0x80

    ; sys_read(fd=0, buf=buffer, count=128)
    LOAD_IMM RAX, 3
    LOAD_IMM RBX, 0
    LOAD_IMM RCX, buffer
    LOAD_IMM RDX, 128
    INT 0x80

    CMP RAX, 0
    JLE exit_program
    MOV RSI, RAX

    ; sys_write(fd=1, buf=buffer, count=bytes_read)
    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, buffer
    MOV RDX, RSI
    INT 0x80

    ; newline
    LOAD_IMM RAX, 4
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, newline
    LOAD_IMM RDX, 1
    INT 0x80

exit_program:
    LOAD_IMM RAX, 1
    LOAD_IMM RBX, 0
    INT 0x80
    HALT

