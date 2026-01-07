# Test Framework Status

## Overview

The DemiEngine test framework supports:

- Unit tests written in C++
- In-assembly tests embedded in `.asm` files via `.test { ... }`
- Negative tests (error conditions) covered by the unit test suite

## Test Types

### 1. Unit Tests (C++)

Unit tests are defined in `src/test/unit_tests.cpp` using the test framework macros:

```cpp
TEST_CASE(test_name, "category") {
    ctx.load_program({...});
    ctx.execute_program();
    ctx.assert_register_eq(0, 42);
}

TEST_CASE_EXPECT_ERROR(test_name, "category") {
    ctx.assemble_code(R"(
        ; Code that should fail
    )");
}
```

### 2. In-Assembly Tests

In-assembly tests are written directly in `.asm` files using `.test { ... }` blocks and `.assert_*` directives:

```asm
.test "Test Name" {
    LOAD_IMM R0, 10
    LOAD_IMM R1, 20
    ADD R0, R1

    .assert_reg R0, 30
    .assert_mem 0x100, 42
    .expect_error
}
```

## Current Negative Test Status

Negative tests are covered by the C++ unit test suite (category: `negative_tests`).

- Current status: 18/18 passing

## Running Tests

```bash
# Run all tests
./bin/demi-engine --test

# Unit tests only
./bin/demi-engine -ut

# In-assembly tests only
./bin/demi-engine -at

# Filter test output (filters output, not which tests execute)
./bin/demi-engine -ut --test-filter fails
./bin/demi-engine -ut --test-filter success   # alias: pass

./bin/demi-engine -at --test-filter fails
./bin/demi-engine -at --test-filter success   # alias: pass
```

## Current Test Status

- Unit tests: 145 passed, 0 failed (145 total)
- In-assembly tests: 371 passed, 3 skipped, 0 failed (374 total)
- Total: 516 passed, 3 skipped, 0 failed (519 total)

## Adding New Tests

### Adding a Unit Test

1. Open `src/test/unit_tests.cpp`
2. Add your test using `TEST_CASE` or `TEST_CASE_EXPECT_ERROR`
3. Rebuild: `make`
4. Run: `./bin/demi-engine -ut`

### Adding an In-Assembly Test

1. Create or edit a `.test.asm` file under `tests/`
2. Add a `.test "name" { ... }` block
3. Run: `./bin/demi-engine -at`
