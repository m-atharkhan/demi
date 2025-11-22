.test "debug_direct_load_imm" {
    .description "Debug direct LOAD_IMM without macros"
    .author "Debug"
    .category "Debug"
    
    LOAD_IMM R1, 42
    HALT
    
    .assert_reg R1, 42
}