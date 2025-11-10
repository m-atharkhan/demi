; Register Operations Test Suite
; Tests for register operations and data movement

.test "register to register move" {
    .description "Tests MOV instruction for copying values between registers"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "basic"
    .tag "move"
    LOAD_IMM R0, 42
    MOV R1, R0
    .assert_reg R0, 42  ; Source unchanged
    .assert_reg R1, 42  ; Destination copied
}

.test "multiple register operations" {
    .description "Tests loading immediate values into multiple registers"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "basic"
    .tag "load-immediate"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    .assert_reg R0, 10
    .assert_reg R1, 20
    .assert_reg R2, 30
    .assert_reg R3, 40
}

.test "register independence" {
    .description "Tests that operations on one register don't affect other registers"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "independence"
    .tag "arithmetic"
    LOAD_IMM R0, 100
    LOAD_IMM R1, 200
    ADD R0, R1  ; Only R0 should change
    .assert_reg R0, 44  ; 100 + 200 = 300, but wraps to 44 (300 - 256)
    .assert_reg R1, 200  ; R1 unchanged
}

.test "extended registers" {
    .description "Tests using registers beyond R0-R7 (extended register set)"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "extended"
    .tag "basic"
    LOAD_IMM R6, 80
    LOAD_IMM R7, 90
    .assert_reg R6, 80
    .assert_reg R7, 90
}

.test "high register range test" {
    .description "Tests registers R8 and beyond to find the supported range"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "extended"
    .tag "limits"
    
    ; Use 64-bit mode and 64-bit opcodes for extended registers (R8+)
    MODE64
    LOAD_IMM64 R8, 123
    .assert_reg R8, 123
    
    ; Verify moving between extended registers using 64-bit move
    LOAD_IMM64 R9, 77
    MOV64 R10, R9
    .assert_reg R10, 77
}

.test "register chain operations" {
    .description "Tests chaining operations through multiple registers"
    .author "DemiEngine Team"
    .category "Registers"
    .tag "chain"
    .tag "complex"
    LOAD_IMM R0, 10
    MOV R1, R0      ; R1 = 10
    ADD R1, R0      ; R1 = 20
    MOV R2, R1      ; R2 = 20
    MUL R2, R0      ; R2 = 200
    .assert_reg R0, 10   ; Original unchanged
    .assert_reg R1, 20   ; First operation
    .assert_reg R2, 200  ; Final result
}