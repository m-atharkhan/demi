# Simple include test

.include "includes/common.inc"

.test "include_test" {
    .description "Tests file inclusion"
    .author "Test Team"
    .category "Include Test"
    
    LOAD_IMM R0, TRUE
    HALT
    
    .assert_reg R0, 1  # TRUE should be 1
}