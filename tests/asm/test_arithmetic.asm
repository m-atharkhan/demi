; Test file for basic arithmetic operations

#test "addition" {
    #description "Tests basic addition of two numbers"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "basic"
    #tag "addition"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    #assert_reg R0, 15
}

#test "subtraction" {
    #description "Tests basic subtraction of two numbers"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "basic"
    #tag "subtraction"
    LOAD_IMM R0, 20
    LOAD_IMM R1, 7
    SUB R0, R1
    #assert_reg R0, 13
}

#test "multiplication" {
    #description "Tests multiplication of two numbers"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "basic"
    #tag "multiplication"
    LOAD_IMM R0, 6
    LOAD_IMM R1, 7
    MUL R0, R1
    #assert_reg R0, 42
}

#test "division" {
    #description "Tests division of two numbers"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "basic"
    #tag "division"
    LOAD_IMM R0, 100
    LOAD_IMM R1, 5
    DIV R0, R1
    #assert_reg R0, 20
}

#test "increment" {
    #description "Tests the INC instruction with multiple increments"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "increment"
    #tag "unary"
    LOAD_IMM R0, 10
    INC R0
    INC R0
    INC R0
    #assert_reg R0, 13
}

#test "decrement" {
    #description "Tests the DEC instruction with multiple decrements"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "decrement"
    #tag "unary"
    LOAD_IMM R0, 20
    DEC R0
    DEC R0
    #assert_reg R0, 18
}

#test "complex arithmetic" {
    #description "Tests multiple arithmetic operations in sequence"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "complex"
    #tag "multi-step"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    ADD R0, R1      ; R0 = 8
    LOAD_IMM R2, 2
    MUL R0, R2      ; R0 = 16
    LOAD_IMM R3, 4
    SUB R0, R3      ; R0 = 12
    #assert_reg R0, 12
}
