; ==========================================
; AND Opcode Test Suite
; Tests bitwise AND operation
; ==========================================

.test "and_basic" {
    .description "Tests basic AND operation"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "basic"
    
    LOAD_IMM RAX, 0b1111
    LOAD_IMM RBX, 0b1010
    AND RAX, RAX, RBX
    
    .assert_reg RAX, 0b1010
}

.test "and_all_zeros" {
    .description "Tests AND with all zeros"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "zero"
    
    LOAD_IMM R8, 0xFFFFFFFF
    LOAD_IMM R9, 0
    AND R10, R8, R9
    
    .assert_reg R10, 0
}

.test "and_all_ones" {
    .description "Tests AND with all ones (identity)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "identity"
    
    LOAD_IMM R11, 0x12345678
    LOAD_IMM R12, 0xFFFFFFFF
    AND R13, R11, R12
    
    .assert_reg R13, 0x12345678
}

.test "and_mask_low_byte" {
    .description "Tests AND to mask low byte"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "mask"
    
    LOAD_IMM R14, 0xABCD1234
    LOAD_IMM R15, 0xFF
    AND RAX, R14, R15
    
    .assert_reg RAX, 0x34
}

.test "and_mask_high_byte" {
    .description "Tests AND to mask high byte"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "mask"
    
    LOAD_IMM RBX, 0xABCD1234
    LOAD_IMM RCX, 0xFF000000
    AND RDX, RBX, RCX
    
    .assert_reg RDX, 0xAB000000
}

.test "and_even_bits" {
    .description "Tests AND to extract even bit positions"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "pattern"
    
    LOAD_IMM R8, 0xFFFFFFFF
    LOAD_IMM R9, 0xAAAAAAAA  ; 10101010...
    AND R10, R8, R9
    
    .assert_reg R10, 0xAAAAAAAA
}

.test "and_odd_bits" {
    .description "Tests AND to extract odd bit positions"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "pattern"
    
    LOAD_IMM R11, 0xFFFFFFFF
    LOAD_IMM R12, 0x55555555  ; 01010101...
    AND R13, R11, R12
    
    .assert_reg R13, 0x55555555
}

.test "and_commutative" {
    .description "Tests AND commutative property (a & b = b & a)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "commutative"
    
    LOAD_IMM R14, 0xF0F0
    LOAD_IMM R15, 0x3C3C
    AND RAX, R14, R15
    AND RBX, R15, R14
    
    CMP64 RAX, RBX
    JZ equal
    LOAD_IMM RCX, 0
    JMP end
equal:
    LOAD_IMM RCX, 1
end:
    .assert_reg RCX, 1
}

.test "and_extended_registers" {
    .description "Tests AND with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "extended"
    
    LOAD_IMM R8, 0xFF00FF00
    LOAD_IMM R9, 0x00FF00FF
    AND R10, R8, R9
    
    .assert_reg R10, 0
}

.test "and_clear_bit_flag" {
    .description "Tests AND to clear specific bit (flag manipulation)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "and"
    .tag "flag"
    
    LOAD_IMM R11, 0b11111111
    LOAD_IMM R12, 0b11111011  ; Clear bit 2
    AND R13, R11, R12
    
    .assert_reg R13, 0b11111011
}
