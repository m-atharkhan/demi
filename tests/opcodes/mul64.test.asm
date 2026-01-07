; ==========================================
; MUL64 Opcode Test Suite
; Tests 64-bit multiplication
; ==========================================


.test "mul64_basic" {
    .description "Tests basic 64-bit multiplication"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "basic"
    
    LOAD_IMM RAX, 6
    LOAD_IMM RCX, 7
    MUL64 RAX, RAX, RCX
    
    .assert_reg RAX, 42
}

.test "mul64_by_zero" {
    .description "Tests multiplication by zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "zero"
    
    LOAD_IMM RBX, 999
    LOAD_IMM RDX, 0
    MUL64 RBX, RBX, RDX
    
    .assert_reg RBX, 0
}

.test "mul64_by_one" {
    .description "Tests multiplication by one (identity)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "identity"
    
    LOAD_IMM R8, 12345
    LOAD_IMM R9, 1
    MUL64 R8, R8, R9
    
    .assert_reg R8, 12345
}

.test "mul64_atoi_pattern" {
    .description "Tests the atoi pattern: result * 10"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "atoi"
    
    ; This is the pattern used in calculator's atoi function
    LOAD_IMM R10, 0
    LOAD_IMM R11, 10
    MUL64 R10, R10, R11
    
    .assert_reg R10, 0
}

.test "mul64_larger_multiplication" {
    .description "Tests larger multiplication"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "large"
    
    LOAD_IMM R12, 123
    LOAD_IMM R13, 456
    MUL64 R12, R12, R13
    
    .assert_reg R12, 56088
}

.test "mul64_power_of_two" {
    .description "Tests multiplication by powers of 2"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "power2"
    
    LOAD_IMM R14, 128
    LOAD_IMM R15, 256
    MUL64 R14, R14, R15
    
    .assert_reg R14, 32768
}

.test "mul64_commutative" {
    .description "Tests commutative property (a*b = b*a)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "property"
    
    LOAD_IMM RAX, 7
    LOAD_IMM RBX, 11
    MUL64 RAX, RAX, RBX
    
    LOAD_IMM RCX, 11
    LOAD_IMM RDX, 7
    MUL64 RCX, RCX, RDX
    
    ; Both should equal 77
    .assert_reg RAX, 77
    .assert_reg RCX, 77
}

.test "mul64_small_factors" {
    .description "Tests multiplication of small factors"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "small"
    
    LOAD_IMM R8, 2
    LOAD_IMM R9, 3
    MUL64 R8, R8, R9
    
    .assert_reg R8, 6
}

.test "mul64_by_ten" {
    .description "Tests multiplication by 10 (common in number conversion)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "decimal"
    
    LOAD_IMM R10, 42
    LOAD_IMM R11, 10
    MUL64 R10, R10, R11
    
    .assert_reg R10, 420
}

.test "mul64_extended_registers" {
    .description "Tests MUL64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "mul64"
    .tag "extended"
    
    LOAD_IMM R13, 25
    LOAD_IMM R14, 40
    MUL64 R13, R13, R14
    
    .assert_reg R13, 1000
}
