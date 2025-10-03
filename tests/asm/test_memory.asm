; Test file for memory operations

#test "load and store" {
    #description "Tests basic LOAD and STORE memory operations"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "basic"
    #tag "load"
    #tag "store"
    LOAD_IMM R0, 42
    STORE R0, 10
    LOAD_IMM R1, 0
    LOAD R1, 10
    #assert_reg R1, 42
    #assert_mem 10, 42
}

#test "multiple memory locations" {
    #description "Tests writing to multiple distinct memory addresses"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "store"
    #tag "multi-location"
    LOAD_IMM R0, 10
    STORE R0, 20
    LOAD_IMM R0, 20
    STORE R0, 21
    LOAD_IMM R0, 30
    STORE R0, 22
    #assert_mem 20, 10
    #assert_mem 21, 20
    #assert_mem 22, 30
}

#test "memory swap using registers" {
    #description "Tests swapping values between memory locations using register intermediates"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "load"
    #tag "store"
    #tag "swap"
    LOAD_IMM R0, 5
    STORE R0, 30
    LOAD_IMM R1, 15
    STORE R1, 31
    
    ; Swap values
    LOAD R2, 30
    LOAD R3, 31
    STORE R3, 30
    STORE R2, 31
    
    #assert_mem 30, 15
    #assert_mem 31, 5
}

#test "memory copy" {
    #description "Tests copying a value from one memory location to another"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "load"
    #tag "store"
    #tag "copy"
    LOAD_IMM R0, 99
    STORE R0, 40
    LOAD R1, 40
    STORE R1, 50
    #assert_mem 40, 99
    #assert_mem 50, 99
}

#test "memory clear" {
    #description "Tests overwriting a memory location with zero"
    #author "DemiEngine Team"
    #category "Memory"
    #tag "store"
    #tag "clear"
    LOAD_IMM R0, 100
    STORE R0, 60
    LOAD_IMM R0, 0
    STORE R0, 60
    #assert_mem 60, 0
}
