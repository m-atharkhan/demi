; Simple test to verify test framework works

#test "simple_test" {
    #description "Simple register test"
    #author "DemiEngine Team"
    #category "debug"
    #tag "basic"
    
    ; Test basic register operation
    LOAD_IMM R0, 42
    #assert_reg R0, 42
    HALT
}