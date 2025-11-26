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
    LOAD_IMM EAX, 42
    STORE EAX, 100

    # Store address 100 in register ECX
    LOAD_IMM ECX, 100

    # Use LOADR to load from address stored in ECX
    LOADR EDX, ECX

    # Test: EDX should now contain 42
    .assert_reg EDX, 42
}

.test "LOADR multiple addresses" {
    .description "Test LOADR with different memory addresses"
    .author "DemiEngine Team"
    .category "Core Instructions"
    .tag "memory"
    .tag "indirect"
    .tag "loadr"

    # Store value 123 at memory address 200
    LOAD_IMM ESI, 123
    STORE ESI, 200

    # Store address 200 in register ESP
    LOAD_IMM ESP, 200

    # Use LOADR to load from address stored in ESP
    LOADR EBP, ESP

    # Test: EBP should contain 123
    .assert_reg EBP, 123
}