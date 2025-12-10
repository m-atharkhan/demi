; Mode Switching and Mode-Aware Operations Tests
; Tests CPU mode transitions and mode-specific behavior

.test "default 32-bit mode" {
    .description "Test that CPU starts in 32-bit mode by default"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "default"
    
    ; Simple operation in default mode
    LOAD_IMM EAX, 100
    HALT
    
    .assert_reg EAX, 100
}

.test "explicit 32-bit mode" {
    .description "Test explicit MODE32 instruction"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    
    MODE32
    LOAD_IMM EAX, 0x12345678
    HALT
    
    .assert_reg EAX, 0x12345678
}

.test "switch to 64-bit mode" {
    .description "Test switching from 32-bit to 64-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode64"
    .tag "switch"
    
    MODE32
    MODE64
    LOAD_IMM RAX, 255
    HALT
    
    .assert_reg RAX, 255
}

.test "switch back to 32-bit" {
    .description "Test switching from 64-bit back to 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "switch"
    
    MODE64
    MODE32
    LOAD_IMM EAX, 42
    HALT
    
    .assert_reg EAX, 42
}

.test "32-bit add with overflow" {
    .description "Test 32-bit addition overflow wraps correctly"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "overflow"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0xFFFFFFFF
    LOAD_IMM EBX, 1
    ADD EAX, EBX
    HALT
    
    ; 0xFFFFFFFF + 1 = 0 (overflow wrap)
    .assert_reg EAX, 0
}

.test "32-bit sub with underflow" {
    .description "Test 32-bit subtraction underflow wraps correctly"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "underflow"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 1
    SUB EAX, EBX
    HALT
    
    ; 0 - 1 = 0xFFFFFFFF (underflow wrap)
    .assert_reg EAX, 0xFFFFFFFF
}

.test "32-bit max positive value" {
    .description "Test loading max positive 32-bit signed value"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "boundary"
    
    MODE32
    LOAD_IMM EAX, 0x7FFFFFFF
    HALT
    
    .assert_reg EAX, 0x7FFFFFFF
}

.test "32-bit min negative value" {
    .description "Test loading min negative 32-bit signed value"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "boundary"
    
    MODE32
    LOAD_IMM EAX, 0x80000000
    HALT
    
    .assert_reg EAX, 0x80000000
}

.test "32-bit shift left boundary" {
    .description "Test shift left to sign bit position"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "shift"
    .tag "boundary"
    
    MODE32
    LOAD_IMM EAX, 1
    SHL EAX, 31
    HALT
    
    .assert_reg EAX, 0x80000000
}

.test "32-bit shift right boundary" {
    .description "Test shift right from sign bit position"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "shift"
    .tag "boundary"
    
    MODE32
    LOAD_IMM EAX, 0x80000000
    SHR EAX, 31
    HALT
    
    .assert_reg EAX, 1
}

.test "64-bit large value load" {
    .description "Test loading large 64-bit values"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode64"
    .tag "large_value"
    
    MODE64
    LOAD_IMM64 RAX, 0x123456789ABCDEF0
    HALT
    
    .assert_reg64 RAX, 0x123456789ABCDEF0
}

.test "64-bit addition no wrap" {
    .description "Test 64-bit addition without 32-bit wrap"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode64"
    .tag "arithmetic"
    
    MODE64
    LOAD_IMM64 RAX, 0x00000000FFFFFFFF
    LOAD_IMM64 RBX, 1
    ADD RAX, RBX
    HALT
    
    ; Should be 0x100000000, not 0 as in 32-bit mode
    .assert_reg64 RAX, 0x0000000100000000
}

.test "mode switch preserves lower bits" {
    .description "Test that mode switching preserves register lower bits"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "mode64"
    .tag "switch"
    
    MODE32
    LOAD_IMM EAX, 0xDEADBEEF
    MODE64
    ; Lower 32 bits should still contain 0xDEADBEEF
    HALT
    
    ; This tests that mode switch doesn't clear registers
    .assert_reg EAX, 0xDEADBEEF
}

