# FPU Tests

This directory contains comprehensive tests for the Floating Point Unit (FPU) operations and ST register stack.

## Test Files

- `test_st_registers.asm` - Complete ST(0) through ST(7) syntax validation
- `test_floating_point_precision.asm` - Floating point parsing and precision tests

## Coverage

### ST Register Syntax
- ST(0) through ST(7) register references
- Case insensitive parsing (ST/st)
- Register index validation (0-7 bounds checking)
- Proper AST node generation (STRegisterExpression)

### Floating Point Operations
- **FADD** - Addition with various operand types
- **FSUB** - Subtraction operations
- **FMUL** - Multiplication operations
- **FDIV** - Division operations
- **FLD** - Load operations (immediate values, memory, ST registers)
- **FST** - Store operations
- **FSTP** - Store and pop operations
- **FXCH** - Register exchange
- **FCHS** - Change sign
- **FABS** - Absolute value

### Data Types
- Single precision (32-bit) floating point
- Double precision (64-bit) floating point
- Immediate floating point values in assembly
- Memory-based floating point operations

### Precision and Accuracy
- IEEE 754 compliance testing
- Rounding behavior validation
- Precision preservation across operations
- Edge case handling (infinity, NaN, denormals)

## FPU Architecture

The DemiEngine FPU implements a traditional x87-style register stack:

```
ST(0) - Top of stack (most recent)
ST(1) - Second from top
ST(2) - Third from top
...
ST(7) - Bottom of stack
```

### Operation Types

1. **Stack Operations**: Push/pop values to/from the FPU stack
2. **Arithmetic**: Binary operations between ST registers or memory
3. **Data Movement**: Load from memory, store to memory, exchange registers
4. **Unary Operations**: Sign change, absolute value, etc.

## Running FPU Tests

From project root:

```bash
# Run all FPU tests
./bin/demi-engine --assembly tests/fpu/test_st_registers.asm
./bin/demi-engine --assembly tests/fpu/test_floating_point_precision.asm

# With verbose output to see FPU operations
./bin/demi-engine --assembly tests/fpu/test_st_registers.asm --verbose=true
```

## Expected Behavior

FPU tests validate:

1. **Syntax Parsing**: All ST(n) syntax variations parse correctly
2. **Operation Execution**: FPU operations execute without errors
3. **Stack Management**: Proper stack pointer manipulation
4. **Precision**: Accurate floating point computation
5. **Memory Integration**: Seamless memory/register operations

## Opcode Range

FPU operations use opcodes 0xA0-0xB6 in the unified opcode registry:
- 0xA0: FADD variants
- 0xA1-0xA3: FSUB variants  
- 0xA4-0xA6: FMUL variants
- 0xA7-0xA9: FDIV variants
- 0xAA-0xAC: FLD variants
- 0xAD-0xAF: FST variants
- 0xB0: FSTP
- 0xB1: FXCH
- 0xB2: FCHS
- 0xB3: FABS
- 0xB4-0xB6: Reserved for future FPU operations