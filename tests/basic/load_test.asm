#test "basic register load test" {
    LOAD_IMM R0, 42         ; Load immediate value
    #assert_reg R0, 42      ; Verify register contains expected value
    HALT
}
