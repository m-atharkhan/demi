; 64-bit Extended Store Tests
; Tests for STOREX instruction with various scenarios

.memory 2048  ; Allocate 2KB of memory for larger addresses

.test "storex basic store" {
    .description "Basic 64-bit extended store test"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    
    LOAD_IMM64 EAX, 987654
    STOREX EAX, 1000        ; Store 64-bit value from EAX to address 1000
    LOADEX EBX, 1000        ; Load it back to verify
    HALT
    
    .assert_reg EBX, 987654
}

.test "storex zero value" {
    .description "Test extended store of zero value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "zero"
    
    LOAD_IMM EAX, 0
    STOREX EAX, 1100
    LOADEX EBX, 1100        ; Load back to verify
    HALT
    
    .assert_reg EBX, 0
}

.test "storex large value" {
    .description "Test extended store of large 64-bit value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "large-values"
    
    LOAD_IMM64 EAX, 1234567
    STOREX EAX, 1200
    LOADEX EBX, 1200        ; Load back to verify
    HALT
    
    .assert_reg EBX, 1234567
}

.test "storex multiple addresses" {
    .description "Test storing to multiple different addresses"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "multiple"
    
    ; Store different values to different addresses
    LOAD_IMM EAX, 111
    STOREX EAX, 1300
    LOAD_IMM EAX, 222
    STOREX EAX, 1400
    LOAD_IMM EAX, 77
    STOREX EAX, 1500
    
    ; Load from middle address to verify
    LOADEX EBX, 1400
    HALT
    
    .assert_reg EBX, 222
}

.test "storex overwrite test" {
    .description "Test overwriting previously stored value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "overwrite"
    
    ; Store initial value
    LOAD_IMM EAX, 99
    STOREX EAX, 1600
    
    ; Overwrite with new value
    LOAD_IMM EAX, 123
    STOREX EAX, 1600
    
    ; Load back to verify overwrite
    LOADEX EBX, 1600
    HALT
    
    .assert_reg EBX, 123
}

.test "storex signed value" {
    .description "Test extended store of signed value"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "storex"
    .tag "signed"
    
    LOAD_IMM64 EAX, 12345   ; Positive value for now
    STOREX EAX, 1700
    LOADEX EBX, 1700       ; Load back to verify
    HALT
    
    .assert_reg EBX, 12345
}