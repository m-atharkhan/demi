; Test AND logical operation
#test "test_and_operation" {
    #description "Test AND logical operation with various values"
    #category "Debug"
    #tag "logical"
    
    ; Test 1: Basic AND operation
    LOAD_IMM R0, 0b11110000  ; 240
    LOAD_IMM R1, 0b10101010  ; 170  
    AND R0, R1               ; Should be 0b10100000 = 160
    #assert_reg R0, 160
    
    ; Test 2: AND with all bits set
    LOAD_IMM R2, 0xFF        ; 255
    LOAD_IMM R3, 0x55        ; 85
    AND R2, R3               ; Should be 85
    #assert_reg R2, 85
    
    ; Test 3: AND with zero
    LOAD_IMM R4, 0xFF        ; 255
    LOAD_IMM R5, 0x00        ; 0
    AND R4, R5               ; Should be 0
    #assert_reg R4, 0
    
    HALT
}