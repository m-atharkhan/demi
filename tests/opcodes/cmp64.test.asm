; ==========================================
; CMP64 Opcode Test Suite
; Tests 64-bit comparison and flag setting
; ==========================================


.test "cmp64_equal" {
    .description "Tests comparison of equal values (sets zero flag)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "equal"
    
    LOAD_IMM RAX, 100
    LOAD_IMM RBX, 100
    CMP64 RAX, RBX
    
    ; Should set zero flag - we can't test flags directly yet,
    ; but JZ will verify the flag is set
    JNZ fail
    JMP pass
fail:
    LOAD_IMM RCX, 1
    HALT
pass:
    LOAD_IMM RCX, 100
    .assert_reg RCX, 100
}

.test "cmp64_not_equal" {
    .description "Tests comparison of unequal values (clears zero flag)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "not_equal"
    
    LOAD_IMM RCX, 50
    LOAD_IMM RDX, 60
    CMP64 RCX, RDX
    
    ; Should clear zero flag
    JZ fail
    JMP pass
fail:
    LOAD_IMM R8, 1
    HALT
pass:
    LOAD_IMM R8, 50
    .assert_reg R8, 50
}

.test "cmp64_greater_than" {
    .description "Tests comparison where first > second"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "greater"
    
    LOAD_IMM R8, 200
    LOAD_IMM R9, 100
    CMP64 R8, R9
    
    ; 200 > 100, should not set zero flag
    JZ fail
    JMP pass
fail:
    LOAD_IMM R10, 1
    HALT
pass:
    LOAD_IMM R10, 200
    .assert_reg R10, 200
}

.test "cmp64_less_than" {
    .description "Tests comparison where first < second"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "less"
    
    LOAD_IMM R10, 50
    LOAD_IMM R11, 150
    CMP64 R10, R11
    
    ; 50 < 150, should not set zero flag
    JZ fail
    JMP pass
fail:
    LOAD_IMM R12, 1
    HALT
pass:
    LOAD_IMM R12, 50
    .assert_reg R12, 50
}

.test "cmp64_with_zero" {
    .description "Tests comparison with zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "zero"
    
    LOAD_IMM R12, 0
    LOAD_IMM R13, 0
    CMP64 R12, R13
    
    ; Both zero, should be equal
    JNZ fail
    JMP pass
fail:
    LOAD_IMM R14, 1
    HALT
pass:
    LOAD_IMM R14, 0
    .assert_reg R14, 0
}

.test "cmp64_ascii_equal" {
    .description "Tests comparison of ASCII values (like calculator operator check)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "ascii"
    
    LOAD_IMM R14, 43  ; '+' ASCII
    LOAD_IMM R15, 43
    CMP64 R14, R15
    
    ; Should be equal
    JNZ fail
    JMP pass
fail:
    LOAD_IMM RAX, 1
    HALT
pass:
    LOAD_IMM RAX, 43
    .assert_reg RAX, 43
}

.test "cmp64_ascii_not_equal" {
    .description "Tests comparison of different ASCII values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "ascii"
    
    LOAD_IMM RAX, 43  ; '+'
    LOAD_IMM RBX, 45  ; '-'
    CMP64 RAX, RBX
    
    ; Should not be equal
    JZ fail
    JMP pass
fail:
    LOAD_IMM RCX, 1
    HALT
pass:
    LOAD_IMM RCX, 43
    .assert_reg RCX, 43
}

.test "cmp64_large_numbers" {
    .description "Tests comparison of large numbers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "large"
    
    LOAD_IMM RDX, 1000000
    LOAD_IMM R8, 1000000
    CMP64 RDX, R8
    
    ; Should be equal
    JNZ fail
    JMP pass
fail:
    LOAD_IMM R9, 1
    HALT
pass:
    LOAD_IMM R9, 1000000
    .assert_reg R9, 1000000
}

.test "cmp64_extended_registers" {
    .description "Tests CMP64 with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "extended"
    
    LOAD_IMM R13, 777
    LOAD_IMM R14, 777
    CMP64 R13, R14
    
    JNZ fail
    JMP pass
fail:
    LOAD_IMM R15, 1
    HALT
pass:
    LOAD_IMM R15, 777
    .assert_reg R15, 777
}

.test "cmp64_max_value" {
    .description "Tests comparison with maximum values"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "cmp64"
    .tag "max"
    
    LOAD_IMM R10, 0xFFFFFFFF
    LOAD_IMM R11, 0xFFFFFFFF
    CMP64 R10, R11
    
    JNZ fail
    JMP pass
fail:
    LOAD_IMM R12, 1
    HALT
pass:
    LOAD_IMM R12, 0xFFFFFFFF
    .assert_reg R12, 0xFFFFFFFF
}
