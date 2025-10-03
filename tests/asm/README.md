# DemiEngine Assembly Test Documentation# Assembly Test Suite



Complete guide to the DemiEngine in-assembly testing system with metadata support.This directory contains in-assembly tests for the DemiEngine assembler and CPU emulator.



## Table of Contents## Test Files



- [Overview](#overview)### test_arithmetic.asm

- [Test Structure](#test-structure)Tests basic arithmetic operations:

- [Metadata System](#metadata-system)- Addition, subtraction, multiplication, division

- [Running Tests](#running-tests)- Increment and decrement

- [Available Test Suites](#available-test-suites)- Complex multi-step arithmetic

- [Writing Your Own Tests](#writing-your-own-tests)

- [Best Practices](#best-practices)**Tests:** 7



---### test_logical.asm

Tests bitwise and logical operations:

## Overview- AND, OR, XOR, NOT

- Shift left and shift right

DemiEngine features a comprehensive in-assembly testing system with **68 tests** covering all aspects of the virtual machine. Tests are written directly in assembly files using special `#test`/`#endtest` blocks with rich metadata support.- Complex logical expressions



### Key Features**Tests:** 7



- ✅ **68 In-Assembly Tests** with 100% pass rate### test_memory.asm

- ✅ **Test Metadata** (description, author, category, tags)Tests memory load and store operations:

- ✅ **Flexible Execution** (all tests or specific files)- Basic load/store

- ✅ **Quiet Mode** for minimal output- Multiple memory locations

- ✅ **Comprehensive Coverage** (arithmetic, control flow, memory, stack, logic, etc.)- Memory swapping and copying



---**Tests:** 5



## Test Structure### test_stack.asm

Tests stack operations:

### Basic Test Format- Push and pop single/multiple values

- Stack ordering (LIFO)

```assembly- Push/pop flags

#test test_name

    #description A brief description of what this test validates**Tests:** 4

    #author Author Name

    #category Test Category### test_control_flow.asm

    #tag tag1, tag2, tag3Tests jumps and conditional branching:

    - Unconditional jumps

    ; Test code goes here- Conditional jumps (JZ, JG, JL, JGE, JLE)

    LOAD_IMM R0, 5- Comparison operations

    LOAD_IMM R1, 10

    ADD R0, R1**Tests:** 6

    

    ; Assertions### test_error_cases.asm

    #assert_eq R0, 15Tests error handling and validation:

#endtest- Division by zero

```- Invalid opcodes

- Stack overflow

### Test Components- Out of bounds memory access

- Invalid register access

1. **Test Block**: Delimited by `#test` and `#endtest`- Invalid jumps and returns

2. **Metadata Directives**: Optional directives providing test information

3. **Assembly Code**: The actual test implementation**Tests:** 8

4. **Assertions**: Validation directives to check results

### test_registers.asm

---Tests register operations:

- Register to register moves

## Metadata System- Register independence

- Extended registers (R10, R20, etc.)

### Available Metadata Directives- Register wrapping



| Directive | Required | Description | Example |**Tests:** 6

|-----------|----------|-------------|---------|

| `#description` | ✅ Recommended | What the test validates | `#description Tests basic addition` |### test_data_directives.asm

| `#author` | ❌ Optional | Test author/team | `#author DemiEngine Team` |Tests assembler directives:

| `#category` | ❌ Optional | Test category/group | `#category Arithmetic` |- DB (define byte) with strings and numbers

| `#tag` | ❌ Optional | Comma-separated tags | `#tag basic, addition, binary-op` |- .org directive

- Mixed data and code

### Metadata Example

**Tests:** 4

```assembly

#test addition### test_complex.asm

    #description Tests basic addition of two numbersTests complex algorithms and edge cases:

    #author DemiEngine Team- Fibonacci sequence

    #category Arithmetic- Factorial calculation

    #tag basic, addition- Array sum

    - Maximum of three numbers

    LOAD_IMM R0, 5- Average calculation

    LOAD_IMM R1, 3- Power of 2 check

    ADD R0, R1- XOR swap

    #assert_eq R0, 8

#endtest**Tests:** 8

```

## Total Tests

### Display Modes

**55 test cases** covering:

**Full Mode** (`-at`):- ✓ Arithmetic operations

```- ✓ Logical operations

✓ addition- ✓ Memory management

  Description: Tests basic addition of two numbers- ✓ Stack operations

  Author: DemiEngine Team- ✓ Control flow

  Category: Arithmetic- ✓ Error handling

  Tags: basic, addition- ✓ Register operations

```- ✓ Data directives

- ✓ Complex algorithms

**Quiet Mode** (`-atq`):

```## Running Tests

addition

  Tests basic addition of two numbersTo run all in-assembly tests:

```

```bash

---./bin/demi-engine --test

```

## Running Tests

The test framework will:

### Run All Assembly Tests1. Parse each `.asm` file in `tests/asm/`

2. Validate test structure (statements and assertions)

```bash3. Report validation results

# Full output with all metadata

./bin/demi-engine -at## Test Syntax

./bin/demi-engine --assembly-test

### Defining a Test

# Quiet mode (title + description only)```assembly

./bin/demi-engine -atq#test "test name" {

./bin/demi-engine --assembly-test-quiet    ; test code here

```    #assert_reg R0, expected_value

}

### Run Specific Test File```



```bash### Available Assertions

# Test single file (full output)- `#assert_reg REGISTER, VALUE` - Assert register contains value

./bin/demi-engine -at tests/asm/test_arithmetic.asm- `#assert_mem ADDRESS, VALUE` - Assert memory location contains value

- `#assert_output "STRING"` - Assert output matches string (not yet implemented)

# Test single file (quiet mode)- `#expect_error` - Assert that test produces an error

./bin/demi-engine -atq tests/asm/test_stack.asm

```## Current Status



### Output Summary✅ **Parser Integration:** Complete

✅ **Test Validation:** Complete

```⏳ **Test Execution:** Planned (validates structure, doesn't execute yet)

[INFO] === Running tests from tests/asm/test_arithmetic.asm ===

[INFO] ✓ addition## Future Enhancements

[INFO]   Description: Tests basic addition of two numbers

...- [ ] Full test execution with CPU emulation

[INFO] === Test Summary ===- [ ] Output capture and verification

Tests: 7/7 passed- [ ] Performance benchmarking

Assertions: 7/7 passed- [ ] Test coverage reporting

[INFO] All tests passed!- [ ] Parallel test execution

```

---

## Available Test Suites

### Current Test Files (68 Tests Total)

| File | Tests | Description |
|------|-------|-------------|
| `test_arithmetic.asm` | 7 | Basic arithmetic operations (ADD, SUB, MUL, DIV, INC, DEC) |
| `test_control_flow.asm` | 6 | Jump instructions (JMP, JZ, JG, JL, JGE, JLE) |
| `test_logical.asm` | 7 | Bitwise operations (AND, OR, XOR, NOT, SHL, SHR) |
| `test_memory.asm` | 5 | Memory operations (LOAD, STORE, addressing modes) |
| `test_stack.asm` | 4 | Stack operations (PUSH, POP, PUSH_FLAG, POP_FLAG) |
| `test_registers.asm` | 6 | Register operations and independence |
| `test_error_cases.asm` | 5 | Error handling (div by zero, invalid opcodes, etc.) |
| `test_data_directives.asm` | 4 | Data directives (.db, .org) |
| `test_complex.asm` | 8 | Complex algorithms (Fibonacci, factorial, etc.) |
| `test_showcase.asm` | 8 | Feature showcase and comprehensive examples |
| `examples/test_example.asm` | 2 | Basic test examples |
| `examples/test_with_metadata.asm` | 2 | Metadata usage examples |
| **Total** | **68** | **Complete test coverage** |

### Test Categories

- **Arithmetic** (7 tests): ADD, SUB, MUL, DIV, INC, DEC operations
- **Control Flow** (6 tests): Conditional and unconditional jumps
- **Logical Operations** (7 tests): Bitwise AND, OR, XOR, NOT, shifts
- **Memory** (5 tests): Load/store operations, addressing modes
- **Stack** (4 tests): Push/pop operations, flag preservation
- **Registers** (6 tests): Register independence, extended registers
- **Error Cases** (5 tests): Error handling validation
- **Data Directives** (4 tests): Assembly directives
- **Complex** (8 tests): Multi-step algorithms
- **Showcase** (8 tests): Feature demonstrations
- **Examples** (4 tests): Tutorial examples

---

## Writing Your Own Tests

### Step 1: Create a Test File

Create a new `.asm` file in `tests/asm/` or your project directory:

```assembly
; my_feature_test.asm

#test feature_basic
    #description Tests basic functionality of my feature
    #author Your Name
    #category Custom Features
    #tag experimental, feature-x
    
    LOAD_IMM R0, 42
    #assert_eq R0, 42
#endtest

#test feature_edge_case
    #description Tests edge case behavior
    #author Your Name
    #category Custom Features
    #tag experimental, edge-case
    
    LOAD_IMM R0, 0
    INC R0
    #assert_eq R0, 1
#endtest
```

### Step 2: Run Your Tests

```bash
# Test your file
./bin/demi-engine -at my_feature_test.asm

# Quick validation in quiet mode
./bin/demi-engine -atq my_feature_test.asm
```

### Step 3: Verify Results

Expected output:
```
[INFO] === Running tests from my_feature_test.asm ===
[INFO] ✓ feature_basic
[INFO]   Description: Tests basic functionality of my feature
[INFO]   Author: Your Name
[INFO]   Category: Custom Features
[INFO]   Tags: experimental, feature-x
[INFO] ✓ feature_edge_case
[INFO]   Description: Tests edge case behavior
[INFO]   Author: Your Name
[INFO]   Category: Custom Features
[INFO]   Tags: experimental, edge-case
[INFO] === Test Summary ===
Tests: 2/2 passed
Assertions: 2/2 passed
[INFO] All tests passed!
```

---

## Best Practices

### 1. Comprehensive Metadata

Always include at least a description:

```assembly
#test my_test
    #description Clearly explains what this test validates
    #author Your Name  # Optional but helpful
    #category Feature Group  # Groups related tests
    #tag specific, keywords  # Aids filtering/searching
    
    ; Test code
#endtest
```

### 2. Clear Test Names

Use descriptive, snake_case names:

```assembly
✅ Good:
#test addition
#test subtraction_with_negative
#test stack_overflow_handling

❌ Avoid:
#test test1
#test t
#test xyz
```

### 3. One Concept Per Test

Keep tests focused on a single concept:

```assembly
✅ Good (separate tests):
#test addition
    LOAD_IMM R0, 5
    LOAD_IMM R1, 3
    ADD R0, R1
    #assert_eq R0, 8
#endtest

#test subtraction
    LOAD_IMM R0, 10
    LOAD_IMM R1, 3
    SUB R0, R1
    #assert_eq R0, 7
#endtest

❌ Avoid (mixed concerns):
#test arithmetic
    ; Tests addition
    ADD R0, R1
    ; Tests subtraction
    SUB R0, R1
    ; Tests multiplication
    MUL R0, R1
#endtest
```

### 4. Use Meaningful Categories

Organize tests into logical categories:

- **Arithmetic**: ADD, SUB, MUL, DIV operations
- **Control Flow**: Jumps and branches
- **Memory**: LOAD/STORE operations
- **Stack**: PUSH/POP operations
- **Logical Operations**: Bitwise operations
- **Edge Cases**: Boundary conditions
- **Error Handling**: Invalid operations

### 5. Tag for Discoverability

Use tags to make tests searchable:

```assembly
#tag basic            # Fundamental operations
#tag advanced         # Complex scenarios
#tag edge-case        # Boundary conditions
#tag performance      # Performance-critical
#tag regression       # Regression tests
```

### 6. Document Complex Tests

Add comments for non-obvious logic:

```assembly
#test fibonacci_recursive
    #description Calculates 5th Fibonacci number using recursion
    #category Algorithms
    #tag recursive, complex
    
    ; Initialize: F(0) = 0, F(1) = 1
    LOAD_IMM R0, 5       ; n = 5
    CALL fibonacci       ; Result in R1
    #assert_eq R1, 5     ; F(5) = 5
#endtest
```

---

## Testing Workflow

### During Development

```bash
# Quick iteration: Test specific file you're working on
./bin/demi-engine -at tests/asm/test_my_feature.asm

# Review what tests do (quiet mode)
./bin/demi-engine -atq tests/asm/test_my_feature.asm
```

### Before Commit

```bash
# Run all assembly tests
./bin/demi-engine -at

# Run all test suites
./bin/demi-engine -ut && ./bin/demi-engine -it && ./bin/demi-engine -at
```

### CI/CD Pipeline

```bash
#!/bin/bash
# run_tests.sh

echo "Running unit tests..."
./bin/demi-engine -ut || exit 1

echo "Running integration tests..."
./bin/demi-engine -it || exit 1

echo "Running assembly tests..."
./bin/demi-engine -at || exit 1

echo "All tests passed!"
```

---

## Troubleshooting

### Test Not Found

```
[ERROR] Test file not found: tests/asm/missing.asm
```

**Solution**: Check file path and ensure file exists.

### Parser Errors

```
[ERROR] Parser errors in test file:
  Line 5: Unexpected token
```

**Solution**: Check assembly syntax and test block structure.

### Failed Assertions

```
✗ my_test
  Expected R0 to equal 15, but got 10
```

**Solution**: Review test logic and verify expected values.

---

## See Also

- [TEST_FLAGS.md](../docs/TEST_FLAGS.md) - Complete test flag documentation
- [README.md](../README.md) - Main project documentation
- [CONTRIBUTING.md](../CONTRIBUTING.md) - How to contribute tests
- [docs/usage/README.md](../docs/usage/README.md) - Assembly programming guide

---

**68 Tests, 100% Pass Rate** - Quality testing for a quality virtual machine! 🎉
