# Ultra simple test
.define MYVAL 42

.test "debug_test" {
    .description "Debug test"
    .author "Test"
    .category "Debug"
    
    LOAD_IMM R0, MYVAL
    HALT
    
    .assert_reg R0, 42
}