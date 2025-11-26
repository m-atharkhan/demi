; Test file for data storage and retrieval functionality
; These tests verify data handling capabilities using STORE/LOAD instructions and DB directives

.test "basic data storage and retrieval" {
    .description "Tests basic STORE and LOAD operations with single byte value"
    .author "DemiEngine Team"
    .category "Data"
    .tag "basic"
    .tag "load"
    .tag "store"
    
    ; Test storing and loading a single byte value
    LOAD_IMM EAX, 72    ; Load ASCII value for 'H' 
    STORE EAX, 100      ; Store it at address 100
    
    ; Clear the register and load it back
    LOAD_IMM EAX, 0     ; Clear EAX
    LOAD EAX, 100       ; Load the stored value
    .assert_reg EAX, 72 ; Should be 'H' (72)
}

.test "string data storage" {
    .description "Tests storing and loading string data character by character"
    .author "DemiEngine Team"
    .category "Data"
    .tag "string"
    .tag "characters"
    .tag "ascii"
    
    ; Test storing and loading string data character by character
    LOAD_IMM EAX, 72    ; 'H'
    STORE EAX, 50
    LOAD_IMM EAX, 101   ; 'e' 
    STORE EAX, 51
    LOAD_IMM EAX, 108   ; 'l'
    STORE EAX, 52
    LOAD_IMM EAX, 108   ; 'l'
    STORE EAX, 53
    LOAD_IMM EAX, 111   ; 'o'
    STORE EAX, 54
    LOAD_IMM EAX, 0     ; null terminator
    STORE EAX, 55
    
    ; Read back the string characters
    LOAD EBX, 50        ; Load 'H'
    LOAD ECX, 51        ; Load 'e'
    LOAD EDX, 52        ; Load 'l'
    LOAD ESI, 53        ; Load 'l'
    LOAD EDI, 54        ; Load 'o'
    LOAD ESP, 55        ; Load null terminator
    
    .assert_reg EBX, 72  ; 'H'
    .assert_reg ECX, 101 ; 'e'
    .assert_reg EDX, 108 ; 'l'
    .assert_reg ESI, 108 ; 'l'
    .assert_reg EDI, 111 ; 'o'
    .assert_reg ESP, 0   ; null terminator
}

.test "multiple data values at different addresses" {
    .description "Tests storing multiple different data types at various memory locations"
    .author "DemiEngine Team"
    .category "Data"
    .tag "multi-location"
    .tag "types"
    .tag "addresses"
    
    ; Test storing multiple different data types
    LOAD_IMM EAX, 65    ; 'A'
    STORE EAX, 200
    LOAD_IMM EAX, 123   ; numeric value
    STORE EAX, 201  
    LOAD_IMM EAX, 255   ; max byte value
    STORE EAX, 202
    LOAD_IMM EAX, 0     ; zero value
    STORE EAX, 203
    
    ; Load and verify each value
    LOAD EBX, 200
    LOAD ECX, 201
    LOAD EDX, 202
    LOAD ESI, 203
    
    .assert_reg EBX, 65  ; 'A'
    .assert_reg ECX, 123 ; numeric value
    .assert_reg EDX, 255 ; max byte value  
    .assert_reg ESI, 0   ; zero value
}

.test "data overwrite and update" {
    .description "Tests overwriting stored data with new values"
    .author "DemiEngine Team"
    .category "Data"
    .tag "overwrite"
    .tag "update"
    .tag "modify"
    
    ; Test overwriting stored data
    LOAD_IMM EAX, 100   ; Initial value
    STORE EAX, 150
    
    ; Verify initial storage
    LOAD EBX, 150
    .assert_reg EBX, 100
    
    ; Overwrite with new value
    LOAD_IMM EAX, 200   ; New value
    STORE EAX, 150
    
    ; Verify overwrite
    LOAD ECX, 150
    .assert_reg ECX, 200
}

