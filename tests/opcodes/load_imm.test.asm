; ==========================================
; LOAD_IMM Opcode Test Suite
; Tests immediate value loading to all registers
; Critical: Tests R8-R15 extended register support and 6-byte encoding
; ==========================================

.test "load_imm_legacy_registers" {
    .description "Tests LOAD_IMM with legacy registers R0-R7"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "basic"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 200
    LOAD_IMM R2, 300
    LOAD_IMM R3, 400
    
    .assert_reg R0, 100
    .assert_reg R1, 200
    .assert_reg R2, 300
    .assert_reg R3, 400
}

.test "load_imm_extended_registers_r8_r11" {
    .description "Tests LOAD_IMM with extended registers R8-R11 (THE BUG FIX!)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "extended"
    .tag "bugfix"
    
    ; This was broken before - 6-byte encoding fix
    LOAD_IMM R8, 500
    LOAD_IMM R9, 600
    LOAD_IMM R10, 700
    LOAD_IMM R11, 800
    
    .assert_reg R8, 500
    .assert_reg R9, 600
    .assert_reg R10, 700
    .assert_reg R11, 800
}

.test "load_imm_extended_registers_r12_r15" {
    .description "Tests LOAD_IMM with extended registers R12-R15"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "extended"
    
    LOAD_IMM R12, 900
    LOAD_IMM R13, 1000
    LOAD_IMM R14, 1100
    LOAD_IMM R15, 1200
    
    .assert_reg R12, 900
    .assert_reg R13, 1000
    .assert_reg R14, 1100
    .assert_reg R15, 1200
}

.test "load_imm_zero" {
    .description "Tests loading zero value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "zero"
    
    LOAD_IMM RAX, 0
    LOAD_IMM R8, 0
    
    .assert_reg RAX, 0
    .assert_reg R8, 0
}

.test "load_imm_max_32bit" {
    .description "Tests loading maximum 32-bit value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "max"
    
    LOAD_IMM RAX, 0xFFFFFFFF
    LOAD_IMM R9, 0xFFFFFFFF
    
    .assert_reg RAX, 0xFFFFFFFF
    .assert_reg R9, 0xFFFFFFFF
}

.test "load_imm_negative_as_unsigned" {
    .description "Tests loading negative numbers (interpreted as large unsigned)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "signed"
    
    ; -1 as unsigned is 0xFFFFFFFF
    LOAD_IMM RBX, 0xFFFFFFFE
    
    .assert_reg RBX, 0xFFFFFFFE
}

.test "load_imm_overwrite" {
    .description "Tests overwriting register value with LOAD_IMM"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "overwrite"
    
    LOAD_IMM RCX, 100
    LOAD_IMM RCX, 200
    LOAD_IMM RCX, 300
    
    .assert_reg RCX, 300
}

.test "load_imm_power_of_two" {
    .description "Tests loading powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "power2"
    
    LOAD_IMM R10, 1
    LOAD_IMM R11, 2
    LOAD_IMM R12, 4
    LOAD_IMM R13, 8
    LOAD_IMM R14, 16
    LOAD_IMM R15, 32
    
    .assert_reg R10, 1
    .assert_reg R11, 2
    .assert_reg R12, 4
    .assert_reg R13, 8
    .assert_reg R14, 16
    .assert_reg R15, 32
}

.test "load_imm_ascii_values" {
    .description "Tests loading ASCII character values (used in calculator)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "ascii"
    
    ; Test loading common ASCII values
    LOAD_IMM R8, 48    ; '0'
    LOAD_IMM R9, 43    ; '+'
    LOAD_IMM R10, 45   ; '-'
    LOAD_IMM R11, 42   ; '*'
    LOAD_IMM R12, 47   ; '/'
    
    .assert_reg R8, 48
    .assert_reg R9, 43
    .assert_reg R10, 45
    .assert_reg R11, 42
    .assert_reg R12, 47
}

.test "load_imm_sequential_pc_advance" {
    .description "Tests that PC advances correctly (6 bytes per LOAD_IMM)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "load_imm"
    .tag "pc"
    
    ; These should execute sequentially without PC misalignment
    LOAD_IMM RDX, 111
    LOAD_IMM RDX, 222
    LOAD_IMM RDX, 333
    LOAD_IMM RDX, 444
    
    .assert_reg RDX, 444
}
