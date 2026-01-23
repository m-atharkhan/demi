; Test: Exhaustive Register Coverage
; Category: Registers
; Description: Test all register combinations and aliases

.test "load immediate to all base registers R0-R7" {
    .description "Load different values to all base registers"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 101
    LOAD_IMM R2, 102
    LOAD_IMM R3, 103
    LOAD_IMM R4, 104
    LOAD_IMM R5, 105
    LOAD_IMM R6, 106
    LOAD_IMM R7, 107
    
    .assert_reg R0, 100
    .assert_reg R1, 101
    .assert_reg R2, 102
    .assert_reg R3, 103
    .assert_reg R4, 104
    .assert_reg R5, 105
    .assert_reg R6, 106
    .assert_reg R7, 107
    halt
}

.test "MOV between R0-R7 pairs" {
    .description "Move values between all base register pairs"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    LOAD_IMM R3, 4
    
    MOV R4, R0
    MOV R5, R1
    MOV R6, R2
    MOV R7, R3
    
    .assert_reg R4, 1
    .assert_reg R5, 2
    .assert_reg R6, 3
    .assert_reg R7, 4
    halt
}

.test "ADD between register combinations" {
    .description "Add between various register pairs"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1
    .assert_reg R0, 30
    
    LOAD_IMM R2, 5
    LOAD_IMM R3, 15
    ADD R2, R3
    .assert_reg R2, 20
    
    LOAD_IMM R4, 100
    LOAD_IMM R5, 200
    ADD R4, R5
    .assert_reg R4, 300
    halt
}

.test "load immediate to all x64 aliases" {
    .description "Load to all x64 register aliases (RAX, RBX, etc.)"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM RAX, 200
    LOAD_IMM RBX, 201
    LOAD_IMM RCX, 202
    LOAD_IMM RDX, 203
    LOAD_IMM RSI, 204
    LOAD_IMM RDI, 205
    LOAD_IMM RBP, 206
    LOAD_IMM RSP, 207
    
    .assert_reg RAX, 200
    .assert_reg RBX, 201
    .assert_reg RCX, 202
    .assert_reg RDX, 203
    .assert_reg RSI, 204
    .assert_reg RDI, 205
    .assert_reg RBP, 206
    .assert_reg RSP, 207
    halt
}

.test "load immediate to all 32-bit aliases" {
    .description "Load to all 32-bit register aliases (EAX, EBX, etc.)"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM EAX, 300
    LOAD_IMM EBX, 301
    LOAD_IMM ECX, 302
    LOAD_IMM EDX, 303
    LOAD_IMM ESI, 304
    LOAD_IMM EDI, 305
    LOAD_IMM EBP, 306
    LOAD_IMM ESP, 307
    
    .assert_reg EAX, 300
    .assert_reg EBX, 301
    .assert_reg ECX, 302
    .assert_reg EDX, 303
    .assert_reg ESI, 304
    .assert_reg EDI, 305
    .assert_reg EBP, 306
    .assert_reg ESP, 307
    halt
}

.test "MOV between x64 and base registers" {
    .description "Move between x64 aliases and base register names"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 42
    MOV RAX, R0
    .assert_reg RAX, 42
    
    LOAD_IMM RBX, 99
    MOV R1, RBX
    .assert_reg R1, 99
    halt
}

.test "register preservation across operations" {
    .description "Verify non-target registers are preserved"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 1
    LOAD_IMM R1, 2
    LOAD_IMM R2, 3
    
    ADD R0, R1
    .assert_reg R1, 2
    .assert_reg R2, 3
    halt
}

.test "64-bit register pairs R0-R1, R2-R3, R4-R5, R6-R7" {
    .description "Load 64-bit values to registers"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM64 R0, 0x0000000100000002
    LOAD_IMM64 R2, 0x0000000300000004
    LOAD_IMM64 R4, 0x0000000500000006
    LOAD_IMM64 R6, 0x0000000700000008
    halt
    .assert_reg R0, 2
    .assert_reg R2, 4
    .assert_reg R4, 6
    .assert_reg R6, 8
}

.test "extended registers R8-R15 basic operations" {
    .description "Use extended registers R8-R15"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R8, 800
    LOAD_IMM R9, 900
    LOAD_IMM R10, 1000
    LOAD_IMM R11, 1100
    
    .assert_reg R8, 800
    .assert_reg R9, 900
    .assert_reg R10, 1000
    .assert_reg R11, 1100
    halt
}

.test "MOVEX between extended registers" {
    .description "Move between R8-R15 using MOVEX"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R8, 100
    LOAD_IMM R9, 200
    MOVEX R10, R8
    MOVEX R11, R9
    
    .assert_reg R10, 100
    .assert_reg R11, 200
    halt
}

.test "ADDEX with extended registers" {
    .description "Add extended registers using ADDEX"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R8, 100
    LOAD_IMM R9, 200
    ADDEX R8, R9
    .assert_reg R8, 300
    halt
}

.test "SUBEX with extended registers" {
    .description "Subtract extended registers using SUBEX"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R8, 500
    LOAD_IMM R9, 200
    SUBEX R8, R9
    .assert_reg R8, 300
    halt
}

.test "XMM registers XMM0-XMM3 basic operations" {
    .description "Use XMM registers XMM0-XMM3"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM64 XMM0, 0x1111111111111111
    LOAD_IMM64 XMM0_HIGH, 0x2222222222222222
    
    LOAD_IMM64 XMM1, 0x3333333333333333
    LOAD_IMM64 XMM1_HIGH, 0x4444444444444444
    
    .assert_reg XMM0, 0x1111111111111111
    .assert_reg XMM0_HIGH, 0x2222222222222222
    .assert_reg XMM1, 0x3333333333333333
    .assert_reg XMM1_HIGH, 0x4444444444444444
    halt
}

.test "MOVAPS between XMM registers" {
    .description "Move between XMM registers using MOVAPS"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM64 XMM0, 0xAAAAAAAAAAAAAAAA
    LOAD_IMM64 XMM0_HIGH, 0xBBBBBBBBBBBBBBBB
    
    MOVAPS XMM1, XMM0
    
    .assert_reg XMM1, 0xAAAAAAAAAAAAAAAA
    .assert_reg XMM1_HIGH, 0xBBBBBBBBBBBBBBBB
    halt
}

.test "all register types in same test" {
    .description "Use base, extended, and XMM registers together"
    .author "copilot"
    .category "Registers"
    
    LOAD_IMM R0, 1
    LOAD_IMM R8, 8
    LOAD_IMM64 XMM0, 0x00000000000000FF
    
    .assert_reg R0, 1
    .assert_reg R8, 8
    .assert_reg XMM0, 0xFF
    halt
}