.test "basic DB directive test" {
    .description "Tests that DB directive works within test framework"
    .author "DemiEngine Team"
    .category "Data"
    .tag "db"
    .tag "data"
    
    ; Define some data using DB instruction
    DB 'Hello', 5, 0
    .org 0x20
    
    ; Test that we can load the data
    LOAD EAX, 0      ; Load first byte 'H' (72)
    LOAD EBX, 1      ; Load second byte 'e' (101)
    LOAD ECX, 4      ; Load fifth byte 'o' (111)
    LOAD EDX, 5      ; Load length byte (5)
    LOAD ESI, 6      ; Load null terminator (0)
    
    .assert_reg EAX, 72   ; 'H'
    .assert_reg EBX, 101  ; 'e'
    .assert_reg ECX, 111  ; 'o'
    .assert_reg EDX, 5    ; length
    .assert_reg ESI, 0    ; null terminator
    
    HALT
}

.test "DB with mixed data" {
    .description "Tests DB directive with mixed data types"
    .author "DemiEngine Team" 
    .category "Data"
    .tag "db"
    .tag "mixed-data"
    
    ; Mixed data using DB instruction
    DB 'Test', 4, 100
    .org 0x20
    
    ; Test loading the data from memory address 0 where DB placed it
    LOAD EAX, 0      ; 'T' (84)
    LOAD EBX, 1      ; 'e' (101)
    LOAD ECX, 2      ; 's' (115)
    LOAD EDX, 3      ; 't' (116)
    
    .assert_reg EAX, 84   ; 'T'
    .assert_reg EBX, 101  ; 'e'
    .assert_reg ECX, 115  ; 's'
    .assert_reg EDX, 116  ; 't'
    
    HALT
}

.test "DB with byte values" {
    .description "Test DB directive with numeric byte values"
    .author "DemiEngine Team"
    .category "Data"
    .tag "db"
    .tag "bytes"

    ; Define some byte data
    DB 42, 13, 255, 0, 100
    .org 0x20
    
    ; Load and test the byte values
    LOAD EAX, 0  ; First byte (42)
    LOAD EBX, 1  ; Second byte (13)
    LOAD ECX, 2  ; Third byte (255)
    LOAD EDX, 3  ; Fourth byte (0)
    LOAD ESI, 4  ; Fifth byte (100)

    .assert_reg EAX, 42
    .assert_reg EBX, 13
    .assert_reg ECX, 255
    .assert_reg EDX, 0
    .assert_reg ESI, 100
    
    HALT
}

.test "DB string data" {
    .description "Test DB directive with string data"
    .author "DemiEngine Team"
    .category "Data"
    .tag "db"
    .tag "string"

    ; Define string data using DB
    DB 'World', 0
    .org 0x20
    
    ; Test individual characters
    LOAD EAX, 0  ; 'W' = 87
    LOAD EBX, 1  ; 'o' = 111
    LOAD ECX, 2  ; 'r' = 114
    LOAD EDX, 3  ; 'l' = 108
    LOAD ESI, 4  ; 'd' = 100
    LOAD EDI, 5  ; null terminator = 0

    .assert_reg EAX, 87   ; 'W'
    .assert_reg EBX, 111  ; 'o'
    .assert_reg ECX, 114  ; 'r'
    .assert_reg EDX, 108  ; 'l'
    .assert_reg ESI, 100  ; 'd'
    .assert_reg EDI, 0    ; null terminator
    
    HALT
}

.test "align directive test" {
    .description "Test .align directive for data alignment"
    .author "DemiEngine Team"
    .category "Data"
    .tag "align"

    ; Manually set unaligned position (within test memory limits)
    .org 0x41
    
    ; Test alignment to 4-byte boundary
    .align 4
    
    ; Store a value at the aligned address (should be at 0x44)
    LOAD_IMM EAX, 42
    STORE EAX, 0x44
    LOAD EBX, 0x44
    
    .assert_reg EBX, 42
    
    HALT
}

.test "bss directive test" {
    .description "Test .bss directive for zero-initialized memory"
    .author "DemiEngine Team"
    .category "Data"
    .tag "bss"

    ; Reserve 10 bytes of zero-initialized memory
    .bss 10
    
    ; Test that the memory is initialized to zero
    LOAD EAX, 0    ; First byte should be 0
    LOAD EBX, 5    ; Middle byte should be 0  
    LOAD ECX, 9    ; Last byte should be 0
    
    .assert_reg EAX, 0
    .assert_reg EBX, 0
    .assert_reg ECX, 0
    
    HALT
}