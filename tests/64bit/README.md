# 64-bit Operations Test Suite

This directory contains comprehensive tests for all 64-bit operations in the DemiEngine VM. All tests use the `.test` directive format for proper integration with the test framework.

## Test Files

### Individual Operation Tests

- `mul64_tests.asm` - Tests for 64-bit multiplication (MUL64)
- `div64_tests.asm` - Tests for 64-bit division (DIV64)
- `and64_tests.asm` - Tests for 64-bit bitwise AND (AND64)
- `cmp64_tests.asm` - Tests for 64-bit comparison (CMP64)
- `loadex_tests.asm` - Tests for extended 64-bit load (LOADEX)
- `storex_tests.asm` - Tests for extended 64-bit store (STOREX)

### Combined Operation Tests

- `combined_tests.asm` - Multi-operation test scenarios

## Test Coverage

Each test file includes:

- ✅ Basic functionality tests
- ✅ Edge case handling (zero, large values)
- ✅ Error conditions (division by zero)
- ✅ Memory operations validation
- ✅ Negative number handling
- ✅ Boundary condition testing

## Running Tests

### Run all 64-bit tests:

```bash
./bin/demi-engine --assembly-test tests/64bit/
```

### Run specific test file:

```bash
./bin/demi-engine --assembly-test tests/64bit/mul64_tests.asm
```

### Run with quiet mode (less verbose output):

```bash
./bin/demi-engine --assembly-test-quiet tests/64bit/
```

## Test Framework Integration

All tests use the standard `.test` directive format:

```assembly
.test "test name" {
    .description "What this test does"
    .author "DemiEngine Team"
    .category "64-bit Operations"
    .tag "operation-type"

    ; Test code here
    LOAD_IMM R0, 100
    MUL64 R1, R0, R0
    HALT

    .assert_reg R1, 10000
}
```

## Expected Results

All tests should pass if the 64-bit operations are implemented correctly. The test framework will:

1. Parse each `.test` directive
2. Assemble and execute the test code
3. Validate assertions against actual register/memory values
4. Report success/failure with detailed information

## Test Statistics

- **Total Tests**: ~35 test cases
- **Operations Covered**: 6 (MUL64, DIV64, AND64, CMP64, LOADEX, STOREX)
- **Test Categories**: Basic, Edge Cases, Error Handling, Combined Operations
- **Expected Runtime**: < 2 seconds for full suite
