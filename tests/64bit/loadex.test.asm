; 64-bit Extended Load Tests
; Tests for LOADEX instruction with various scenarios

.memory 2048  ; Allocate 2KB of memory for larger addresses

.test "loadex basic load" {
    .description "Basic 64-bit extended load test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    
    ; Store a value first, then load it
    LOAD_IMM64 R0, 123456
    STOREX R0, 1000        ; Store R0 at memory address 1000
    LOADEX R1, 1000        ; Load 64-bit value from address 1000 into R1
    HALT
    
    .assert_reg R1, 123456
}

.test "loadex zero value" {
    .description "Test extended load of zero value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "zero"
    
    LOAD_IMM R0, 0
    STOREX R0, 1100
    LOADEX R1, 1100        ; Load zero value
    HALT
    
    .assert_reg R1, 0
}

.test "loadex large value" {
    .description "Test extended load of large 64-bit value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "large-values"
    
    LOAD_IMM64 R0, 999999
    STOREX R0, 1200
    LOADEX R1, 1200        ; Load large value
    HALT
    
    .assert_reg R1, 999999
}

.test "loadex different addresses" {
    .description "Test loading from different memory addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "addressing"
    
    ; Store different values at different addresses
    LOAD_IMM R0, 111
    STOREX R0, 1300
    LOAD_IMM R0, 222
    STOREX R0, 1400
    LOAD_IMM R0, 77
    STOREX R0, 1500
    
    ; Load from second address
    LOADEX R1, 1400
    HALT
    
    .assert_reg R1, 222
}

.test "loadex sequential addresses" {
    .description "Test loading from sequential memory addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "sequential"
    
    ; Store values at sequential addresses
    LOAD_IMM R0, 42
    STOREX R0, 1600
    LOAD_IMM R0, 84
    STOREX R0, 1608  ; 8 bytes apart for 64-bit values
    
    ; Load from first address
    LOADEX R1, 1600
    HALT
    
    .assert_reg R1, 42
}