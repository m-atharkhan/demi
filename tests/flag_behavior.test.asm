; Test: CPU Flag Behavior
; Category: Flags
; Description: Test flag setting in simple cases

.test "zero flag after SUB resulting in zero" {
    .description "SUB that results in zero should set zero flag"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 42
    LOAD_IMM R1, 42
    SUB R0, R1
    .assert_reg R0, 0
    halt
}

.test "zero flag after XOR self" {
    .description "XOR of register with itself sets zero flag"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 0xFFFFFFFF
    XOR R0, R0
    .assert_reg R0, 0
    halt
}

.test "CMP equal sets zero flag" {
    .description "CMP of equal values sets zero flag, enabling JE"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 100
    CMP R0, R1
    JE equal
    HALT
equal:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "CMP less-than clears zero flag" {
    .description "CMP of unequal values clears zero flag, enabling JNE"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 50
    LOAD_IMM R1, 100
    CMP R0, R1
    JNE not_equal
    HALT
not_equal:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "CMP greater-than enables JG" {
    .description "CMP with first operand greater enables JG"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 50
    CMP R0, R1
    JG greater
    HALT
greater:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "CMP less-than enables JL" {
    .description "CMP with first operand less enables JL"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 50
    LOAD_IMM R1, 100
    CMP R0, R1
    JL less
    HALT
less:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "CMP greater-or-equal enables JGE" {
    .description "CMP with first >= second enables JGE"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 100
    CMP R0, R1
    JGE greater_equal
    HALT
greater_equal:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "CMP less-or-equal enables JLE" {
    .description "CMP with first <= second enables JLE"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 50
    LOAD_IMM R1, 50
    CMP R0, R1
    JLE less_equal
    HALT
less_equal:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "ADD without overflow" {
    .description "ADD without overflow should not affect carry flag"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1
    .assert_reg R0, 30
    halt
}

.test "INC updates result" {
    .description "INC increments value correctly"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 10
    INC R0
    .assert_reg R0, 11
    halt
}

.test "DEC updates result" {
    .description "DEC decrements value correctly"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 10
    DEC R0
    .assert_reg R0, 9
    halt
}

.test "DEC to zero sets zero flag" {
    .description "Decrementing to zero sets zero flag"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 1
    DEC R0
    .assert_reg R0, 0
    halt
}

.test "multiple CMPs with different flags" {
    .description "Multiple comparisons set different flag states"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 50
    LOAD_IMM R1, 50
    CMP R0, R1
    JE first_equal
    HALT
    
first_equal:
    LOAD_IMM R0, 30
    LOAD_IMM R1, 60
    CMP R0, R1
    JL second_less
    HALT
    
second_less:
    LOAD_IMM R0, 80
    LOAD_IMM R1, 40
    CMP R0, R1
    JG third_greater
    HALT
    
third_greater:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "conditional jumps based on arithmetic" {
    .description "Conditional jumps work after arithmetic operations"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 10
    SUB R0, R1
    ; R0 is now 0
    
    LOAD_IMM R2, 100
    CMP R0, R2
    JL is_less
    HALT
    
is_less:
    LOAD_IMM R0, 1
    .assert_reg R0, 1
    halt
}

.test "flag preservation across operations" {
    .description "Flags set by CMP are preserved until next flag-setting op"
    .author "copilot"
    .category "Flags"
    
    LOAD_IMM R0, 100
    LOAD_IMM R1, 100
    CMP R0, R1
    ; Zero flag is set
    
    LOAD_IMM R2, 42
    ; Zero flag should still be set
    
    JE still_equal
    HALT
    
still_equal:
    .assert_reg R2, 42
    halt
}
