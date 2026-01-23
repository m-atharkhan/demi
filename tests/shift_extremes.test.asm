; Test: Shift Operation Edge Cases
; Category: Logical Operations
; Description: Test extreme shift counts and boundary conditions

.test "shift by zero (SHL) - no operation" {
    .description "SHL by 0 should not change the value"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    SHL R0, 0
    .assert_reg R0, 0b10101010
    halt
}

.test "shift by zero (SHR) - no operation" {
    .description "SHR by 0 should not change the value"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b10101010
    SHR R0, 0
    .assert_reg R0, 0b10101010
    halt
}

.test "shift left by 1" {
    .description "Basic left shift by 1 bit"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0b00000001
    SHL R0, 1
    .assert_reg R0, 0b00000010
    halt
}

.test "shift left by 31 (maximum meaningful)" {
    .description "Shift 1 into MSB position"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 1
    SHL R0, 31
    .assert_reg R0, 0x80000000
    halt
}

.test "shift right by 31 (maximum meaningful)" {
    .description "Shift MSB to LSB position"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0x80000000
    SHR R0, 31
    .assert_reg R0, 1
    halt
}

.test "shift left overflow - bit loss" {
    .description "Shifting MSB out results in bit loss"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0x80000000
    SHL R0, 1
    .assert_reg R0, 0
    halt
}

.test "shift right to zero" {
    .description "Shift all bits out to the right"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0xFF
    SHR R0, 8
    .assert_reg R0, 0
    halt
}

.test "shift left by power of 2 - multiplication equivalent" {
    .description "Left shift is equivalent to multiplication by power of 2"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 5
    SHL R0, 2
    .assert_reg R0, 20
    halt
}

.test "shift right by power of 2 - division equivalent" {
    .description "Right shift is equivalent to division by power of 2"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 20
    SHR R0, 2
    .assert_reg R0, 5
    halt
}

.test "shift preserves zero" {
    .description "Shifting zero always results in zero"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0
    SHL R0, 10
    .assert_reg R0, 0
    SHR R0, 10
    .assert_reg R0, 0
    halt
}

.test "shift left then right - identity for even" {
    .description "SHL then SHR returns to original for even values"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 8
    SHL R0, 1
    SHR R0, 1
    .assert_reg R0, 8
    halt
}

.test "shift left then right - loses precision" {
    .description "SHL by more than SHR causes precision loss"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 7
    SHL R0, 2
    SHR R0, 3
    .assert_reg R0, 3
    halt
}

.test "consecutive left shifts" {
    .description "Multiple left shifts accumulate"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 1
    SHL R0, 1
    SHL R0, 1
    SHL R0, 1
    .assert_reg R0, 8
    halt
}

.test "consecutive right shifts" {
    .description "Multiple right shifts accumulate"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 64
    SHR R0, 1
    SHR R0, 1
    SHR R0, 1
    .assert_reg R0, 8
    halt
}

.test "shift creates power of 2" {
    .description "Shifting 1 left creates powers of 2"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 1
    SHL R0, 10
    .assert_reg R0, 1024
    halt
}

.test "shift right larger value" {
    .description "Shift right divides large values"
    .author "copilot"
    .category "Logical Operations"
    
    LOAD_IMM R0, 0xFFFF
    SHR R0, 8
    .assert_reg R0, 0xFF
    halt
}
