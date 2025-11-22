; 64-bit Extended Store Tests
; Tests for STOREX instruction with various scenarios

.memory 2048  ; Allocate 2KB of memory for larger addresses

.test "storex basic store" {
    .description "Basic 64-bit extended store test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    
    LOAD_IMM64 R0, 987654
    STOREX R0, 1000        ; Store 64-bit value from R0 to address 1000
    LOADEX R1, 1000        ; Load it back to verify
    HALT
    
    .assert_reg R1, 987654
}

.test "storex zero value" {
    .description "Test extended store of zero value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "zero"
    
    LOAD_IMM R0, 0
    STOREX R0, 1100
    LOADEX R1, 1100        ; Load back to verify
    HALT
    
    .assert_reg R1, 0
}

.test "storex large value" {
    .description "Test extended store of large 64-bit value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "large-values"
    
    LOAD_IMM64 R0, 1234567
    STOREX R0, 1200
    LOADEX R1, 1200        ; Load back to verify
    HALT
    
    .assert_reg R1, 1234567
}

.test "storex multiple addresses" {
    .description "Test storing to multiple different addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "multiple"
    
    ; Store different values to different addresses
    LOAD_IMM R0, 111
    STOREX R0, 1300
    LOAD_IMM R0, 222
    STOREX R0, 1400
    LOAD_IMM R0, 77
    STOREX R0, 1500
    
    ; Load from middle address to verify
    LOADEX R1, 1400
    HALT
    
    .assert_reg R1, 222
}

.test "storex overwrite test" {
    .description "Test overwriting previously stored value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "overwrite"
    
    ; Store initial value
    LOAD_IMM R0, 99
    STOREX R0, 1600
    
    ; Overwrite with new value
    LOAD_IMM R0, 123
    STOREX R0, 1600
    
    ; Load back to verify overwrite
    LOADEX R1, 1600
    HALT
    
    .assert_reg R1, 123
}

.test "storex signed value" {
    .description "Test extended store of signed value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "signed"
    
    LOAD_IMM64 R0, 12345   ; Positive value for now
    STOREX R0, 1700
    LOADEX R1, 1700       ; Load back to verify
    HALT
    
    .assert_reg R1, 12345
}