.include "includes/math.inc"

.test "debug_no_aliases" {
    .description "Debug without register aliases"
    .author "Debug"
    .category "Debug"
    
    LOAD_IMMEDIATE(R1, 42)
    HALT
    
    .assert_reg R1, 42
}