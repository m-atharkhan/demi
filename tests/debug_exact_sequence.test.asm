.include "includes/math.inc"

.test "debug_exact_sequence" {
    .description "Debug exact sequence from failing test"
    .author "Debug"
    .category "Debug"
    
    CLEAR_REG(COUNTER)
    LOAD_IMMEDIATE(ACCUMULATOR, 42)
    COPY_REG(COUNTER, ACCUMULATOR)
    HALT
    
    .assert_reg R0, 42
    .assert_reg R1, 42
}