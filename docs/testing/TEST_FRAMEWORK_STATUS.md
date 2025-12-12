# Test Framework Documentation

## Overview

The DemiEngine test framework provides comprehensive testing capabilities with support for:

- Unit tests written in C++
- Integration tests using hex files
- Negative tests (testing error conditions)
- In-assembly test cases (planned feature)

## Test Types

### 1. Unit Tests (C++)

Unit tests are defined in `src/test/unit_tests.cpp` using the test framework macros:

```cpp
// Regular test case
TEST_CASE(test_name, "category") {
    ctx.load_program({...});
    ctx.execute_program();
    ctx.assert_register_eq(0, 42);
}

// Negative test case (expects an error)
TEST_CASE_EXPECT_ERROR(test_name, "category") {
    ctx.assemble_code(R"(
        ; Code that should fail
    )");
}
```

### 2. Integration Tests (Hex Files)

Integration tests are hex files in the `tests/hex/` directory. They are automatically discovered and run with `--test` mode.

### 3. Negative Tests

Negative tests verify that the system properly detects and reports errors. These use `TEST_CASE_EXPECT_ERROR` and verify that:

- Invalid operations are caught
- Bounds checking works
- Type mismatches are detected
- Malformed input is rejected

## Current Negative Test Status

### ✅ Working Negative Tests (11/20)

- `invalid_register_number` - Catches invalid register numbers
- `undefined_label` - Catches undefined label references
- `invalid_opcode` - Catches invalid/unknown opcodes
- `stack_underflow` - Catches stack underflow
- `db_missing_length` - Catches missing DB length parameter
- `db_negative_length` - Catches negative DB length
- `duplicate_label` - Catches duplicate label definitions
- `malformed_instruction` - Catches malformed instructions
- `invalid_string_terminator` - Catches unterminated strings
- `register_type_mismatch` - Catches wrong register type usage
- `call_stack_overflow` - Catches excessive call depth

### ❌ Not Yet Implemented (9/20)

The following error conditions are not currently detected by the system:

- `memory_out_of_bounds_read` - Memory bounds checking on reads
- `memory_out_of_bounds_write` - Memory bounds checking on writes
- `stack_overflow` - Stack overflow detection
- `db_length_exceeds_string` - Warning when DB length > string length
- `invalid_immediate_value` - Out-of-range immediate values
- `org_backwards` - .org directive moving backwards
- `division_modulo_zero` - Modulo by zero detection
- `jump_to_invalid_address` - Jump to invalid memory address
- `ret_without_call` - RET without matching CALL

These represent opportunities for improving the assembler and CPU validation.

## In-Assembly Test Cases (Planned)

The test framework will support test cases written directly in assembly files using special directives:

```asm
#test "Test Name" {
    ; Test code here
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1

    ; Assertions
    #assert_reg R0, 30
    #assert_mem 0x100, 42
    #assert_output "Hello"
    #expect_error
}
```

### Test Directives

- `#test "name" { body }` - Define a test case
- `#assert_reg register, expected_value` - Assert register value
- `#assert_mem address, expected_value` - Assert memory value
- `#assert_output "string"` - Assert output string
- `#expect_error` - Expect the test to generate an error

### Status

The lexer and AST support for test directives has been implemented, but the parser and execution engine integration is still in progress.

## Running Tests

```bash
# Run all tests (unit + integration)
./bin/demi-engine --test

# Run with debug output
./bin/demi-engine --test --debug

# Run specific test category
# (Not yet implemented - would require code changes)
```

## Test Results

Current test status:

- **Unit Tests**: 70/79 passing (88.6%)
- **Integration Tests**: 42/42 passing (100%)
- **Negative Tests**: 11/20 working as designed

## Adding New Tests

### Adding a Unit Test

1. Open `src/test/unit_tests.cpp`
2. Add your test using `TEST_CASE` or `TEST_CASE_EXPECT_ERROR`
3. Rebuild: `make`
4. Run: `./bin/demi-engine --test`

### Adding an Integration Test

1. Create a `.hex` file in `tests/hex/`
2. Add test code as hex bytes
3. Run: `./bin/demi-engine --test`

### Adding a Negative Test

1. Open `src/test/unit_tests.cpp`
2. Add test using `TEST_CASE_EXPECT_ERROR`
3. The test passes if an error occurs during execution
4. The test fails if no error occurs

## Future Improvements

1. **Complete In-Assembly Test Support**
   - Parse #test blocks in assembly files
   - Execute tests with assertions
   - Integrate with test framework output

2. **Improve Error Detection**
   - Implement missing bounds checks
   - Add validation for .org directives
   - Enhance stack overflow detection
   - Add memory access validation

3. **Test Organization**
   - Category-based test filtering
   - Test dependency management
   - Parallel test execution
   - Test performance benchmarking

4. **Better Reporting**
   - Detailed failure diagnostics
   - Test coverage reports
   - Performance metrics
   - Regression tracking
