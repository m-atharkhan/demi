; Example assembly file with embedded test cases
; This demonstrates the new #test and #assert features

; Simple addition function
main:
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    ADD EAX, EBX
    HALT

#test "Basic Addition Test" {
    ; Test that 10 + 20 = 30
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    ADD EAX, EBX
    #assert_reg EAX, 30
    HALT
}

#test "Memory Store and Load" {
    ; Test memory operations
    LOAD_IMM EAX, 42
    STORE EAX, 0x100
    LOAD_IMM EBX, 0
    LOAD EBX, 0x100
    #assert_reg EBX, 42
    #assert_mem 0x100, 42
    HALT
}

#test "String Output Test" {
    ; Test string output
    .org 0x200
    msg:
    DB "Hello", 5
    
    LOAD_IMM EAX, 0x200
    OUTSTR EAX, 5
    #assert_output "Hello"
    HALT
}

#test "Division by Zero" {
    ; This should generate an error
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 0
    DIV EAX, EBX
    #expect_error
}

#test "Flag Test - Zero Flag" {
    ; Test that zero flag is set correctly
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 5
    CMP EAX, EBX
    #assert_reg FLAGS, 0x01  ; Zero flag should be set
    HALT
}
