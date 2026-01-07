; ==========================================
; ADD64 Opcode Test Suite
; Tests 64-bit addition with flag handling
; ==========================================

.test "add64_basic" {
    .description "Tests basic 64-bit addition"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "basic"
    
    LOAD_IMM RAX, 5
    LOAD_IMM RCX, 3
    ADD64 RAX, RCX
    
    .assert_reg RAX, 8
}

.test "add64_zero_operand" {
    .description "Tests addition with zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "zero"
    
    LOAD_IMM RBX, 42
    LOAD_IMM RDX, 0
    ADD64 RBX, RDX
    
    .assert_reg RBX, 42
}

.test "add64_both_zero" {
    .description "Tests addition of zero + zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "zero"
    
    LOAD_IMM R8, 0
    LOAD_IMM R9, 0
    ADD64 R8, R9
    
    .assert_reg R8, 0
}

.test "add64_large_numbers" {
    .description "Tests addition of large numbers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "large"
    
    LOAD_IMM R10, 1000000
    LOAD_IMM R11, 2000000
    ADD64 R10, R11
    
    .assert_reg R10, 3000000
}

.test "add64_self" {
    .description "Tests adding a register to itself (doubling)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "self"
    
    LOAD_IMM R12, 100
    ADD64 R12, R12
    
    .assert_reg R12, 200
}

.test "add64_extended_registers" {
    .description "Tests ADD64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "extended"
    
    LOAD_IMM R13, 12345
    LOAD_IMM R14, 67890
    ADD64 R13, R14
    
    .assert_reg R13, 80235
}

.test "add64_commutative" {
    .description "Tests commutative property (a+b = b+a)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "property"
    
    LOAD_IMM RAX, 15
    LOAD_IMM RBX, 7
    ADD64 RAX, RBX
    
    LOAD_IMM RCX, 7
    LOAD_IMM RDX, 15
    ADD64 RCX, RDX
    
    ; Both should equal 22
    .assert_reg RAX, 22
    .assert_reg RCX, 22
}

.test "add64_power_of_two" {
    .description "Tests addition with powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "power2"
    
    LOAD_IMM R15, 128
    LOAD_IMM RAX, 256
    ADD64 R15, RAX
    
    .assert_reg R15, 384
}

.test "add64_sequential" {
    .description "Tests sequential additions"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "sequential"
    
    LOAD_IMM R8, 10
    LOAD_IMM R9, 20
    ADD64 R8, R9
    LOAD_IMM R10, 30
    ADD64 R8, R10
    
    .assert_reg R8, 60
}

.test "add64_one" {
    .description "Tests adding one (increment alternative)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "add64"
    .tag "increment"
    
    LOAD_IMM R11, 999
    LOAD_IMM R12, 1
    ADD64 R11, R12
    
    .assert_reg R11, 1000
}
