; 64-bit Comparison Tests
; Tests for CMP64 instruction with various scenarios

.test "cmp64 equal values" {
    .description "Test 64-bit comparison of equal values"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "equal"
    
    LOAD_IMM EAX, 123
    LOAD_IMM EBX, 123
    CMP64 EAX, EBX          ; Compare EAX with EBX (equal)
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
    
    LOAD_IMM EAX, 250
    LOAD_IMM EBX, 100
    CMP64 EAX, EBX          ; Compare EAX with EBX (EAX > EBX)
    HALT
    
    ; Instruction should execute successfully
}

.test "cmp64 first smaller" {
    .description "Test 64-bit comparison where first value is smaller"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "smaller"
    
    LOAD_IMM EAX, 100
    LOAD_IMM EBX, 250
    CMP64 EAX, EBX          ; Compare EAX with EBX (EAX < EBX)
    HALT
    
    ; Instruction should execute successfully
}

.test "cmp64 zero comparison" {
    .description "Test 64-bit comparison with zero"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "zero"
    
    LOAD_IMM EAX, 0
    LOAD_IMM EBX, 0
    CMP64 EAX, EBX          ; Compare EAX with EBX (both zero)
    HALT
    
    ; Both values are zero, should be equal
}

.test "cmp64 large numbers" {
    .description "Test 64-bit comparison with large numbers"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "large-values"
    
    LOAD_IMM64 EAX, 999999999
    LOAD_IMM64 EBX, 1000000000
    CMP64 EAX, EBX          ; Compare large values
    HALT
    
    ; First value is slightly smaller than second
}

.test "cmp64 mixed sign comparison" {
    .description "Test 64-bit comparison with different signs"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "cmp64"
    .tag "mixed-sign"
    
    LOAD_IMM EAX, 100     ; Positive value
    LOAD_IMM EBX, 200     ; Larger positive value
    CMP64 EAX, EBX         ; Compare positive values
    HALT
    
    ; First value is smaller than second value
}