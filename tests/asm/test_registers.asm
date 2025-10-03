; Test file for register operations

#test "register to register move" {
    #description "Tests MOV instruction for copying values between registers"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "basic"
    #tag "move"
    LOAD_IMM R0, 100
    MOV R1, R0
    #assert_reg R0, 100
    #assert_reg R1, 100
}

#test "multiple register operations" {
    #description "Tests loading immediate values into multiple registers"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "basic"
    #tag "load-immediate"
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    LOAD_IMM R2, 30
    LOAD_IMM R3, 40
    
    #assert_reg R0, 10
    #assert_reg R1, 20
    #assert_reg R2, 30
    #assert_reg R3, 40
}

#test "register independence" {
    #description "Tests that operations on one register don't affect other registers"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "independence"
    #tag "arithmetic"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    #assert_reg R0, 15
    #assert_reg R1, 10    ; R1 should be unchanged
}

#test "using extended registers" {
    #description "Tests using registers beyond R0-R5 (extended register set)"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "extended"
    #tag "basic"
    LOAD_IMM R6, 111
    LOAD_IMM R7, 222
    #assert_reg R6, 111
    #assert_reg R7, 222
}

#test "register wrap on operations" {
    #description "Tests that register values wrap around at 255/256 boundary"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "overflow"
    #tag "wrap"
    LOAD_IMM R0, 255
    INC R0
    #assert_reg R0, 0    ; Should wrap to 0
}

#test "zero register" {
    #description "Tests setting a register to zero"
    #author "DemiEngine Team"
    #category "Registers"
    #tag "basic"
    #tag "zero"
    LOAD_IMM R0, 123
    LOAD_IMM R0, 0
    #assert_reg R0, 0
}
