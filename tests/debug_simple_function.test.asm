# Simple function macro test
.define SIMPLE_LOAD(reg) LOAD_IMM reg, 42

.test "debug_simple_function_macro" {
    .description "Debug simple function macro"
    .author "Debug"
    .category "Debug"
    
    SIMPLE_LOAD(R1)
    HALT
    
    .assert_reg R1, 42
}