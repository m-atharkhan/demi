; Arithmetic Overflow and Underflow Test Suite
; Tests for overflow/underflow behavior in arithmetic operations
; Author: copilot
; Category: Arithmetic

; =============================================================================
; 32-bit Integer Overflow Tests
; =============================================================================

.test "ADD 32-bit overflow wrap-around" {
    .description "Test that 0xFFFFFFFF + 1 wraps to 0"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFF
    LOAD_IMM R1, 1
    ADD R0, R1
    halt
    .assert_reg R0, 0
}

.test "ADD maximum values overflow" {
    .description "Test adding two maximum values in 32-bit mode"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM EAX, 0x80000000
    LOAD_IMM EBX, 0x80000000
    ADD EAX, EBX
    halt
    .assert_reg EAX, 0
}

.test "ADD signed overflow positive" {
    .description "Test signed overflow: 0x7FFFFFFF + 1"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x7FFFFFFF    ; Max signed positive
    LOAD_IMM R1, 1
    ADD R0, R1
    halt
    .assert_reg R0, 0x80000000  ; Wraps to min signed negative
}

.test "INC overflow from max value" {
    .description "Test INC overflow: 0xFFFFFFFF + 1"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFF
    INC R0
    halt
    .assert_reg R0, 0
}

.test "multiple INC causing overflow" {
    .description "Test multiple increments wrapping around"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFE
    INC R0
    INC R0
    INC R0
    halt
    .assert_reg R0, 1
}

; =============================================================================
; 32-bit Integer Underflow Tests
; =============================================================================

.test "SUB 32-bit underflow wrap-around" {
    .description "Test that 0 - 1 wraps to 0xFFFFFFFF"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0
    LOAD_IMM R1, 1
    SUB R0, R1
    halt
    .assert_reg R0, 0xFFFFFFFF
}

.test "SUB minimum value underflow" {
    .description "Test subtracting from minimum signed value"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x80000000    ; Min signed negative
    LOAD_IMM R1, 1
    SUB R0, R1
    halt
    .assert_reg R0, 0x7FFFFFFF  ; Wraps to max signed positive
}

.test "DEC underflow from zero" {
    .description "Test DEC underflow: 0 - 1"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0
    DEC R0
    halt
    .assert_reg R0, 0xFFFFFFFF
}

.test "multiple DEC causing underflow" {
    .description "Test multiple decrements wrapping around"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 1
    DEC R0
    DEC R0
    DEC R0
    halt
    .assert_reg R0, 0xFFFFFFFE
}

.test "SUB larger from smaller" {
    .description "Test subtracting larger value from smaller"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 10
    LOAD_IMM R1, 100
    SUB R0, R1
    halt
    .assert_reg R0, 4294967206  ; 10 - 100 = -90 (as unsigned)
}

; =============================================================================
; Multiplication Overflow Tests
; =============================================================================

.test "MUL small values no overflow" {
    .description "Test multiplication without overflow"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 100
    LOAD_IMM R1, 100
    MUL R0, R1
    halt
    .assert_reg R0, 10000
}

.test "MUL causing overflow" {
    .description "Test multiplication overflow with large values"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x10000      ; 65536
    LOAD_IMM R1, 0x10000      ; 65536
    MUL R0, R1
    halt
    .assert_reg R0, 0         ; 65536 * 65536 = 4294967296, wraps to 0
}

.test "MUL overflow with max values" {
    .description "Test multiplying maximum values"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFF
    LOAD_IMM R1, 2
    MUL R0, R1
    halt
    .assert_reg R0, 0xFFFFFFFE  ; Overflow wraps
}

.test "MUL by zero after overflow setup" {
    .description "Test that multiplication by zero works correctly"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFF
    LOAD_IMM R1, 0
    MUL R0, R1
    halt
    .assert_reg R0, 0
}

; =============================================================================
; 64-bit Overflow Tests
; =============================================================================

