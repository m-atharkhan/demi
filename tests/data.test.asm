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
    LOAD_IMM R0, 72    ; Load ASCII value for 'H' 
    STORE R0, 100      ; Store it at address 100
    
    ; Clear the register and load it back
    LOAD_IMM R0, 0     ; Clear R0
    LOAD R0, 100       ; Load the stored value
    .assert_reg R0, 72 ; Should be 'H' (72)
}

.test "string data storage" {
    .description "Tests storing and loading string data character by character"
    .author "DemiEngine Team"
    .category "Data"
    .tag "string"
    .tag "characters"
    .tag "ascii"
    
    ; Test storing and loading string data character by character
    LOAD_IMM R0, 72    ; 'H'
    STORE R0, 50
    LOAD_IMM R0, 101   ; 'e' 
    STORE R0, 51
    LOAD_IMM R0, 108   ; 'l'
    STORE R0, 52
    LOAD_IMM R0, 108   ; 'l'
    STORE R0, 53
    LOAD_IMM R0, 111   ; 'o'
    STORE R0, 54
    LOAD_IMM R0, 0     ; null terminator
    STORE R0, 55
    
    ; Read back the string characters
    LOAD R1, 50        ; Load 'H'
    LOAD R2, 51        ; Load 'e'
    LOAD R3, 52        ; Load 'l'
    LOAD R4, 53        ; Load 'l'
    LOAD R5, 54        ; Load 'o'
    LOAD R6, 55        ; Load null terminator
    
    .assert_reg R1, 72  ; 'H'
    .assert_reg R2, 101 ; 'e'
    .assert_reg R3, 108 ; 'l'
    .assert_reg R4, 108 ; 'l'
    .assert_reg R5, 111 ; 'o'
    .assert_reg R6, 0   ; null terminator
}

.test "multiple data values at different addresses" {
    .description "Tests storing multiple different data types at various memory locations"
    .author "DemiEngine Team"
    .category "Data"
    .tag "multi-location"
    .tag "types"
    .tag "addresses"
    
    ; Test storing multiple different data types
    LOAD_IMM R0, 65    ; 'A'
    STORE R0, 200
    LOAD_IMM R0, 123   ; numeric value
    STORE R0, 201  
    LOAD_IMM R0, 255   ; max byte value
    STORE R0, 202
    LOAD_IMM R0, 0     ; zero value
    STORE R0, 203
    
    ; Load and verify each value
    LOAD R1, 200
    LOAD R2, 201
    LOAD R3, 202
    LOAD R4, 203
    
    .assert_reg R1, 65  ; 'A'
    .assert_reg R2, 123 ; numeric value
    .assert_reg R3, 255 ; max byte value  
    .assert_reg R4, 0   ; zero value
}

.test "data overwrite and update" {
    .description "Tests overwriting stored data with new values"
    .author "DemiEngine Team"
    .category "Data"
    .tag "overwrite"
    .tag "update"
    .tag "modify"
    
    ; Test overwriting stored data
    LOAD_IMM R0, 100   ; Initial value
    STORE R0, 150
    
    ; Verify initial storage
    LOAD R1, 150
    .assert_reg R1, 100
    
    ; Overwrite with new value
    LOAD_IMM R0, 200   ; New value
    STORE R0, 150
    
    ; Verify overwrite
    LOAD R2, 150
    .assert_reg R2, 200
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
    LOAD R0, 0      ; Load first byte 'H' (72)
    LOAD R1, 1      ; Load second byte 'e' (101)
    LOAD R2, 4      ; Load fifth byte 'o' (111)
    LOAD R3, 5      ; Load null terminator (0)
    
    .assert_reg R0, 72   ; 'H'
    .assert_reg R1, 101  ; 'e'
    .assert_reg R2, 111  ; 'o'
    .assert_reg R3, 0    ; null terminator
    
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
    LOAD R0, 0      ; 'T' (84)
    LOAD R1, 1      ; 'e' (101)
    LOAD R2, 2      ; 's' (115)
    LOAD R3, 3      ; 't' (116)
    
    .assert_reg R0, 84   ; 'T'
    .assert_reg R1, 101  ; 'e'
    .assert_reg R2, 115  ; 's'
    .assert_reg R3, 116  ; 't'
    
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
    LOAD R0, 0  ; First byte (42)
    LOAD R1, 1  ; Second byte (13)
    LOAD R2, 2  ; Third byte (255)
    LOAD R3, 3  ; Fourth byte (0)
    LOAD R4, 4  ; Fifth byte (100)

    .assert_reg R0, 42
    .assert_reg R1, 13
    .assert_reg R2, 255
    .assert_reg R3, 0
    .assert_reg R4, 100
    
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
    LOAD R0, 0  ; 'W' = 87
    LOAD R1, 1  ; 'o' = 111
    LOAD R2, 2  ; 'r' = 114
    LOAD R3, 3  ; 'l' = 108
    LOAD R4, 4  ; 'd' = 100
    LOAD R5, 5  ; null terminator = 0

    .assert_reg R0, 87   ; 'W'
    .assert_reg R1, 111  ; 'o'
    .assert_reg R2, 114  ; 'r'
    .assert_reg R3, 108  ; 'l'
    .assert_reg R4, 100  ; 'd'
    .assert_reg R5, 0    ; null terminator
    
    HALT
}

.test "dot db directive test" {
    .description "Test .db directive syntax (assembler directive, not instruction)"
    .author "DemiEngine Team"
    .category "Data"
    .tag "db-directive"

    ; Try using .db assembler directive
    .db 65, 66, 67, 0  ; "ABC" with null terminator
    
    ; Test loading the data
    LOAD R0, 256  ; First byte should be 'A' (65)
    LOAD R1, 257  ; Second byte should be 'B' (66)
    LOAD R2, 258  ; Third byte should be 'C' (67)
    LOAD R3, 259  ; Fourth byte should be null (0)

    .assert_reg R0, 65   ; 'A'
    .assert_reg R1, 66   ; 'B'
    .assert_reg R2, 67   ; 'C'
    .assert_reg R3, 0    ; null terminator
    
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
    LOAD_IMM R0, 42
    STORE R0, 0x44
    LOAD R1, 0x44
    
    .assert_reg R1, 42
    
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
    LOAD R0, 0    ; First byte should be 0
    LOAD R1, 5    ; Middle byte should be 0  
    LOAD R2, 9    ; Last byte should be 0
    
    .assert_reg R0, 0
    .assert_reg R1, 0
    .assert_reg R2, 0
    
    HALT
}