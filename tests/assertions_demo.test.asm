; Example assembly file with embedded test cases
; This demonstrates the .test and .assert features


.test "Basic Addition Test" {
    .description "Test that 10 + 20 = 30"
    .author "bobrossrtx"
    .category "Arithmetic"
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    ADD EAX, EBX
    .assert_reg EAX, 30
    HALT
}

.test "Memory Store and Load" {
    .description "Test memory operations with STORE and LOAD"
    .author "bobrossrtx"
    .category "Memory"
    ; Note: STORE uses 8-bit addresses (0-255), so we use 0x80
    LOAD_IMM EAX, 42
    STORE EAX, 0x80
    LOAD_IMM EBX, 0
    LOAD EBX, 0x80
    .assert_reg EBX, 42
    .assert_mem 0x80, 42
    HALT
}

.test "String Output Test" {
    .description "Test string output with OUTSTR"
    .author "bobrossrtx"
    .category "I/O"
    ; Test string output - currently skipped due to test infrastructure limitations
    ; with .org and DB directives inside test blocks
    .skip
    .org 0x80
    msg:
    DB "Hello", 5
    
    LOAD_IMM EAX, 0x80
    OUTSTR EAX, 5
    .assert_output "Hello"
    HALT
}

.test "Division by Zero" {
    .description "Test that division by zero generates an error"
    .author "bobrossrtx"
    .category "Error Handling"
    ; This should generate an error
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 0
    DIV EAX, EBX
    .expect_error
}

.test "Flag Test - Zero Flag" {
    .description "Test that zero flag is set correctly after CMP"
    .author "bobrossrtx"
    .category "Flags"
    ; DemiEngine flags: ZERO=bit0, SIGN=bit1, CARRY=bit2, OVERFLOW=bit3
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 5
    CMP EAX, EBX
    .assert_reg RFLAGS, 0x01  ; Zero flag (bit 0) should be set
    HALT
}
