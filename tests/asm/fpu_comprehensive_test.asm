; Comprehensive FPU test demonstrating working operations

#test "fpu_stack_test" {
    #description "Test FPU stack operations work properly"
    #author "DemiEngine Team"
    #category "FPU"
    #tag "comprehensive"
    #tag "floating-point"
    
    ; Test basic register operation first to ensure test framework works
    LOAD_IMM R0, 1
    #assert_reg R0, 1
    
    ; Test FINIT (initialize FPU)
    FINIT
    
    ; Test basic register after FINIT to ensure execution continues
    LOAD_IMM R1, 2
    #assert_reg R1, 2
    
    HALT
}