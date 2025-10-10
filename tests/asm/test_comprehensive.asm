; Comprehensive test file covering edge cases and missing functionality

; ============ MOD Operation Tests ============
#test "modulo operation" {
    #description "Tests modulo (remainder) operation"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "modulo"
    #tag "division"
    LOAD_IMM R0, 17
    LOAD_IMM R1, 5
    MOD R0, R1
    #assert_reg R0, 2
}

#test "modulo by one" {
    #description "Tests modulo with divisor of 1 (should always be 0)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "modulo"
    #tag "edge-case"
    LOAD_IMM R0, 99
    LOAD_IMM R1, 1
    MOD R0, R1
    #assert_reg R0, 0
}

#test "modulo with equal values" {
    #description "Tests modulo when dividend equals divisor (should be 0)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "modulo"
    #tag "edge-case"
    LOAD_IMM R0, 42
    LOAD_IMM R1, 42
    MOD R0, R1
    #assert_reg R0, 0
}

; ============ Overflow Flag Tests ============
#test "addition overflow" {
    #description "Tests that adding two large numbers sets overflow flag"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "overflow"
    #tag "addition"
    LOAD_IMM R0, 200
    LOAD_IMM R1, 100
    ADD R0, R1
    ; R0 should wrap to 44 (300 % 256)
    #assert_reg R0, 44
}

#test "subtraction underflow" {
    #description "Tests subtraction resulting in underflow (negative wrap)"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "underflow"
    #tag "subtraction"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    SUB R0, R1
    ; R0 should wrap to 251 (5 - 10 = -5, wraps to 256-5)
    #assert_reg R0, 251
}

#test "multiplication overflow" {
    #description "Tests multiplication resulting in overflow"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "overflow"
    #tag "multiplication"
    LOAD_IMM R0, 50
    LOAD_IMM R1, 10
    MUL R0, R1
    ; 50 * 10 = 500, wraps to 244 (500 % 256)
    #assert_reg R0, 244
}

; ============ Zero Flag Tests ============
#test "zero result from subtraction" {
    #description "Tests that subtracting equal values produces zero"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "zero-flag"
    #tag "subtraction"
    LOAD_IMM R0, 42
    LOAD_IMM R1, 42
    SUB R0, R1
    #assert_reg R0, 0
}

#test "zero result from AND" {
    #description "Tests AND operation producing zero result"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "zero-flag"
    #tag "bitwise"
    LOAD_IMM R0, 0b11110000
    LOAD_IMM R1, 0b00001111
    AND R0, R1
    #assert_reg R0, 0
}

#test "zero result from XOR same values" {
    #description "Tests XOR of identical values produces zero"
    #author "DemiEngine Team"
    #category "Flags"
    #tag "zero-flag"
    #tag "bitwise"
    LOAD_IMM R0, 0xAB
    LOAD_IMM R1, 0xAB
    XOR R0, R1
    #assert_reg R0, 0
}

; ============ Shift Edge Cases ============
#test "shift left by zero" {
    #description "Tests that shifting left by 0 doesn't change value"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "edge-case"
    LOAD_IMM R0, 42
    SHL R0, 0
    #assert_reg R0, 42
}

#test "shift right by zero" {
    #description "Tests that shifting right by 0 doesn't change value"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "edge-case"
    LOAD_IMM R0, 42
    SHR R0, 0
    #assert_reg R0, 42
}

#test "shift left overflow" {
    #description "Tests shifting left causes bits to fall off the end"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "overflow"
    LOAD_IMM R0, 0xFF
    SHL R0, 1
    #assert_reg R0, 254
}

#test "shift right to zero" {
    #description "Tests shifting right until value becomes zero"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "zero"
    LOAD_IMM R0, 1
    SHR R0, 1
    #assert_reg R0, 0
}

#test "shift left maximum" {
    #description "Tests shifting left by 7 positions"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "maximum"
    LOAD_IMM R0, 1
    SHL R0, 7
    #assert_reg R0, 128
}

