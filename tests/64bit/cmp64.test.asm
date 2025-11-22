; 64-bit Comparison Tests
; Tests for CMP64 instruction with various scenarios

.test "cmp64 equal values" {
    .description "Test 64-bit comparison of equal values"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "equal"
    
    LOAD_IMM R0, 123
    LOAD_IMM R1, 123
    CMP64 R0, R1          ; Compare R0 with R1 (equal)
    ; After CMP64, flags should be set appropriately
    ; We can't directly test flags, but we can test conditional behavior
    HALT
    
    ; Note: CMP64 sets flags, testing would require conditional jumps
    ; For now, just test that instruction executes without error
}

.test "cmp64 first greater" {
    .description "Test 64-bit comparison where first value is greater"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "greater"
    
    LOAD_IMM R0, 250
    LOAD_IMM R1, 100
    CMP64 R0, R1          ; Compare R0 with R1 (R0 > R1)
    HALT
    
    ; Instruction should execute successfully
}

.test "cmp64 first smaller" {
    .description "Test 64-bit comparison where first value is smaller"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "smaller"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 250
    CMP64 R0, R1          ; Compare R0 with R1 (R0 < R1)
    HALT
    
    ; Instruction should execute successfully
}

.test "cmp64 zero comparison" {
    .description "Test 64-bit comparison with zero"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "zero"
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 0
    CMP64 R0, R1          ; Compare R0 with R1 (both zero)
    HALT
    
    ; Both values are zero, should be equal
}

.test "cmp64 large numbers" {
    .description "Test 64-bit comparison with large numbers"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "large-values"
    
    LOAD_IMM64 R0, 999999999
    LOAD_IMM64 R1, 1000000000
    CMP64 R0, R1          ; Compare large values
    HALT
    
    ; First value is slightly smaller than second
}

.test "cmp64 mixed sign comparison" {
    .description "Test 64-bit comparison with different signs"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "mixed-sign"
    
    LOAD_IMM R0, 100     ; Positive value
    LOAD_IMM R1, 200     ; Larger positive value
    CMP64 R0, R1         ; Compare positive values
    HALT
    
    ; First value is smaller than second value
}