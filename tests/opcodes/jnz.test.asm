; ==========================================
; JNZ (Jump if Not Zero) Opcode Test Suite
; Tests conditional jump when zero flag is not set
; ==========================================


.test "jnz_when_not_equal" {
    .description "Tests JNZ jumps when values are not equal (zero flag clear)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "jump"
    
    LOAD_IMM RAX, 5
    LOAD_IMM RBX, 10
    CMP64 RAX, RBX  ; Clears zero flag
    JNZ target
    LOAD_IMM RCX, 1  ; Should not execute
    HALT
target:
    LOAD_IMM RCX, 5
    .assert_reg RCX, 5
}

.test "jnz_when_equal" {
    .description "Tests JNZ does not jump when values are equal (zero flag set)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "no_jump"
    
    LOAD_IMM RDX, 100
    LOAD_IMM R8, 100
    CMP64 RDX, R8  ; Sets zero flag
    JNZ fail
    LOAD_IMM R9, 100  ; Should execute
    .assert_reg R9, 100
    JMP done
fail:
    LOAD_IMM R9, 1
    HALT
done:
}

.test "jnz_loop_countdown" {
    .description "Tests JNZ in countdown loop pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "loop"
    
    LOAD_IMM R10, 5
    LOAD_IMM R11, 0
loop_start:
    INC64 R11
    DEC64 R10
    CMP R10, 0
    JNZ loop_start
    
    ; R11 should have counted to 5
    .assert_reg R11, 5
}

.test "jnz_after_nonzero_sub" {
    .description "Tests JNZ after subtraction with non-zero result"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "arithmetic"
    
    LOAD_IMM R12, 50
    LOAD_IMM R13, 30
    SUB64 R12, R13  ; Result is 20, clears zero flag
    JNZ success
    HALT
success:
    LOAD_IMM R14, 20
    .assert_reg R14, 20
}

.test "jnz_after_zero_sub" {
    .description "Tests JNZ does not jump after subtraction with zero result"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "arithmetic"
    
    LOAD_IMM R15, 100
    LOAD_IMM RAX, 100
    SUB64 R15, RAX  ; Result is 0, sets zero flag
    JNZ fail
    LOAD_IMM RBX, 0
    .assert_reg RBX, 0
    JMP done
fail:
    LOAD_IMM RBX, 1
    HALT
done:
}

.test "jnz_skip_over_code" {
    .description "Tests JNZ to skip over code block"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "skip"
    
    LOAD_IMM RCX, 1
    CMP RCX, 0
    JNZ skip_block
    LOAD_IMM RDX, 1
    LOAD_IMM R8, 2
    LOAD_IMM R9, 3
skip_block:
    LOAD_IMM R10, 100
    .assert_reg R10, 100
}

.test "jnz_with_extended_registers" {
    .description "Tests JNZ with R8-R15 extended registers"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "extended"
    
    LOAD_IMM R11, 50
    LOAD_IMM R12, 60
    CMP64 R11, R12
    JNZ success
    HALT
success:
    LOAD_IMM R13, 50
    .assert_reg R13, 50
}

.test "jnz_counter_pattern" {
    .description "Tests JNZ in typical counter pattern"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "counter"
    
    LOAD_IMM R0, 3
    LOAD_IMM R1, 0
count_loop:
    INC64 R1
    DEC64 R0
    CMP R0, 0
    JNZ count_loop
    
    .assert_reg R1, 3
}

.test "jnz_multiple_conditions" {
    .description "Tests multiple JNZ conditions"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jnz"
    .tag "multiple"
    
    LOAD_IMM RAX, 1
    CMP RAX, 0
    JNZ test2
    HALT
test2:
    LOAD_IMM RBX, 2
    CMP RBX, 0
    JNZ test3
    HALT
test3:
    LOAD_IMM RCX, 99
    .assert_reg RCX, 99
}
