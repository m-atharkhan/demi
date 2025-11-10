# Simple preprocessor test without includes

.define TEST_VAL 42
.define ADD_TEN(reg) LOAD_IMM R1, 10\nADD reg, R1

.test "simple_macro_test" {
    .description "Simple macro expansion test"
    .author "Test Team"
    .category "Macro Test"
    
    LOAD_IMM R0, TEST_VAL
    ADD_TEN(R0)
    HALT
    
    .assert_reg R0, 52  # 42 + 10 = 52
}