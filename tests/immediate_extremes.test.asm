; Test: Extreme Immediate Value Edge Cases
; Category: Operands
; Description: Test boundary values and extreme immediates

.test "load immediate zero" {
    .description "Load zero immediate"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0
    .assert_reg R0, 0
    halt
}

.test "load immediate one" {
    .description "Load one immediate"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "load immediate max 32-bit" {
    .description "Load maximum 32-bit value"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0xFFFFFFFF
    .assert_reg R0, 0xFFFFFFFF
    halt
}

.test "load immediate max signed positive" {
    .description "Load maximum positive signed 32-bit value"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0x7FFFFFFF
    .assert_reg R0, 0x7FFFFFFF
    halt
}

.test "load immediate min signed negative" {
    .description "Load minimum negative signed 32-bit value"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0x80000000
    .assert_reg R0, 0x80000000
    halt
}

.test "load immediate powers of 2" {
    .description "Load various powers of 2"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 4
    LOAD_IMM R3, 8
    LOAD_IMM R4, 16
    LOAD_IMM R5, 32
    LOAD_IMM R6, 64
    LOAD_IMM R7, 128
    LOAD_IMM R8, 256
    LOAD_IMM R9, 512
    LOAD_IMM R10, 1024
    halt
    .assert_reg R0, 1
    .assert_reg R1, 2
    .assert_reg R2, 4
    .assert_reg R3, 8
    .assert_reg R4, 16
    .assert_reg R5, 32
    .assert_reg R6, 64
    .assert_reg R7, 128
    .assert_reg R8, 256
    .assert_reg R9, 512
    .assert_reg R10, 1024
}

.test "load immediate alternating bits" {
    .description "Load alternating bit patterns"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0xAAAAAAAA
    LOAD_IMM R1, 0x55555555
    halt
    .assert_reg R0, 2863311530
    .assert_reg R1, 1431655765
}

.test "load immediate byte boundaries" {
    .description "Load values at byte boundaries"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0xFF
    LOAD_IMM R1, 0x100
    LOAD_IMM R2, 0xFFFF
    LOAD_IMM R3, 0x10000
    LOAD_IMM R4, 0xFFFFFF
    LOAD_IMM R5, 0x1000000
    halt
    .assert_reg R0, 255
    .assert_reg R1, 256
    .assert_reg R2, 65535
    .assert_reg R3, 65536
    .assert_reg R4, 16777215
    .assert_reg R5, 16777216
}

.test "load 64-bit immediate zero" {
    .description "Load 64-bit zero"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM64 R0, 0
    .assert_reg R0, 0
    .assert_reg R1, 0
    halt
}

.test "load 64-bit immediate max" {
    .description "Load maximum 64-bit value"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM64 R0, 0xFFFFFFFFFFFFFFFF
    .assert_reg R0, -1
    .assert_reg R1, -1
    halt
}

.test "load 64-bit immediate crossing boundary" {
    .description "Load 64-bit value that crosses 32-bit boundary"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM64 R0, 0x00000001FFFFFFFF
    halt
    .assert_reg R0, 8589934591
}

.test "load 64-bit immediate upper half only" {
    .description "Load 64-bit value with only upper 32 bits set"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM64 R0, 0x0000000100000000
    halt
    .assert_reg R0, 0
}

.test "immediate in all formats - decimal" {
    .description "Load immediate in decimal format"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 42
    .assert_reg R0, 42
    halt
}

.test "immediate in all formats - hex" {
    .description "Load immediate in hexadecimal format"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0x2A
    .assert_reg R0, 42
    halt
}

.test "immediate in all formats - binary" {
    .description "Load immediate in binary format"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0b101010
    .assert_reg R0, 42
    halt
}

.test "immediate in all formats - octal" {
    .description "Load immediate in octal format"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0o52
    .assert_reg R0, 42
    halt
}

.test "immediate value consistency" {
    .description "Same value in different formats produces same result"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 255
    LOAD_IMM R1, 0xFF
    LOAD_IMM R2, 0b11111111
    LOAD_IMM R3, 0o377
    
    CMP R0, R1
    JNE fail
    CMP R0, R2
    JNE fail
    CMP R0, R3
    JNE fail
    
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
    
fail:
    HALT
}

.test "large immediate values in arithmetic" {
    .description "Use large immediates in arithmetic operations"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 0x7FFFFFFF
    LOAD_IMM R1, 1
    ADD R0, R1
    .assert_reg R0, 0x80000000
    halt
}

.test "immediate zero in all operations" {
    .description "Zero immediate behaves correctly in all operations"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 42
    ADD R1, R0
    .assert_reg R1, 42
    
    LOAD_IMM R2, 42
    MUL R2, R0
    .assert_reg R2, 0
    
    LOAD_IMM R3, 42
    AND R3, R0
    .assert_reg R3, 0
    
    halt
}
