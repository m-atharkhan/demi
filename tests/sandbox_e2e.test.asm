; Sandbox End-to-End Integration Tests
; Run with:  demi-engine --sandbox -at tests/sandbox_e2e.test.asm

.test "sandbox_open_blocked" {
    .description "SYS_OPEN returns error in sandbox (VFS jail active)"
    .author "Security Audit 2026-06-09"
    .category "Sandbox"

    LOAD_IMM EAX, 0x0000782F   ; "/x\0\0" for filename
    STORE EAX, 0x80

    LOAD_IMM EAX, 5            ; SYS_OPEN
    LOAD_IMM EBX, 0x80         ; path
    LOAD_IMM ECX, 0            ; O_RDONLY
    LOAD_IMM EDX, 0            ; mode
    INT 0x80

    ; In sandbox, EAX should be 0xFFFFFFFE (-EACCES = -13 as uint32)
    HALT

    .assert_reg EAX, 0xFFFFFFFE
}

.test "sandbox_fork_blocked" {
    .description "SYS_FORK returns error in sandbox"
    .author "Security Audit 2026-06-09"
    .category "Sandbox"

    LOAD_IMM EAX, 2            ; SYS_FORK
    INT 0x80

    ; In sandbox, EAX should be 0xFFFFFFF3 (-EACCES)
    HALT

    .assert_reg EAX, 0xFFFFFFF3
}

.test "sandbox_write_stdout" {
    .description "SYS_WRITE to stdout fd=1 works in sandbox"
    .author "Security Audit 2026-06-09"
    .category "Sandbox"

    LOAD_IMM EAX, 0x0A4B4F    ; "OK\n"
    STORE EAX, 0x80

    LOAD_IMM EAX, 4            ; SYS_WRITE
    LOAD_IMM EBX, 1            ; fd=stdout
    LOAD_IMM ECX, 0x80         ; buf
    LOAD_IMM EDX, 3            ; len
    INT 0x80

    ; Should return 3 bytes written
    HALT

    .assert_reg EAX, 3
}

.test "sandbox_exit_stops" {
    .description "SYS_EXIT stops VM and sets RAX=0"
    .author "Security Audit 2026-06-09"
    .category "Sandbox"

    LOAD_IMM EAX, 1            ; SYS_EXIT
    LOAD_IMM EBX, 0            ; code 0
    INT 0x80

    ; VM stops here. RAX set to 0 by handle_syscall.
    HALT

    .assert_reg EAX, 0
}
