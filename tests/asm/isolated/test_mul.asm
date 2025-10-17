#test "test_mul" {
    #description "Test MUL instruction"
    #author "DemiEngine Team"
    #category "Benchmarks"
    
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    
    MUL R1, R0
    
    #assert_reg R1, 15
    HALT
}