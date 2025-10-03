# Testing Documentation

This directory contains all testing-related documentation for DemiEngine, including test framework design, test writing guides, and testing status reports.

## Contents

### Core Testing Documents

- **[TEST_FRAMEWORK_DESIGN.md](TEST_FRAMEWORK_DESIGN.md)** - Complete test framework architecture and design
- **[TEST_QUICK_REFERENCE.md](TEST_QUICK_REFERENCE.md)** - Quick guide for running and writing tests
- **[TEST_FLAGS.md](TEST_FLAGS.md)** - CPU flags testing documentation and examples

### Status & Implementation

- **[TEST_FRAMEWORK_STATUS.md](TEST_FRAMEWORK_STATUS.md)** - Current testing status and coverage (188/188 tests passing)
- **[TEST_ENHANCEMENT_SUMMARY.md](TEST_ENHANCEMENT_SUMMARY.md)** - Summary of test framework improvements
- **[NEGATIVE_TEST_IMPLEMENTATION.md](NEGATIVE_TEST_IMPLEMENTATION.md)** - Negative/error case testing implementation

### Historical Test Status Reports

- **[TEST_EXECUTION_COMPLETE.md](TEST_EXECUTION_COMPLETE.md)** - Historical test completion report
- **[TEST_SUITE_SUMMARY.md](TEST_SUITE_SUMMARY.md)** - Historical test suite summary
- **[TEST_STATUS_SUMMARY.md](TEST_STATUS_SUMMARY.md)** - Historical status summary
- **[TEST_FLAG_IMPLEMENTATION.md](TEST_FLAG_IMPLEMENTATION.md)** - Historical flag implementation notes
- **[VALIDATION_TEST_RESULTS.md](VALIDATION_TEST_RESULTS.md)** - Historical validation results

## Current Test Status

**Total**: 188/188 tests passing (100% coverage)
- **Unit Tests**: 78/78 ✅
- **Assembly Tests**: 68/68 ✅
- **Integration Tests**: 42/42 ✅

## Quick Start

### Running Tests

```bash
# Run all tests
./bin/demi-engine --test

# Run specific test categories
./bin/demi-engine --unit-test           # Unit tests only
./bin/demi-engine --assembly-test       # Assembly tests only
./bin/demi-engine --integration-test    # Integration tests only

# Run tests quietly (just results)
./bin/demi-engine --assembly-test-quiet
```

### Writing Tests

See [TEST_FRAMEWORK_DESIGN.md](TEST_FRAMEWORK_DESIGN.md) for detailed information on writing unit tests, assembly tests, and integration tests.

## Test Categories

### 1. Unit Tests (C++)
Located in `src/test/`
- CPU operations and register management
- Arithmetic instructions
- Memory operations
- Control flow
- Device I/O
- Assembler functionality

### 2. Assembly Tests (In-code)
Embedded in assembly files under `tests/asm/`
- Real assembly program execution
- Multi-instruction workflows
- Error handling validation

### 3. Integration Tests (Hex Files)
Located in `tests/hex/`
- Complete program execution
- System-level validation
- Multi-component interactions

## Related Documentation

- **Test Examples**: `/tests/asm/README.md`
- **Contributing Guide**: `/CONTRIBUTING.md`
- **Codebase Testing Module**: `/docs/codebase/modules/testing.md`

## Test Framework Features

- ✅ Rich assertion system (registers, memory, flags, devices)
- ✅ Component testing (CPU, devices, opcodes, assembler)
- ✅ Integration testing (complete program execution)
- ✅ Clear error reporting with context
- ✅ Test organization by category
- ✅ Easy test extension via macros
