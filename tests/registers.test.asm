; Register Operations Test Suite
; Tests for register operations and data movement

.test "register to register move" {
    .description "Tests MOV instruction for copying values between registers"
    .author "bobrossrtx"
    .category "Registers"
    .tag "basic"
    .tag "move"
    LOAD_IMM EAX, 42
    MOV EBX, EAX
    .assert_reg EAX, 42  ; Source unchanged
    .assert_reg EBX, 42  ; Destination copied
}

.test "multiple register operations" {
    .description "Tests loading immediate values into multiple registers"
    .author "bobrossrtx"
    .category "Registers"
    .tag "basic"
    .tag "load-immediate"
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    LOAD_IMM ECX, 30
    LOAD_IMM EDX, 40
    .assert_reg EAX, 10
    .assert_reg EBX, 20
    .assert_reg ECX, 30
    .assert_reg EDX, 40
}

.test "register independence" {
    .description "Tests that operations on one register don't affect other registers"
    .author "bobrossrtx"
    .category "Registers"
    .tag "independence"
    .tag "arithmetic"
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 200
    ADD EAX, EBX  ; Only EAX should change
    .assert_reg EAX, 44  ; 100 + 200 = 300, but wraps to 44 (300 - 256)
    .assert_reg EBX, 200  ; EBX unchanged
}

.test "extended registers" {
    .description "Tests using registers beyond EAX-EBP (extended register set)"
    .author "bobrossrtx"
    .category "Registers"
    .tag "extended"
    .tag "basic"
    LOAD_IMM ESP, 80
    LOAD_IMM EBP, 90
    .assert_reg ESP, 80
    .assert_reg EBP, 90
}

.test "high register range test" {
    .description "Tests registers R8 and beyond to find the supported range"
    .author "bobrossrtx"
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
    .author "bobrossrtx"
    .category "Registers"
    .tag "chain"
    .tag "complex"
    LOAD_IMM EAX, 10
    MOV EBX, EAX      ; EBX = 10
    ADD EBX, EAX      ; EBX = 20
    MOV ECX, EBX      ; ECX = 20
    MUL ECX, EAX      ; ECX = 200
    .assert_reg EAX, 10   ; Original unchanged
    .assert_reg EBX, 20   ; First operation
    .assert_reg ECX, 200  ; Final result
}