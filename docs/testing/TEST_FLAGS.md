# DemiEngine Test Flags Documentation

Complete guide to using DemiEngine's comprehensive test framework with command-line flags.

## Table of Contents

- [Overview](#overview)
- [Test Flag Summary](#test-flag-summary)
- [Detailed Usage](#detailed-usage)
  - [--test / -t](#--test---t)
  - [--unit-test / -ut](#--unit-test---ut)
  - [--assembly-test / -at](#--assembly-test---at)
  - [--assembly-test-quiet / -atq](#--assembly-test-quiet---atq)
- [File-Specific Testing](#file-specific-testing)
- [Test Output Examples](#test-output-examples)
- [Best Practices](#best-practices)

---

## Overview

DemiEngine provides a flexible test framework with **188 total tests** achieving **100% coverage**:

| Test Suite            | Count | Description                                               |
| --------------------- | ----- | --------------------------------------------------------- |
| **Unit Tests**        | 78    | Core functionality validation (CPU, memory, instructions) |
| **Assembly Tests**    | 68    | In-assembly test validation with metadata                 |
| **Integration Tests** | 42    | Real program execution from hex files                     |

**Key Feature**: All test flags support **optional file arguments**, allowing you to run all tests of a type OR test a single specific file.

---

## Test Flag Summary

```bash
# Run all tests
./bin/demi-engine --test                    # All unit tests (78)

# Run specific test suites
./bin/demi-engine --unit-test               # Unit tests only (78)
./bin/demi-engine --assembly-test           # All assembly tests (68)
./bin/demi-engine --assembly-test-quiet     # Assembly tests, quiet mode (68)

# Test specific files
./bin/demi-engine -t <file>                 # Test specific file
./bin/demi-engine -ut <file>                # Test specific file (unit test mode)
```

---

## Detailed Usage

### `--test` / `-t`

**Description**: The primary testing flag. Without arguments, it runs the full unit test suite. With a file argument, it runs tests for that specific file.

**Usage**:

```bash
./bin/demi-engine -t
./bin/demi-engine -t tests/arithmetic.test.asm
```

### `--unit-test` / `-ut`

**Description**: Runs the C++ unit test suite. These tests validate the internal components of the engine (CPU, Memory, Assembler) directly.

**Usage**:

```bash
./bin/demi-engine -ut
```

**Features tested**:

- Assembler functionality
- Parser operations
- CPU instruction validation
- Memory bounds checking
- Error handling
- Negative test cases

### `--assembly-test` / `-at`

**Description**: Runs the "In-Assembly" test suite. These are `.asm` files that contain self-validating code using `.assert` directives.

**Usage**:

```bash
./bin/demi-engine -at
```

**Use when:**

- Testing core assembler/parser changes
- Quick feedback on code changes
- CI/CD pipelines (fastest test suite)

**Average runtime:** ~1-2 seconds

### `--assembly-test-quiet` / `-atq`

**Description**: Same as `-at`, but suppresses detailed output, showing only the final summary and any failures.

**Usage**:

```bash
./bin/demi-engine -atq
```

---

## File-Specific Testing

You can run tests for a single file using the `-t` or `-ut` flags. This is useful when developing a new feature or debugging a specific issue.

```bash
# Run tests for a specific assembly file
./bin/demi-engine -t tests/my_new_feature.asm
```

---

## Best Practices

1. **Run Unit Tests First**: Always run `-ut` before committing changes.
2. **Use Assembly Tests for Logic**: Use `-at` to verify that instructions behave as expected in a real program.
3. **Test Specific Files**: When working on a specific feature, run only the relevant test file to save time.
