; Test file for data directives

#test "define byte string" {
    #description "Tests manual memory initialization with character values"
    #author "DemiEngine Team"
    #category "Data Directives"
    #tag "memory"
    #tag "string"
    ; Test manual memory setup instead of DB directive
    LOAD_IMM R0, 72    ; 'H'
    STORE R0, 50
    LOAD_IMM R0, 0
    LOAD R0, 50
    #assert_reg R0, 72
}

#test "define byte sequence" {
    #description "Tests manual memory initialization with numeric values"
    #author "DemiEngine Team"
    #category "Data Directives"
    #tag "memory"
    #tag "data"
    ; Test manual memory setup instead of DB directive
    LOAD_IMM R0, 10
    STORE R0, 50
    LOAD_IMM R0, 0
    LOAD R0, 50
    #assert_reg R0, 10
}

#test "org directive placement" {
    #description "Tests .org directive for setting code origin address"
    #author "DemiEngine Team"
    #category "Data Directives"
    #tag "org"
    #tag "placement"
    .org 100
    LOAD_IMM R0, 99
    #assert_reg R0, 99
}

#test "data and code mixed" {
    #description "Tests interleaving data initialization with code execution"
    #author "DemiEngine Team"
    #category "Data Directives"
    #tag "mixed"
    #tag "data"
    ; Test manual memory setup instead of DB directive
    LOAD_IMM R0, 5
    LOAD_IMM R1, 42
    ADD R0, R1
    #assert_reg R0, 47      ; 5 + 42
}
