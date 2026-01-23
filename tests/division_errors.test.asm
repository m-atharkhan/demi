; Division by Zero Error Test Suite
; Comprehensive tests for division and modulo by zero error handling
; Author: copilot
; Category: Error Handling
; Phase: 3

; =============================================================================
; 32-bit Division by Zero Tests
; =============================================================================

.test "DIV by zero - basic" {
    .description "Test DIV with zero divisor triggers error"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 0
    DIV EAX, EBX    ; Should trigger division by zero error
}

.test "DIV by zero - large dividend" {
    .description "Test DIV by zero with maximum value dividend"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    MODE32
    LOAD_IMM EAX, 0xFFFFFFFF
    LOAD_IMM EBX, 0
    DIV EAX, EBX    ; Maximum value divided by zero
}

.test "DIV by zero - zero dividend" {
    .description "Test DIV by zero where dividend is also zero"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 0
    DIV EAX, EBX    ; 0 / 0 should still error
}

.test "DIV by zero - after valid operation" {
    .description "Test DIV by zero after successful division"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 10
    DIV EAX, EBX    ; Valid: 100 / 10 = 10
    LOAD_IMM EBX, 0
    DIV EAX, EBX    ; Now divide by zero
}

; =============================================================================
; 32-bit Modulo by Zero Tests
; =============================================================================

.test "MOD by zero - basic" {
    .description "Test MOD with zero divisor triggers error"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 0
    MOD EAX, EBX    ; Should trigger modulo by zero error
}

.test "MOD by zero - large dividend" {
    .description "Test MOD by zero with maximum value"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "error"
    .expect_error true
    
    MODE32
    LOAD_IMM EAX, 0xFFFFFFFF
    LOAD_IMM EBX, 0
    MOD EAX, EBX    ; Maximum value modulo zero
}

.test "MOD by zero - zero dividend" {
    .description "Test MOD by zero where dividend is also zero"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 0
    MOD EAX, EBX    ; 0 % 0 should still error
}

.test "MOD by zero - after valid operation" {
    .description "Test MOD by zero after successful modulo"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 7
    MOD EAX, EBX    ; Valid: 100 % 7 = 2
    LOAD_IMM EBX, 0
    MOD EAX, EBX    ; Now modulo by zero
}

; =============================================================================
; 64-bit Division by Zero Tests
; =============================================================================

.test "DIV64 by zero - basic" {
    .description "Test 64-bit division by zero"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "64-bit"
    .tag "error"
    .expect_error true
    
    MODE64
    LOAD_IMM64 RAX, 1000000
    LOAD_IMM64 RBX, 0
    DIV64 RCX, RAX, RBX    ; RCX = RAX / RBX, but RBX is zero
}

.test "DIV64 by zero - maximum value" {
    .description "Test 64-bit division by zero with max value"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "64-bit"
    .tag "error"
    .expect_error true
    
    MODE64
    LOAD_IMM64 RAX, 0xFFFFFFFFFFFFFFFF
    LOAD_IMM64 RBX, 0
    DIV64 RCX, RAX, RBX    ; Maximum 64-bit value divided by zero
}

; =============================================================================
; 64-bit Modulo by Zero Tests
; =============================================================================

.test "MOD64 by zero - basic" {
    .description "Test 64-bit modulo by zero"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "64-bit"
    .tag "error"
    .expect_error true
    
    MODE64
    LOAD_IMM64 RAX, 1000000
    LOAD_IMM64 RBX, 0
    MOD64 RCX, RAX, RBX    ; RCX = RAX % RBX, but RBX is zero
}

.test "MOD64 by zero - maximum value" {
    .description "Test 64-bit modulo by zero with max value"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "64-bit"
    .tag "error"
    .expect_error true
    
    MODE64
    LOAD_IMM64 RAX, 0xFFFFFFFFFFFFFFFF
    LOAD_IMM64 RBX, 0
    MOD64 RCX, RAX, RBX    ; Maximum 64-bit value modulo zero
}

; =============================================================================
; Edge Cases with Register Combinations
; =============================================================================

.test "DIV self by zero" {
    .description "Test dividing register by itself when it's zero"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 0
    DIV EAX, EAX    ; 0 / 0 (self-division)
}

.test "MOD self by zero" {
    .description "Test modulo register by itself when it's zero"
    .author "copilot"
    .category "Error Handling"
    .tag "modulo"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 0
    MOD EAX, EAX    ; 0 % 0 (self-modulo)
}

; =============================================================================
; Multiple Operations Before Error
; =============================================================================

.test "chain operations ending in DIV by zero" {
    .description "Test multiple operations before division by zero"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    MODE32
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 10
    ADD EAX, EBX    ; EAX = 110
    MUL EAX, EBX    ; EAX = 1100
    SUB EBX, EBX    ; EBX = 0
    DIV EAX, EBX    ; Divide by zero
}

.test "loop until DIV by zero" {
    .description "Test decrementing divisor to zero in loop"
    .author "copilot"
    .category "Error Handling"
    .tag "division"
    .tag "error"
    .expect_error true
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 3
    
    ; First division: 100 / 3 = 33
    DIV EAX, EBX
    
    ; Decrement divisor
    DEC EBX         ; EBX = 2
    DIV EAX, EBX    ; 33 / 2 = 16
    
    DEC EBX         ; EBX = 1
    DIV EAX, EBX    ; 16 / 1 = 16
    
    DEC EBX         ; EBX = 0
    DIV EAX, EBX    ; 16 / 0 - ERROR!
}
