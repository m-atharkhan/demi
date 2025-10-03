; Test file for logical operations

#test "bitwise AND" {
    #description "Tests bitwise AND operation"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "basic"
    LOAD_IMM R0, 0xFF
    LOAD_IMM R1, 0x0F
    AND R0, R1
    #assert_reg R0, 0x0F
}

#test "bitwise OR" {
    #description "Tests bitwise OR operation"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "basic"
    LOAD_IMM R0, 0xF0
    LOAD_IMM R1, 0x0F
    OR R0, R1
    #assert_reg R0, 0xFF
}

#test "bitwise XOR" {
    #description "Tests bitwise XOR operation"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "basic"
    LOAD_IMM R0, 0xFF
    LOAD_IMM R1, 0xAA
    XOR R0, R1
    #assert_reg R0, 0x55
}

#test "bitwise NOT" {
    #description "Tests bitwise NOT unary operation"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "unary"
    LOAD_IMM R0, 0
    NOT R0
    #assert_reg R0, 255
}

#test "shift left" {
    #description "Tests left shift operation (multiply by power of 2)"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "basic"
    LOAD_IMM R0, 1
    SHL R0, 3
    #assert_reg R0, 8
}

#test "shift right" {
    #description "Tests right shift operation (divide by power of 2)"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "shift"
    #tag "basic"
    LOAD_IMM R0, 64
    SHR R0, 2
    #assert_reg R0, 16
}

#test "complex logical" {
    #description "Tests multiple logical operations in sequence"
    #author "DemiEngine Team"
    #category "Logical Operations"
    #tag "bitwise"
    #tag "complex"
    #tag "multi-step"
    LOAD_IMM R0, 0b11110000
    LOAD_IMM R1, 0b10101010
    AND R0, R1          ; R0 = 0b10100000
    LOAD_IMM R2, 0b00001111
    OR R0, R2           ; R0 = 0b10101111
    #assert_reg R0, 0xAF
}
