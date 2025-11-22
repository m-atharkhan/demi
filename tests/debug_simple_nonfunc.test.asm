# Simple non-function macro test
.define SIMPLE_NOP NOP

.test "debug_simple_nonfunc_macro" {
    .description "Debug simple non-function macro"
    .author "Debug"
    .category "Debug"
    
    SIMPLE_NOP
    HALT
    
    .assert_reg R0, 0
}