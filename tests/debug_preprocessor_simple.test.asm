.include "includes/common.inc"

.test "debug_preprocessor_simple" {
    .description "Debug simple preprocessor functionality"
    .author "Debug"
    .category "Debug"
    
    # Simple test that should work
    LOAD_IMM R0, 42
    HALT
    
    .assert_reg R0, 42
}