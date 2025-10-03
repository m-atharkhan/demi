; Test example showing in-assembly test syntax

#test "simple addition" {
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    ADD R0, R1
    #assert_reg R0, 8
}

#test "memory test" {
    LOAD_IMM R0, 42
    STORE R0, 10
    #assert_mem 10, 42
}

; Regular code
LOAD_IMM R0, 100
HALT
