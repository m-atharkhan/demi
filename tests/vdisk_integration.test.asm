; VirtualDisk Integration Tests
; Run with: demi-engine --sandbox -at tests/vdisk_integration.test.asm

.test "vdisk_sysopen_returns_fd" {
    .description "SYS_OPEN in sandbox returns a VirtualDisk fd (>= 1000)"
    .author "2026-06-09"
    .category "Sandbox"

    ; Store "test.txt\0" at 0x80
    LOAD_IMM EAX, 0x74736574   ; "test"
    STORE EAX, 0x80
    LOAD_IMM EAX, 0x00747874   ; ".txt\0"
    STORE EAX, 0x84

    ; SYS_OPEN("test.txt", O_CREAT|O_WRONLY, 0644)
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 0x80
    LOAD_IMM ECX, 65
    LOAD_IMM EDX, 420
    INT 0x80

    ; fd should be >= 1000 (VirtualDisk synthetic fd range)
    ; If sandbox is disabled, fd will be small (3 or so).
    HALT

    .assert_reg EAX, 1000
}

.test "vdisk_close_success" {
    .description "SYS_CLOSE on VirtualDisk fd returns 0"
    .author "2026-06-09"
    .category "Sandbox"

    ; Open a file first
    LOAD_IMM EAX, 0x74736574   ; "test"
    STORE EAX, 0x80
    LOAD_IMM EAX, 0x00747874   ; ".txt\0"
    STORE EAX, 0x84

    LOAD_IMM EAX, 5            ; SYS_OPEN
    LOAD_IMM EBX, 0x80
    LOAD_IMM ECX, 65
    LOAD_IMM EDX, 420
    INT 0x80

    ; EAX = fd (should be 1000+). Store for close.
    STORE EAX, 0x70

    ; SYS_CLOSE(fd)
    LOAD_IMM EAX, 6            ; SYS_CLOSE
    LOAD_IMM EBX, 1000         ; fd
    INT 0x80

    ; Should return 0
    HALT

    .assert_reg EAX, 0
}
