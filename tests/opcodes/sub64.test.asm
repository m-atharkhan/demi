; ==========================================
; SUB64 Opcode Test Suite
; Tests 64-bit subtraction with flag handling
; ==========================================

.test "sub64_basic" {
    .description "Tests basic 64-bit subtraction"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "basic"
    
    LOAD_IMM RAX, 20
    LOAD_IMM RCX, 8
    SUB64 RAX, RCX
    
    .assert_reg RAX, 12
}

.test "sub64_zero_result" {
    .description "Tests subtraction resulting in zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "zero"
    
    LOAD_IMM RBX, 100
    LOAD_IMM RDX, 100
    SUB64 RBX, RDX
    
    .assert_reg RBX, 0
}

.test "sub64_subtract_zero" {
    .description "Tests subtracting zero (no change)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "zero"
    
    LOAD_IMM R8, 42
    LOAD_IMM R9, 0
    SUB64 R8, R9
    
    .assert_reg R8, 42
}

.test "sub64_large_numbers" {
    .description "Tests subtraction of large numbers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "large"
    
    LOAD_IMM R10, 5000000
    LOAD_IMM R11, 2000000
    SUB64 R10, R11
    
    .assert_reg R10, 3000000
}

.test "sub64_self" {
    .description "Tests subtracting a register from itself (always zero)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "self"
    
    LOAD_IMM R12, 999
    SUB64 R12, R12
    
    .assert_reg R12, 0
}

.test "sub64_one" {
    .description "Tests subtracting one (decrement alternative)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "decrement"
    
    LOAD_IMM R13, 1000
    LOAD_IMM R14, 1
    SUB64 R13, R14
    
    .assert_reg R13, 999
}

.test "sub64_extended_registers" {
    .description "Tests SUB64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "extended"
    
    LOAD_IMM R15, 80235
    LOAD_IMM RAX, 12345
    SUB64 R15, RAX
    
    .assert_reg R15, 67890
}

.test "sub64_sequential" {
    .description "Tests sequential subtractions"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "sequential"
    
    LOAD_IMM RCX, 100
    LOAD_IMM RDX, 20
    SUB64 RCX, RDX
    SUB64 RCX, RDX
    SUB64 RCX, RDX
    
    .assert_reg RCX, 40
}

.test "sub64_power_of_two" {
    .description "Tests subtraction with powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "power2"
    
    LOAD_IMM R8, 512
    LOAD_IMM R9, 256
    SUB64 R8, R9
    
    .assert_reg R8, 256
}

.test "sub64_near_boundary" {
    .description "Tests subtraction near zero boundary"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "sub64"
    .tag "boundary"
    
    LOAD_IMM R10, 5
    LOAD_IMM R11, 3
    SUB64 R10, R11
    
    .assert_reg R10, 2
}
