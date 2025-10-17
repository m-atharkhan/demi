# Basic CPU Tests

This directory contains tests for fundamental CPU operations and core instruction sets.

## Test Files

- `test_cpu_basics.asm` - Core CPU operations and register management

## Coverage

### Register Operations
- General purpose register usage (EAX, EBX, ECX, EDX)
- Register-to-register moves
- Register arithmetic operations

### Arithmetic Instructions
- ADD - Addition operations
- SUB - Subtraction operations  
- MUL - Multiplication operations
- Basic overflow handling

### Memory Operations
- MOV with memory operands
- Load/store operations
- Memory addressing modes

### Control Flow
- Unconditional jumps (JMP)
- Conditional jumps (JZ, JNZ, etc.)
- Label resolution
- Program flow control

### System Operations
- HLT instruction
- Basic program termination

## Running Basic Tests

From project root:

```bash
# Run all basic tests
./bin/demi-engine --assembly tests/basic/test_cpu_basics.asm

# With verbose output
./bin/demi-engine --assembly tests/basic/test_cpu_basics.asm --verbose=true
```

## Expected Behavior

All basic tests should execute without errors and demonstrate proper:
- Register state management
- Arithmetic computation
- Memory access
- Program flow control

The tests validate that the core CPU emulation is working correctly before testing more advanced features.