.include "includes/common.inc"

.test "debug_clear_reg_macro" {
    .description "Debug CLEAR_REG macro"
    .author "Debug"
    .category "Debug"
    
    CLEAR_REG(COUNTER)
    HALT
    
    .assert_reg R0, 0
}