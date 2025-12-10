; Example test file demonstrating test metadata features

.test "addition with metadata" {
    .description "Tests basic addition of two numbers and verifies the result"
    .author "bobrossrtx"
    .category "Arithmetic"
    .tag "basic"
    .tag "addition"
    
    LOAD_IMM EAX, 5
    LOAD_IMM EBX, 3
    ADD EAX, EBX
    .assert_reg EAX, 8
}

.test "simple test without metadata" {
    .description "Simple test to verify register loading"
    .author "bobrossrtx"
    .category "Basic"
    LOAD_IMM EAX, 10
    .assert_reg EAX, 10
}

; Example test file demonstrating test metadata features
; 
; Metadata directives allow you to document your tests with:
; - .description "text" - A detailed description of what the test does
; - .author "name" - Who wrote the test
; - .category "name" - Logical grouping (e.g., "Arithmetic", "Memory", "Control Flow")
; - .tag "label" - Multiple tags for filtering and organization
;
; All metadata values must be enclosed in double quotes.

.test "basic metadata example" {
    .description "Demonstrates basic test metadata with a simple assertion"
    .author "bobrossrtx"
    .category "Tutorial"
    .tag "beginner"
    .tag "example"
    
    LOAD_IMM EAX, 42
    .assert_reg EAX, 42
}

.test "test without metadata" {
    .description "Tests can still work without any metadata"
    .author "bobrossrtx"
    .category "Basic"
    LOAD_IMM EBX, 100
    .assert_reg EBX, 100
}

.test "complex metadata example" {
    .description "Shows how metadata helps document more complex test scenarios"
    .author "bobrossrtx"
    .category "Integration"
    .tag "arithmetic"
    .tag "multi-step"
    .tag "regression"
    
    LOAD_IMM EAX, 10
    LOAD_IMM EBX, 20
    ADD EAX, EBX
    .assert_reg EAX, 30
}

.test "memory operations example" {
    .description "Tests memory store and load operations"
    .author "bobrossrtx"
    .category "Memory"
    .tag "memory"
    
    LOAD_IMM EAX, 123
    STORE EAX, 50
    LOAD_IMM EBX, 0
    LOAD EBX, 50
    .assert_reg EBX, 123
}