.test "ADD64 overflow wrap-around" {
    .description "Test 64-bit addition overflow"
    .author "copilot"
    .category "64-bit Operations"
    
    MODE64
    ; Load max 64-bit value into RAX
    LOAD_IMM64 RAX, 0xFFFFFFFFFFFFFFFF
    LOAD_IMM64 RBX, 1
    ADD64 RAX, RBX
    HALT
    .assert_reg RAX, 0
}

.test "SUB64 underflow wrap-around" {
    .description "Test 64-bit subtraction underflow"
    .author "copilot"
    .category "64-bit Operations"
    
    MODE64
    LOAD_IMM64 RAX, 0
    LOAD_IMM64 RBX, 1
    SUB64 RAX, RBX
    HALT
    .assert_reg RAX, 0xFFFFFFFFFFFFFFFF
}

.test "INC64 overflow from max value" {
    .description "Test 64-bit increment overflow"
    .author "copilot"
    .category "64-bit Operations"
    
    MODE64
    LOAD_IMM64 RAX, 0xFFFFFFFFFFFFFFFF
    INC64 RAX
    HALT
    .assert_reg RAX, 0
}

.test "DEC64 underflow from zero" {
    .description "Test 64-bit decrement underflow"
    .author "copilot"
    .category "64-bit Operations"
    
    MODE64
    LOAD_IMM64 RAX, 0
    DEC64 RAX
    HALT
    .assert_reg RAX, 0xFFFFFFFFFFFFFFFF
}

; =============================================================================
; Boundary Value Arithmetic
; =============================================================================

.test "ADD at boundaries near overflow" {
    .description "Test addition near overflow boundary"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFE
    LOAD_IMM R1, 1
    ADD R0, R1
    halt
    .assert_reg R0, 0xFFFFFFFF
}

.test "SUB at boundaries near underflow" {
    .description "Test subtraction near underflow boundary"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 1
    LOAD_IMM R1, 1
    SUB R0, R1
    halt
    .assert_reg R0, 0
}

.test "ADD chain with overflow" {
    .description "Test chain of additions with overflow"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x80000000
    LOAD_IMM R1, 0x80000000
    ADD R0, R1
    LOAD_IMM R1, 0x80000000
    ADD R0, R1
    halt
    .assert_reg R0, 0x80000000  ; Wraps back
}

.test "SUB chain with underflow" {
    .description "Test chain of subtractions with underflow"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 5
    LOAD_IMM R1, 2
    SUB R0, R1
    SUB R0, R1
    SUB R0, R1
    halt
    .assert_reg R0, 0xFFFFFFFF  ; 5 - 2 - 2 - 2 = -1 (underflow)
}

; =============================================================================
; Mixed Operations with Overflow
; =============================================================================

.test "overflow then underflow" {
    .description "Test ADD overflow followed by SUB underflow"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0xFFFFFFFF
    INC R0                      ; Overflow to 0
    DEC R0                      ; Underflow back to 0xFFFFFFFF
    halt
    .assert_reg R0, 0xFFFFFFFF
}

.test "alternating INC DEC at boundary" {
    .description "Test alternating increment/decrement at zero boundary"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0
    DEC R0                      ; Underflow to 0xFFFFFFFF
    INC R0                      ; Overflow back to 0
    halt
    .assert_reg R0, 0
}

.test "ADD with self overflow" {
    .description "Test adding register to itself causing overflow"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x80000000
    ADD R0, R0
    halt
    .assert_reg R0, 0           ; 0x80000000 + 0x80000000 = 0 (overflow)
}

.test "MUL overflow then ADD" {
    .description "Test multiplication overflow followed by addition"
    .author "copilot"
    .category "Arithmetic"
    
    MODE32
    LOAD_IMM R0, 0x10000
    LOAD_IMM R1, 0x10000
    MUL R0, R1                  ; Overflow to 0
    LOAD_IMM R1, 42
    ADD R0, R1
    halt
    .assert_reg R0, 42
}
