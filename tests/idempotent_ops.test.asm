; Test: Idempotent and Self-Operations
; Category: Operations
; Description: Test operations on same register/value that should be idempotent or predictable

.test "MOV self - idempotent" {
    .description "Moving a register to itself should not change it"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM R0, 42
    MOV R0, R0
    .assert_reg R0, 42
    halt
}

.test "MOV EAX to EAX - x64 alias" {
    .description "Moving EAX to itself (register alias test)"
    .author "copilot"
    .category "Core Instructions"
    
    LOAD_IMM EAX, 100
    MOV EAX, EAX
    .assert_reg EAX, 100
    halt
}

.test "XOR self - zeroing idiom" {
    .description "XOR of register with itself is common zeroing pattern"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0xFFFFFFFF
    XOR R0, R0
    .assert_reg R0, 0
    halt
}

.test "SUB self - zeroing" {
    .description "Subtracting register from itself always gives zero"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 12345
    SUB R0, R0
    .assert_reg R0, 0
    halt
}

.test "AND self - idempotent" {
    .description "AND of value with itself is idempotent (x & x = x)"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    AND R0, R0
    .assert_reg R0, 0b10101010
    halt
}

.test "OR self - idempotent" {
    .description "OR of value with itself is idempotent (x | x = x)"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    OR R0, R0
    .assert_reg R0, 0b10101010
    halt
}

.test "ADD zero - identity" {
    .description "Adding zero is the additive identity"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 99
    LOAD_IMM R1, 0
    ADD R0, R1
    .assert_reg R0, 99
    halt
}

.test "SUB zero - identity" {
    .description "Subtracting zero leaves value unchanged"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 99
    LOAD_IMM R1, 0
    SUB R0, R1
    .assert_reg R0, 99
    halt
}

.test "MUL one - identity" {
    .description "Multiplying by one is the multiplicative identity"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 77
    LOAD_IMM R1, 1
    MUL R0, R1
    .assert_reg R0, 77
    halt
}

.test "DIV one - identity" {
    .description "Dividing by one leaves value unchanged"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 77
    LOAD_IMM R1, 1
    DIV R0, R1
    .assert_reg R0, 77
    halt
}

.test "MUL zero - annihilator" {
    .description "Multiplying by zero is the multiplicative annihilator"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 999
    LOAD_IMM R1, 0
    MUL R0, R1
    .assert_reg R0, 0
    halt
}

.test "AND zero - annihilator" {
    .description "AND with zero is the logical annihilator"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0xFFFFFFFF
    LOAD_IMM R1, 0
    AND R0, R1
    .assert_reg R0, 0
    halt
}

.test "OR all-ones - saturation" {
    .description "OR with all ones saturates to all ones"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    LOAD_IMM R1, 0xFFFFFFFF
    OR R0, R1
    .assert_reg R0, 0xFFFFFFFF
    halt
}

.test "AND all-ones - identity" {
    .description "AND with all ones is the logical identity"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    LOAD_IMM R1, 0xFFFFFFFF
    AND R0, R1
    .assert_reg R0, 0b10101010
    halt
}

.test "NOT NOT - double negation identity" {
    .description "Double negation returns to original value"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    NOT R0
    NOT R0
    .assert_reg R0, 0b10101010
    halt
}

.test "INC then DEC - identity" {
    .description "Incrementing then decrementing returns to original"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 50
    INC R0
    DEC R0
    .assert_reg R0, 50
    halt
}

.test "DEC then INC - identity" {
    .description "Decrementing then incrementing returns to original"
    .author "copilot"
    .category "Arithmetic"
    
    LOAD_IMM R0, 50
    DEC R0
    INC R0
    .assert_reg R0, 50
    halt
}

.test "CMP self - always equal" {
    .description "Comparing register with itself always shows equal"
    .author "copilot"
    .category "Control Flow"
    
    LOAD_IMM R0, 42
    CMP R0, R0
    JE equal_branch
    HALT
equal_branch:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "XORPS self - SSE zeroing idiom" {
    .description "XOR of XMM register with itself zeros it (common SSE idiom)"
    .author "copilot"
    .category "SIMD"
    
    LOAD_IMM64 XMM0, 0xFFFFFFFFFFFFFFFF
    LOAD_IMM64 XMM0_HIGH, 0xFFFFFFFFFFFFFFFF
    XORPS XMM0, XMM0
    .assert_reg XMM0, 0
    .assert_reg XMM0_HIGH, 0
    halt
}

.test "MOVAPS self - SSE idempotent" {
    .description "Moving XMM register to itself is idempotent"
    .author "copilot"
    .category "SIMD"
    
    LOAD_IMM64 XMM0, 0x123456789ABCDEF0
    LOAD_IMM64 XMM0_HIGH, 0xFEDCBA9876543210
    MOVAPS XMM0, XMM0
    .assert_reg XMM0, 0x123456789ABCDEF0
    .assert_reg XMM0_HIGH, 0xFEDCBA9876543210
    halt
}
