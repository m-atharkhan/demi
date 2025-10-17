# Tests Directory

This directory contains comprehensive tests for the DemiEngine assembler and virtual machine.

## Directory Structure

- **basic/** - Basic CPU operations and fundamental instruction tests
- **fpu/** - Floating point unit operations and FPU stack management tests  
- **parsing/** - Advanced parser features like sections, directives, and syntax
- **README.md** - This file

## Running Tests

Use the test runner from the project root:

```bash
./run_tests.sh
```

Or run individual categories:

```bash
# Basic CPU tests
./bin/demi-engine --assembly tests/basic/test_cpu_basics.asm

# FPU tests  
./bin/demi-engine --assembly tests/fpu/test_st_registers.asm

# Parser tests
./bin/demi-engine --assembly tests/parsing/test_sections_and_directives.asm
```

## Test Categories

### Basic Tests (`basic/`)
- Core CPU instructions (MOV, ADD, SUB, etc.)
- Register operations
- Basic arithmetic and logic
- Jump and conditional instructions
- Memory operations

### FPU Tests (`fpu/`)
- ST(0) through ST(7) register syntax
- Floating point arithmetic (FADD, FSUB, FMUL, FDIV)
- Stack operations (FLD, FST, FSTP)
- Floating point comparisons
- Precision and rounding tests

### Parser Tests (`parsing/`)
- Section directives (.data, .text)
- Data directives (.dd for double words)
- Floating point number parsing
- Parentheses in expressions
- Size specifiers (byte, word, dword, qword)
- Complex expressions and edge cases

## Writing New Tests

Test files should follow these conventions:

1. Use descriptive filenames: `test_feature_name.asm`
2. Include test metadata comments at the top
3. Use assertions where available
4. Test both positive and negative cases
5. Document expected behavior

Example test structure:

```assembly
; Test: Feature description
; Purpose: What this test validates
; Expected: Expected outcome

.data
    test_value dd 42.5

.text
main:
    ; Test implementation
    fld test_value
    ; ... test operations
    hlt
```