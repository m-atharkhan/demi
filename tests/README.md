# DemiEngine Test Suite

This directory contains the organized test suite for DemiEngine, structured for maintainability and clarity.

## Test Organization

All test files follow the naming convention: `testname.test.asm`

### Core Test Files

| Test File               | Description                      | Tests | Focus Area                                         |
| ----------------------- | -------------------------------- | ----- | -------------------------------------------------- |
| `arithmetic.test.asm`   | Basic arithmetic operations      | 8     | ADD, SUB, MUL, DIV, MOD, INC, DEC, overflow        |
| `memory.test.asm`       | Memory operations and addressing | 6     | LOAD, STORE, LOADR indirect addressing, boundaries |
| `control_flow.test.asm` | Jumps and conditionals           | 5     | JMP, conditional jumps, loops, comparisons         |
| `logical.test.asm`      | Bitwise and logical operations   | 7     | AND, OR, XOR, NOT, shifts, XOR swap                |
| `registers.test.asm`    | Register operations              | 5     | MOV, LOAD_IMM, independence, extended registers    |
| `stack.test.asm`        | Stack operations                 | 4     | PUSH, POP, LIFO order, flag operations             |
| `simd.test.asm`         | SIMD foundation operations       | 6     | VADD, VMUL, VDOT, VMAX, VBROADCAST, packed bytes   |
| `fpu.test.asm`          | Floating-point arithmetic        | 7     | FLD, FST, FADD, FSUB, FMUL, FDIV, trigonometry     |
| `errors.test.asm`       | Error handling and exceptions    | 5     | Division by zero, invalid opcodes, stack overflow  |
| `algorithms.test.asm`   | Complex algorithmic operations   | 5     | Fibonacci, factorial, max finding, binary search   |
| `benchmarks.test.asm`   | Performance benchmarks           | 4     | Algorithm performance testing                      |
| `loadr.test.asm`        | LOADR indirect addressing        | 2     | Specialized LOADR instruction tests                |

## Running Tests

### Run All Tests

```bash
./bin/demi-engine -at
```

### Run Specific Test File

```bash
./bin/demi-engine -at tests_new/arithmetic.test.asm
```

### Run with Detailed Output

```bash
./bin/demi-engine -at tests_new/simd.test.asm --show=all
```

### Run with Error Details Only

```bash
./bin/demi-engine -at tests_new/errors.test.asm --show=fails
```

## Test Categories

- **Arithmetic**: Basic math operations and overflow handling
- **Memory**: Load/store operations and memory addressing modes
- **Control Flow**: Program flow control and conditional execution
- **Logical Operations**: Bitwise operations and shift instructions
- **Registers**: Register manipulation and data movement
- **Stack**: Stack-based operations and LIFO behavior
- **SIMD**: Foundation vector processing operations
- **FPU**: Floating-point arithmetic and mathematical functions
- **Error Handling**: Exception and error condition testing
- **Algorithms**: Complex algorithmic implementations
- **Benchmarks**: Performance measurement and optimization testing

## Test Coverage

- **Total Tests**: ~65 individual test cases
- **Core Instructions**: 100% coverage of fundamental operations
- **SIMD Foundation**: 8 vector operations with comprehensive validation
- **FPU Operations**: 23+ floating-point instructions tested
- **Error Conditions**: Complete error handling validation
- **Memory Operations**: All addressing modes including indirect (LOADR)

## Implementation Status

✅ **SIMD Foundation**: 8 fundamental vector operations implemented and tested  
✅ **FPU Arithmetic**: Complete floating-point mathematics support  
✅ **Core Instructions**: All basic operations fully functional  
✅ **Error Handling**: Comprehensive exception management  
✅ **LOADR Indirect**: Advanced memory addressing implemented

The test suite ensures 100% reliability of the DemiEngine virtual machine foundation.