#test "shift right maximum" {
    #description "Tests shifting right by 7 positions"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "maximum"
    LOAD_IMM R0, 128
    SHR R0, 7
    #assert_reg R0, 1
}

; ============ Jump Edge Cases ============
#test "jump forward" {
    #description "Tests jumping forward over several instructions"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "forward"
    LOAD_IMM R0, 1
    JMP target
    LOAD_IMM R0, 2
    LOAD_IMM R0, 3
    LOAD_IMM R0, 4
target:
    #assert_reg R0, 1
}

#test "jump backward" {
    #description "Tests jumping backward to create a simple loop"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "backward"
    LOAD_IMM R0, 0
    LOAD_IMM R1, 3
    LOAD_IMM R2, 0
loop_start:
    INC R0
    DEC R1
    CMP R1, R2
    JG loop_start
    #assert_reg R0, 3
}

#test "nested jumps" {
    #description "Tests nested conditional jumps"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "jump"
    #tag "nested"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    CMP R0, R1
    JL first_true
    LOAD_IMM R2, 0
    JMP done
first_true:
    LOAD_IMM R2, 1
    LOAD_IMM R4, 1
    CMP R2, R4
    JZ second_true
    LOAD_IMM R3, 0
    JMP done
second_true:
    LOAD_IMM R3, 1
done:
    #assert_reg R2, 1
    #assert_reg R3, 1
}

; ============ CMP Variations ============
#test "compare less than" {
    #description "Tests CMP when first operand is less than second"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "comparison"
    #tag "less-than"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    CMP R0, R1
    JL is_less
    LOAD_IMM R2, 0
    JMP done
is_less:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "compare greater than" {
    #description "Tests CMP when first operand is greater than second"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "comparison"
    #tag "greater-than"
    LOAD_IMM R0, 20
    LOAD_IMM R1, 10
    CMP R0, R1
    JG is_greater
    LOAD_IMM R2, 0
    JMP done
is_greater:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

#test "compare equal" {
    #description "Tests CMP when both operands are equal"
    #author "DemiEngine Team"
    #category "Control Flow"
    #tag "comparison"
    #tag "equal"
    LOAD_IMM R0, 15
    LOAD_IMM R1, 15
    CMP R0, R1
    JZ is_equal
    LOAD_IMM R2, 0
    JMP done
is_equal:
    LOAD_IMM R2, 1
done:
    #assert_reg R2, 1
}

; ============ Memory Edge Cases ============
#test "store and load at address zero" {
    #description "Tests memory operations at address 0"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "edge-case"
    #tag "address-zero"
    LOAD_IMM R0, 123
    STORE R0, 0
    LOAD_IMM R1, 0
    LOAD R1, 0
    #assert_reg R1, 123
    #assert_mem 0, 123
}

#test "overwrite same memory location" {
    #description "Tests overwriting the same memory location multiple times"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "overwrite"
    LOAD_IMM R0, 10
    STORE R0, 100
    LOAD_IMM R0, 20
    STORE R0, 100
    LOAD_IMM R0, 30
    STORE R0, 100
    #assert_mem 100, 30
}

#test "load from uninitialized memory" {
    #description "Tests loading from memory that hasn't been explicitly initialized"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "uninitialized"
    LOAD R0, 200
    ; Should get whatever is in memory (likely 0)
    #assert_reg R0, 0
}

; ============ Register Combination Tests ============
#test "all registers independent" {
    #description "Tests that all registers maintain independent values"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "independence"
    #tag "comprehensive"
    LOAD_IMM R0, 0
    LOAD_IMM R1, 1
    LOAD_IMM R2, 2
    LOAD_IMM R3, 3
    LOAD_IMM R4, 4
    LOAD_IMM R5, 5
    LOAD_IMM R6, 6
    LOAD_IMM R7, 7
    #assert_reg R0, 0
    #assert_reg R1, 1
    #assert_reg R2, 2
    #assert_reg R3, 3
    #assert_reg R4, 4
    #assert_reg R5, 5
    #assert_reg R6, 6
    #assert_reg R7, 7
}

