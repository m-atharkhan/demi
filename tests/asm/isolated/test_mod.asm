#test "benchmark_prime_check_test1" {
    #description "Test MOD instruction"
    #author "DemiEngine Team"
    #category "Benchmarks"
    
    LOAD_IMM R0, 17
    LOAD_IMM R2, 1
    
    MOV R3, R0
    MOD R3, 2
    
    #assert_reg R2, 1
    HALT
}