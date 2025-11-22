# No includes - define locally
.define ACCUMULATOR R1
.define LOAD_IMMEDIATE(reg, value) LOAD_IMM reg, value

.test "debug_no_include" {
    .description "Debug without includes"
    .author "Debug"
    .category "Debug"
    
    LOAD_IMMEDIATE(ACCUMULATOR, 42)
    HALT
    
    .assert_reg R1, 42
}