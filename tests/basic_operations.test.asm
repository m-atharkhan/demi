; Simple test to verify basic opcodes are working

.test "basic_add" {
    .description "Test basic ADD operation"
    .author "bobrossrtx"
    .category "Arithmetic"
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 3
    ADD EAX, EBX
    .assert_reg EAX, 8
}

.test "basic_mul" {
    .description "Test basic MUL operation"
    .author "bobrossrtx"
    .category "Arithmetic"
    LOAD_IMM EAX, 4
    LOAD_IMM EBX, 3
    MUL EAX, EBX
    .assert_reg EAX, 12
}