.test "multiple mode switches" {
    .description "Test multiple consecutive mode switches"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "mode64"
    .tag "switch"
    
    MODE32
    LOAD_IMM EAX, 1
    MODE64
    LOAD_IMM RBX, 2
    MODE32
    LOAD_IMM ECX, 3
    MODE64
    LOAD_IMM RDX, 4
    HALT
    
    .assert_reg EAX, 1
    .assert_reg EBX, 2
    .assert_reg ECX, 3
    .assert_reg EDX, 4
}

.test "32-bit AND operation" {
    .description "Test bitwise AND in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "bitwise"
    
    MODE32
    LOAD_IMM EAX, 0xFF00FF00
    LOAD_IMM EBX, 0x0F0F0F0F
    AND EAX, EBX
    HALT
    
    .assert_reg EAX, 0x0F000F00
}

.test "32-bit OR operation" {
    .description "Test bitwise OR in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "bitwise"
    
    MODE32
    LOAD_IMM EAX, 0xF0F0F0F0
    LOAD_IMM EBX, 0x0F0F0F0F
    OR EAX, EBX
    HALT
    
    .assert_reg EAX, 0xFFFFFFFF
}

.test "32-bit XOR self zeroing" {
    .description "Test XOR register with itself to zero"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "bitwise"
    
    MODE32
    LOAD_IMM EAX, 0xCAFEBABE
    XOR EAX, EAX
    HALT
    
    .assert_reg EAX, 0
}

.test "32-bit NOT operation" {
    .description "Test bitwise NOT in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "bitwise"
    
    MODE32
    LOAD_IMM EAX, 0x55555555
    NOT EAX
    HALT
    
    .assert_reg EAX, 0xAAAAAAAA
}

.test "32-bit INC at boundary" {
    .description "Test INC at 32-bit maximum boundary"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "boundary"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0xFFFFFFFF
    INC EAX
    HALT
    
    .assert_reg EAX, 0
}

.test "32-bit DEC at zero" {
    .description "Test DEC at zero (underflow)"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "boundary"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0
    DEC EAX
    HALT
    
    .assert_reg EAX, 0xFFFFFFFF
}

.test "32-bit division" {
    .description "Test integer division in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 7
    DIV EAX, EBX
    HALT
    
    .assert_reg EAX, 14
}

.test "32-bit modulo" {
    .description "Test modulo operation in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 17
    LOAD_IMM EBX, 5
    MOD EAX, EBX
    HALT
    
    .assert_reg EAX, 2
}

.test "32-bit multiplication" {
    .description "Test multiplication in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 1000
    LOAD_IMM EBX, 500
    MUL EAX, EBX
    HALT
    
    .assert_reg EAX, 500000
}

.test "32-bit multiplication overflow" {
    .description "Test multiplication overflow in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "overflow"
    .tag "arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0x10000
    LOAD_IMM EBX, 0x10000
    MUL EAX, EBX
    HALT
    
    ; 0x10000 * 0x10000 = 0x100000000 truncated to 32 bits = 0
    .assert_reg EAX, 0
}

.test "compare equal values 32-bit" {
    .description "Test CMP with equal values in 32-bit mode"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode32"
    .tag "compare"
    
    MODE32
    LOAD_IMM EAX, 0x12345678
    LOAD_IMM EBX, 0x12345678
    CMP EAX, EBX
    ; Zero flag should be set, registers unchanged
    HALT
    
    .assert_reg EAX, 0x12345678
    .assert_reg EBX, 0x12345678
}

.test "64-bit boundary values" {
    .description "Test 64-bit mode with boundary values"
    .author "bobrossrtx"
    .category "Mode Awareness"
    .tag "mode64"
    .tag "boundary"
    
    MODE64
    LOAD_IMM64 RAX, 0x7FFFFFFFFFFFFFFF  ; Max signed 64-bit
    LOAD_IMM64 RBX, 0x8000000000000000  ; Min signed 64-bit
    LOAD_IMM64 RCX, 0xFFFFFFFFFFFFFFFF  ; Max unsigned 64-bit
    HALT
    
    .assert_reg64 RAX, 0x7FFFFFFFFFFFFFFF
    .assert_reg64 RBX, 0x8000000000000000
    .assert_reg64 RCX, 0xFFFFFFFFFFFFFFFF
}
