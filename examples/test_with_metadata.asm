; Example test file demonstrating test metadata features

#test "addition with metadata" {
    #description "Tests basic addition of two numbers and verifies the result"
    #author "DemiEngine Team"
    #category "Arithmetic"
    #tag "basic"
    #tag "addition"
    
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    ADD R0, R1
    #assert_reg R0, 8
}

#test "simple test without metadata" {
    LOAD_IMM R0, 10
    #assert_reg R0, 10
}

; Example test file demonstrating test metadata features
; 
; Metadata directives allow you to document your tests with:
; - #description "text" - A detailed description of what the test does
; - #author "name" - Who wrote the test
; - #category "name" - Logical grouping (e.g., "Arithmetic", "Memory", "Control Flow")
; - #tag "label" - Multiple tags for filtering and organization
;
; All metadata values must be enclosed in double quotes.

#test "basic metadata example" {
    #description "Demonstrates basic test metadata with a simple assertion"
    #author "DemiEngine Team"
    #category "Tutorial"
    #tag "beginner"
    #tag "example"
    
    LOAD_IMM R0, 42
    #assert_reg R0, 42
}

#test "test without metadata" {
    ; Tests can still work without any metadata
    LOAD_IMM R1, 100
    #assert_reg R1, 100
}

#test "complex metadata example" {
    #description "Shows how metadata helps document more complex test scenarios"
    #author "QA Team"
    #category "Integration"
    #tag "arithmetic"
    #tag "multi-step"
    #tag "regression"
    
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1
    #assert_reg R0, 30
}

#test "memory operations example" {
    #description "Tests memory store and load operations"
    #category "Memory"
    #tag "memory"
    
    LOAD_IMM R0, 123
    STORE R0, 50
    LOAD_IMM R1, 0
    LOAD R1, 50
    #assert_reg R1, 123
}
