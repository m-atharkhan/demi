; ==========================================
; Conditional Jump Opcodes Test Suite
; Tests JS, JNS, JC, JNC, JO, JNO, JG, JL, JGE, JLE
; ==========================================

.test "js_sign_flag_set" {
    .description "Tests JS when sign flag is set (negative result)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "js"
    .tag "sign"
    
    LOAD_IMM RAX, 5
    LOAD_IMM RBX, 10
    CMP64 RAX, RBX  ; Compare 5 with 10 (5 < 10), sets sign flag
    JS sign_set
    LOAD_IMM RDX, 0
    JMP end
sign_set:
    LOAD_IMM RDX, 1
end:
    .assert_reg RDX, 1
}

.test "jns_sign_flag_clear" {
    .description "Tests JNS when sign flag is clear (positive result)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jns"
    .tag "sign"
    
    LOAD_IMM RAX, 10
    LOAD_IMM RBX, 5
    CMP64 RAX, RBX  ; Compare 10 with 5 (10 > 5), clears sign flag
    JNS no_sign
    LOAD_IMM RCX, 0
    JMP end
no_sign:
    LOAD_IMM RCX, 1
end:
    .assert_reg RCX, 1
}

.test "jg_greater_than" {
    .description "Tests JG (jump if greater) signed comparison"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jg"
    .tag "comparison"
    
    LOAD_IMM R5, 10
    LOAD_IMM R6, 5
    CMP R5, R6
    JG greater
    LOAD_IMM R7, 0
    JMP end
greater:
    LOAD_IMM R7, 1
end:
    .assert_reg R7, 1
}

.test "jl_less_than" {
    .description "Tests JL (jump if less) signed comparison"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jl"
    .tag "comparison"
    
    LOAD_IMM RAX, 3
    LOAD_IMM RBX, 8
    CMP RAX, RBX
    JL less
    LOAD_IMM RCX, 0
    JMP end
less:
    LOAD_IMM RCX, 1
end:
    .assert_reg RCX, 1
}

.test "jge_greater_or_equal" {
    .description "Tests JGE (jump if greater or equal)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jge"
    .tag "comparison"
    
    LOAD_IMM RDX, 7
    LOAD_IMM R0, 7
    CMP RDX, R0
    JGE greater_equal
    LOAD_IMM R1, 0
    JMP end
greater_equal:
    LOAD_IMM R1, 1
end:
    .assert_reg R1, 1
}

.test "jle_less_or_equal" {
    .description "Tests JLE (jump if less or equal)"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jle"
    .tag "comparison"
    
    LOAD_IMM R2, 4
    LOAD_IMM R3, 4
    CMP R2, R3
    JLE less_equal
    LOAD_IMM R5, 0
    JMP end
less_equal:
    LOAD_IMM R5, 1
end:
    .assert_reg R5, 1
}

.test "jg_not_taken" {
    .description "Tests JG not taken when values are equal"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jg"
    .tag "not_taken"
    
    LOAD_IMM R6, 10
    LOAD_IMM R7, 10
    CMP R6, R7
    JG not_taken
    LOAD_IMM RAX, 1
    JMP end
not_taken:
    LOAD_IMM RAX, 0
end:
    .assert_reg RAX, 1
}

.test "jl_not_taken" {
    .description "Tests JL not taken when first value is greater"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "jl"
    .tag "not_taken"
    
    LOAD_IMM RBX, 15
    LOAD_IMM RCX, 10
    CMP RBX, RCX
    JL not_taken
    LOAD_IMM RDX, 1
    JMP end
not_taken:
    LOAD_IMM RDX, 0
end:
    .assert_reg RDX, 1
}

.test "conditional_chain" {
    .description "Tests chain of conditional jumps"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "chain"
    .tag "multiple"
    
    LOAD_IMM R0, 20
    LOAD_IMM R1, 10
    LOAD_IMM R2, 0
    
    CMP R0, R1
    JL less_path
    JG greater_path
    JMP equal_path
    
less_path:
    LOAD_IMM R2, 1
    JMP end
greater_path:
    LOAD_IMM R2, 2
    JMP end
equal_path:
    LOAD_IMM R2, 3
end:
    .assert_reg R2, 2
}

.test "range_check_pattern" {
    .description "Tests using conditional jumps for range checking"
    .author "DemiEngine Team"
    .category "Opcode Edgecases"
    .tag "range"
    .tag "pattern"
    
    LOAD_IMM R3, 15
    LOAD_IMM R5, 10
    LOAD_IMM R6, 20
    
    CMP R3, R5
    JL out_of_range
    CMP R3, R6
    JG out_of_range
    
    LOAD_IMM R7, 1  ; In range
    JMP end
out_of_range:
    LOAD_IMM R7, 0
end:
    .assert_reg R7, 1
}
