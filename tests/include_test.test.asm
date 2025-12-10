# Simple include test

.include "includes/common.inc"

.test "include_test" {
    .description "Tests file inclusion"
    .author "bobrossrtx"
    .category "Include Test"
    
    LOAD_IMM EAX, TRUE
    HALT
    
    .assert_reg EAX, 1  # TRUE should be 1
}