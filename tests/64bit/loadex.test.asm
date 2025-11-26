; 64-bit Extended Load Tests
; Tests for LOADEX instruction with various scenarios

.memory 2048  ; Allocate 2KB of memory for larger addresses

.test "loadex basic load" {
    .description "Basic 64-bit extended load test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    
    ; Store a value first, then load it
    LOAD_IMM64 EAX, 123456
    STOREX EAX, 1000        ; Store EAX at memory address 1000
    LOADEX EBX, 1000        ; Load 64-bit value from address 1000 into EBX
    HALT
    
    .assert_reg EBX, 123456
}

.test "loadex zero value" {
    .description "Test extended load of zero value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "zero"
    
    LOAD_IMM EAX, 0
    STOREX EAX, 1100
    LOADEX EBX, 1100        ; Load zero value
    HALT
    
    .assert_reg EBX, 0
}

.test "loadex large value" {
    .description "Test extended load of large 64-bit value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "large-values"
    
    LOAD_IMM64 EAX, 999999
    STOREX EAX, 1200
    LOADEX EBX, 1200        ; Load large value
    HALT
    
    .assert_reg EBX, 999999
}

.test "loadex different addresses" {
    .description "Test loading from different memory addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "addressing"
    
    ; Store different values at different addresses
    LOAD_IMM EAX, 111
    STOREX EAX, 1300
    LOAD_IMM EAX, 222
    STOREX EAX, 1400
    LOAD_IMM EAX, 77
    STOREX EAX, 1500
    
    ; Load from second address
    LOADEX EBX, 1400
    HALT
    
    .assert_reg EBX, 222
}

.test "loadex sequential addresses" {
    .description "Test loading from sequential memory addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "loadex"
    .tag "sequential"
    
    ; Store values at sequential addresses
    LOAD_IMM EAX, 42
    STOREX EAX, 1600
    LOAD_IMM EAX, 84
    STOREX EAX, 1608  ; 8 bytes apart for 64-bit values
    
    ; Load from first address
    LOADEX EBX, 1600
    HALT
    
    .assert_reg EBX, 42
}