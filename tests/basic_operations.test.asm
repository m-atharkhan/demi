; Simple test to verify basic opcodes are working

.test "basic_add" {
    .description "Test basic ADD operation"
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    ADD R0, R1
    .assert_reg R0, 8
}

.test "basic_mul" {
    .description "Test basic MUL operation" 
    LOAD_IMM R0, 4
    LOAD_IMM R1, 3
    MUL R0, R1
    .assert_reg R0, 12
}
