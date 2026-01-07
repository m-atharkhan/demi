; ==========================================
; MOV64 Opcode Test Suite
; Tests 64-bit register-to-register moves
; ==========================================

.test "mov64_basic_legacy" {
    .description "Tests basic move between legacy registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "basic"
    
    LOAD_IMM RAX, 42
    MOV64 RBX, RAX
    
    .assert_reg RBX, 42
}

.test "mov64_extended_to_extended" {
    .description "Tests move between extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "extended"
    
    LOAD_IMM R8, 12345
    MOV64 R9, R8
    
    .assert_reg R9, 12345
}

.test "mov64_legacy_to_extended" {
    .description "Tests move from legacy to extended register"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "mixed"
    
    LOAD_IMM RCX, 99999
    MOV64 R10, RCX
    
    .assert_reg R10, 99999
}

.test "mov64_extended_to_legacy" {
    .description "Tests move from extended to legacy register"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "mixed"
    
    LOAD_IMM R11, 55555
    MOV64 RDX, R11
    
    .assert_reg RDX, 55555
}

.test "mov64_zero" {
    .description "Tests moving zero value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "zero"
    
    LOAD_IMM R12, 0
    MOV64 R13, R12
    
    .assert_reg R13, 0
}

.test "mov64_max_value" {
    .description "Tests moving maximum 32-bit value"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "max"
    
    LOAD_IMM R14, 0xFFFFFFFF
    MOV64 R15, R14
    
    .assert_reg R15, 0xFFFFFFFF
}

.test "mov64_chain" {
    .description "Tests chain of moves"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "chain"
    
    LOAD_IMM RAX, 777
    MOV64 RBX, RAX
    MOV64 RCX, RBX
    MOV64 RDX, RCX
    
    .assert_reg RDX, 777
}

.test "mov64_overwrite" {
    .description "Tests overwriting destination register"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "overwrite"
    
    LOAD_IMM R8, 100
    LOAD_IMM R9, 200
    MOV64 R8, R9
    
    .assert_reg R8, 200
}

.test "mov64_preserve_source" {
    .description "Tests that source register is preserved"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "preserve"
    
    LOAD_IMM R10, 888
    MOV64 R11, R10
    
    .assert_reg R10, 888  ; Source unchanged
    .assert_reg R11, 888  ; Destination updated
}

.test "mov64_power_of_two" {
    .description "Tests moving powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mov64"
    .tag "power2"
    
    LOAD_IMM R12, 1024
    MOV64 R13, R12
    MOV64 R14, R13
    
    .assert_reg R14, 1024
}
