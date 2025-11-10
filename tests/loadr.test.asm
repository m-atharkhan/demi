# Test LOADR instruction - indirect memory addressing
.test "LOADR basic functionality" {
    .description "Test LOADR instruction for indirect addressing"
    .author "DemiEngine Team"
    .category "Core Instructions"
    .tag "memory"
    .tag "indirect"
    .tag "loadr"

    # Test 1: Basic LOADR functionality
    # Store value 42 at memory address 100
    LOAD_IMM R0, 42
    STORE R0, 100

    # Store address 100 in register R2
    LOAD_IMM R2, 100

    # Use LOADR to load from address stored in R2
    LOADR R3, R2

    # Test: R3 should now contain 42
    .assert_reg R3, 42
}

.test "LOADR multiple addresses" {
    .description "Test LOADR with different memory addresses"
    .author "DemiEngine Team"
    .category "Core Instructions"
    .tag "memory"
    .tag "indirect"
    .tag "loadr"

    # Store value 123 at memory address 200
    LOAD_IMM R4, 123
    STORE R4, 200

    # Store address 200 in register R6
    LOAD_IMM R6, 200

    # Use LOADR to load from address stored in R6
    LOADR R7, R6

    # Test: R7 should contain 123
    .assert_reg R7, 123
}