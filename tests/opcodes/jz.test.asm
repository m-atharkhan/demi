; ==========================================
; JZ (Jump if Zero) Opcode Test Suite
; Tests conditional jump when zero flag is set
; ==========================================


.test "jz_when_equal" {
    .description "Tests JZ jumps when values are equal (zero flag set)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "jump"
    
    LOAD_IMM RAX, 100
    LOAD_IMM RBX, 100
    CMP64 RAX, RBX  ; Sets zero flag
    JZ target
    LOAD_IMM RCX, 1  ; Should not execute
    HALT
target:
    LOAD_IMM RCX, 100
    .assert_reg RCX, 100
}

.test "jz_when_not_equal" {
    .description "Tests JZ does not jump when values are not equal (zero flag clear)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "no_jump"
    
    LOAD_IMM RDX, 50
    LOAD_IMM R8, 60
    CMP64 RDX, R8  ; Clears zero flag
    JZ fail
    LOAD_IMM R9, 50  ; Should execute
    .assert_reg R9, 50
    JMP done
fail:
    LOAD_IMM R9, 1
    HALT
done:
}

.test "jz_after_subtraction_zero" {
    .description "Tests JZ after subtraction resulting in zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "arithmetic"
    
    LOAD_IMM R10, 100
    LOAD_IMM R11, 100
    SUB64 R10, R11  ; Result is 0, sets zero flag
    JZ success
    LOAD_IMM R12, 1
    HALT
success:
    LOAD_IMM R12, 0
    .assert_reg R12, 0
}

.test "jz_after_subtraction_nonzero" {
    .description "Tests JZ does not jump after subtraction with non-zero result"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "arithmetic"
    
    LOAD_IMM R13, 50
    LOAD_IMM R14, 30
    SUB64 R13, R14  ; Result is 20, clears zero flag
    JZ fail
    LOAD_IMM R15, 20
    .assert_reg R15, 20
    JMP done
fail:
    LOAD_IMM R15, 1
    HALT
done:
}

.test "jz_forward_jump" {
    .description "Tests JZ with forward jump"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "forward"
    
    LOAD_IMM RAX, 0
    CMP RAX, 0
    JZ skip_code
    LOAD_IMM RBX, 1
    LOAD_IMM RCX, 2
    LOAD_IMM RDX, 3
skip_code:
    LOAD_IMM R8, 100
    .assert_reg R8, 100
}

.test "jz_multiple_tests" {
    .description "Tests multiple JZ conditions in sequence"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "multiple"
    
    LOAD_IMM R9, 10
    CMP R9, 10
    JZ test2
    HALT
test2:
    LOAD_IMM R10, 20
    CMP R10, 20
    JZ test3
    HALT
test3:
    LOAD_IMM R11, 30
    .assert_reg R11, 30
}

.test "jz_with_extended_registers" {
    .description "Tests JZ with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "extended"
    
    LOAD_IMM R12, 777
    LOAD_IMM R13, 777
    CMP64 R12, R13
    JZ success
    HALT
success:
    LOAD_IMM R14, 777
    .assert_reg R14, 777
}

.test "jz_zero_comparison" {
    .description "Tests JZ after comparing zero with zero"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jz"
    .tag "zero"
    
    LOAD_IMM R15, 0
    CMP R15, 0
    JZ works
    HALT
works:
    LOAD_IMM RAX, 1
    .assert_reg RAX, 1
}
