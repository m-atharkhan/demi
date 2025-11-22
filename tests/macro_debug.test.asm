# Debug preprocessor macro expansion
.include "includes/common.inc"

.test "debug_macro_expansion" {
    .description "Debug macro expansion"
    .author "Debug"
    .category "Debug"
    
    # Test simple macro usage
    CLEAR_REG(COUNTER)
    HALT
    
    .assert_reg R0, 0
}
