; Example assembly file with embedded test cases
; This demonstrates the new #test and #assert features

; Simple addition function
main:
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1
    HALT

#test "Basic Addition Test" {
    ; Test that 10 + 20 = 30
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1
    #assert_reg R0, 30
    HALT
}

#test "Memory Store and Load" {
    ; Test memory operations
    LOAD_IMM R0, 42
    STORE R0, 0x100
    LOAD_IMM R1, 0
    LOAD R1, 0x100
    #assert_reg R1, 42
    #assert_mem 0x100, 42
    HALT
}

#test "String Output Test" {
    ; Test string output
    .org 0x200
    msg:
    DB "Hello", 5
    
    LOAD_IMM R0, 0x200
    OUTSTR R0, 5
    #assert_output "Hello"
    HALT
}

#test "Division by Zero" {
    ; This should generate an error
    LOAD_IMM R0, 10
    LOAD_IMM R1, 0
    DIV R0, R1
    #expect_error
}

#test "Flag Test - Zero Flag" {
    ; Test that zero flag is set correctly
    LOAD_IMM R0, 5
    LOAD_IMM R1, 5
    CMP R0, R1
    #assert_reg FLAGS, 0x01  ; Zero flag should be set
    HALT
}
