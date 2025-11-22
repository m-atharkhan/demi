.include "includes/math.inc"

.test "debug_load_immediate" {
    .description "Debug LOAD_IMMEDIATE macro"
    .author "Debug"
    .category "Debug"
    
    LOAD_IMMEDIATE(ACCUMULATOR, 42)
    HALT
    
    .assert_reg R1, 42
}