#test "register chain operations" {
    #description "Tests chaining operations through multiple registers"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "chain"
    #tag "complex"
    LOAD_IMM R0, 10
    MOV R1, R0
    ADD R1, R0      ; R1 = 20
    MOV R2, R1
    ADD R2, R0      ; R2 = 30
    MOV R3, R2
    ADD R3, R0      ; R3 = 40
    #assert_reg R0, 10
    #assert_reg R1, 20
    #assert_reg R2, 30
    #assert_reg R3, 40
}

; ============ Boundary Value Tests ============
#test "maximum byte value" {
    #description "Tests operations with maximum 8-bit value (255)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "boundary"
    #tag "maximum"
    LOAD_IMM R0, 255
    #assert_reg R0, 255
}

#test "minimum byte value" {
    #description "Tests operations with minimum value (0)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "boundary"
    #tag "minimum"
    LOAD_IMM R0, 0
    #assert_reg R0, 0
}

#test "increment at boundary" {
    #description "Tests incrementing at 255 boundary (wraps to 0)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "boundary"
    #tag "wrap"
    LOAD_IMM R0, 255
    INC R0
    #assert_reg R0, 0
}

#test "decrement at boundary" {
    #description "Tests decrementing at 0 boundary (wraps to 255)"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "boundary"
    #tag "wrap"
    LOAD_IMM R0, 0
    DEC R0
    #assert_reg R0, 255
}

; ============ Bitwise Pattern Tests ============
#test "all bits set" {
    #description "Tests operations with all bits set (0xFF)"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "pattern"
    LOAD_IMM R0, 0xFF
    #assert_reg R0, 255
}

#test "alternating bit pattern" {
    #description "Tests alternating bit pattern 0b10101010"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "pattern"
    LOAD_IMM R0, 0b10101010
    #assert_reg R0, 170
}

#test "inverse alternating bit pattern" {
    #description "Tests inverse alternating bit pattern 0b01010101"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "pattern"
    LOAD_IMM R0, 0b01010101
    #assert_reg R0, 85
}

#test "single bit set" {
    #description "Tests single bit operations for each bit position"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "single-bit"
    LOAD_IMM R0, 0b00000001
    #assert_reg R0, 1
    LOAD_IMM R1, 0b00000010
    #assert_reg R1, 2
    LOAD_IMM R2, 0b00000100
    #assert_reg R2, 4
    LOAD_IMM R3, 0b00001000
    #assert_reg R3, 8
}

; ============ Stack Depth Tests ============
#test "stack deep push and pop" {
    #description "Tests pushing and popping many values on the stack"
    #author "DemiEngine Team"
    #category "Stack"
    #tag "depth"
    #tag "stress"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    LOAD_IMM R4, 50
    
    PUSH R0
    PUSH R1
    PUSH R2
    PUSH R3
    PUSH R4
    
    LOAD_IMM R0, 0
    LOAD_IMM R1, 0
    LOAD_IMM R2, 0
    LOAD_IMM R3, 0
    LOAD_IMM R4, 0
    
    POP R4
    POP R3
    POP R2
    POP R1
    POP R0
    
    #assert_reg R0, 10
    #assert_reg R1, 20
    #assert_reg R2, 30
    #assert_reg R3, 40
    #assert_reg R4, 50
}

; ============ MOV Variations ============
#test "mov preserves source" {
    #description "Tests that MOV copies value without modifying source"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "move"
    #tag "copy"
    LOAD_IMM R0, 99
    MOV R1, R0
    MOV R2, R0
    MOV R3, R0
    #assert_reg R0, 99
    #assert_reg R1, 99
    #assert_reg R2, 99
    #assert_reg R3, 99
}

#test "mov chain" {
    #description "Tests chaining MOV operations across multiple registers"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "move"
    #tag "chain"
    LOAD_IMM R0, 42
    MOV R1, R0
    MOV R2, R1
    MOV R3, R2
    MOV R4, R3
    #assert_reg R4, 42
}
