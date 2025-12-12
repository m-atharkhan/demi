# DemiEngine Instruction Set Quick Reference

## Overview

**Currently Implemented**: 94 opcodes (fully functional)

- 63 Core opcodes (basic operations, arithmetic, memory, control flow, stack, bitwise, I/O)
- 23 FPU opcodes (floating-point arithmetic and control)
- 8 SIMD opcodes (vector operations)

**Defined but Not Implemented**: 57 opcodes (SSE, AVX, MMX - planned for future)

## Basic Syntax

- Instructions are written in hexadecimal
- Comments start with `#`
- Registers: R0-R7 (basic), R0-R15 (extended), plus special/system registers
- LOAD_IMM uses 3-byte format: opcode + register + immediate_value (8-bit)

## Implemented Instruction Categories

### Basic Operations

```hex
00                    # NOP - No operation
FF                    # HALT - Stop execution
01 <reg> <imm8>       # LOAD_IMM - Load immediate value (8-bit)
04 <dst> <src>        # MOV - Copy register
```

### Arithmetic

```hex
02 <dst> <src>        # ADD - Add registers
03 <dst> <src>        # SUB - Subtract registers
10 <dst> <src>        # MUL - Multiply registers
11 <dst> <src>        # DIV - Divide registers
29 <dst> <src>        # MOD - Modulo
12 <reg>              # INC - Increment register
13 <reg>              # DEC - Decrement register
```

### Memory Operations

```hex
06 <dst> <addr>       # LOAD - Load from memory
07 <addr> <src>       # STORE - Store to memory
20 <reg> <addr>       # LEA - Load Effective Address
21 <reg> <addr>       # SWAP - Swap register with memory
41 <reg>              # LOADR - Load indirect (address in register)
```

### Control Flow

```hex
05 <addr>             # JMP - Unconditional jump
0A <reg1> <reg2>      # CMP - Compare registers
0B <addr>             # JZ - Jump if zero
0C <addr>             # JNZ - Jump if not zero
26 <addr>             # JL - Jump if less
25 <addr>             # JG - Jump if greater
28 <addr>             # JLE - Jump if less or equal
27 <addr>             # JGE - Jump if greater or equal
0D <addr>             # JS - Jump if sign
0E <addr>             # JNS - Jump if not sign
0F <addr>             # JC - Jump if carry
22 <addr>             # JNC - Jump if no carry
23 <addr>             # JO - Jump if overflow
24 <addr>             # JNO - Jump if no overflow
```

### Stack Operations

```hex
08 <reg>              # PUSH - Push register to stack
09 <reg>              # POP - Pop from stack
1A <addr>             # CALL - Call subroutine
1B                    # RET - Return from subroutine
1C <reg>              # PUSH_ARG - Push function argument
1D <reg>              # POP_ARG - Pop function argument
1E <reg>              # PUSH_FLAG - Push flags onto stack
1F <reg>              # POP_FLAG - Pop flags from stack
```

### Bitwise Operations

```hex
14 <dst> <src>        # AND - Bitwise AND
15 <dst> <src>        # OR - Bitwise OR
16 <dst> <src>        # XOR - Bitwise XOR
17 <reg>              # NOT - Bitwise NOT
18 <reg> <amount>     # SHL - Shift left (immediate amount)
19 <reg> <amount>     # SHR - Shift right (immediate amount)
```

### I/O Operations

```hex
30 <dst> <port>       # IN - Read from port
31 <port> <src>       # OUT - Write to port
32 <dst> <port>       # INB - Read byte from port
33 <port> <src>       # OUTB - Write byte to port
34 <dst> <port>       # INW - Read word from port
35 <port> <src>       # OUTW - Write word to port
36 <dst> <port>       # INL - Read long from port
37 <port> <src>       # OUTL - Write long to port
38 <dst> <port>       # INSTR - Read string from port
39 <port> <src>       # OUTSTR - Write string to port
```

### Data Definition

```hex
40 <byte>             # DB - Define byte in program
```

## Common Patterns

### Hello World

```hex
01 00 10              # LOAD_IMM R0, 0x10 (string address)
39 01 00              # OUTSTR port=1, R0
FF                    # HALT

# String data at 0x10
48 65 6C 6C 6F 00     # "Hello\0"
```

### Simple Loop

```hex
01 00 0A              # LOAD_IMM R0, 10 (counter)
01 01 00              # LOAD_IMM R1, 0 (comparison value)
# Loop start (0x06):
13 00                 # DEC R0
0A 00 01              # CMP R0, R1
0C 06                 # JNZ 0x06 (loop back)
FF                    # HALT
```

### Function Call

```hex
01 00 05              # LOAD_IMM R0, 5
1A 10                 # CALL 0x10 (function address)
FF                    # HALT

# Function at 0x10:
12 00                 # INC R0 (increment parameter)
1B                    # RET
```

## Device Ports

- **Port 1**: Console (text I/O)
- **Port 2**: File device
- **Port 3**: Counter device
- **Port 4**: Serial port
- **Ports 5-255**: Available for custom devices

## Register Usage Conventions

- **R0-R3**: General purpose, function parameters/return values
- **R4-R6**: General purpose, local variables
- **R7**: Often used as temporary or frame pointer
- **PC**: Program counter (automatic)
- **SP**: Stack pointer (automatic)
- **FP**: Frame pointer (manual management)

## Memory Layout

```
High Memory ┌─────────┐
            │  Stack  │ ← SP
            ├─────────┤
            │  Free   │
            ├─────────┤
            │  Heap   │
            ├─────────┤
            │  Data   │
            ├─────────┤
Low Memory  │  Code   │ ← PC starts here
            └─────────┘
```

## Common Gotchas

1. **8-bit immediates**: LOAD_IMM only supports 8-bit values (0-255)
2. **Stack direction**: Stack grows downward (SP decreases on push)
3. **String termination**: Strings must be null-terminated for OUTSTR
4. **Register encoding**: Registers are single bytes (0x00-0x07)
5. **Address sizes**: Jump addresses are 8-bit (0-255 program space)
6. **Carry flag**: Requires 32-bit overflow to trigger (use NOT + ADD technique)

## Testing Quick Reference

### Run Tests

```bash
./bin/demi-engine -ut                         # Unit tests (101)
./bin/demi-engine -at                         # Assembly tests (118)
# Total: 219 tests
```

### Test Specific Files

```bash
./bin/demi-engine -at tests/arithmetic.test.asm   # Test single file
./bin/demi-engine -at tests/stack.test.asm        # Another example
```

### In-Assembly Test Format

```assembly
.test test_name
    .description What this test validates
    .author Your Name
    .category Test Category
    .tag tag1, tag2

    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    .assert_eq R0, 15
.endtest
```

### Test Metadata Directives

- `.description` - Brief test description (recommended)
- `.author` - Test author/team (optional)
- `.category` - Test category/group (optional)
- `.tag` - Comma-separated tags (optional)
- `.maxsteps` - Maximum execution steps (default: 10,000)
- `.maxcalldepth` - Maximum call stack depth (default: 64 for tests, 256 for production)
- `.timeout` - Maximum execution time in milliseconds (default: no timeout)
- `.skip` - Skip this test (no value needed)

### Benchmark Directives

- `.benchmark` - Mark this test as a benchmark (measures performance)
- `.warmup` - Number of warmup iterations before measurement
- `.iterations` - Number of benchmark iterations (default: 1)
- `.measure` - What to measure: "time", "cycles", or "instructions"

### Test Commands

- `.assert_eq <reg>, <value>` - Assert register equals value
- `.assert_ne <reg>, <value>` - Assert register not equals value
- `.assert_lt <reg>, <value>` - Assert register less than value
- `.assert_gt <reg>, <value>` - Assert register greater than value

See [TEST_FLAGS.md](testing/TEST_FLAGS.md) and [tests/asm/README.md](../tests/asm/README.md) for complete documentation.

---

### Floating Point Unit (FPU)

```hex
# FPU Load/Store (Floating-Point)
A0 <format> <src>     # FLD - Load floating-point value
A1 <format> <dst>     # FST - Store ST(0) (no pop)
A2 <format> <dst>     # FSTP - Store ST(0) and pop

# FPU Load/Store (Integer)
A3 <format> <src>     # FILD - Load integer to FPU stack
A4 <format> <dst>     # FIST - Store ST(0) as integer (no pop)
A5 <format> <dst>     # FISTP - Store ST(0) as integer and pop

# FPU Arithmetic
A6 <format> <src>     # FADD - Floating-point add
A7 <format> <src>     # FSUB - Floating-point subtract
A8 <format> <src>     # FMUL - Floating-point multiply
A9 <format> <src>     # FDIV - Floating-point divide

# FPU Transcendental Functions
AA                    # FSIN - Compute sine of ST(0)
AB                    # FCOS - Compute cosine of ST(0)
AC                    # FTAN - Compute tangent of ST(0)
AD                    # FSQRT - Square root of ST(0)

# FPU Sign Operations
AE                    # FABS - Absolute value of ST(0)
AF                    # FCHS - Change sign of ST(0)

# FPU Control
B0                    # FINIT - Initialize FPU
B1                    # FCLEX - Clear FPU exceptions
B2 <addr>             # FSTCW - Store FPU control word
B3 <addr>             # FLDCW - Load FPU control word
B4 <addr>             # FSTSW - Store FPU status word

# FPU Comparison
B5                    # FCOMPP - Compare ST(0) and ST(1), pop twice
B6                    # FUCOMPP - Unordered compare ST(0) and ST(1), pop twice
```

### SIMD Vector Operations

```hex
D4 <dst> <src>        # VADD - Vector addition (element-wise)
D5 <dst> <src>        # VMUL - Vector multiplication (element-wise)
D6 <dst> <src>        # VDOT - Vector dot product
D7 <dst> <src>        # VMAX - Vector maximum (element-wise)
D8 <dst> <src>        # VBROADCAST - Broadcast scalar to vector
D9 <dst> <src>        # VCMPGT - Vector compare greater than
DA <dst> <src>        # PACKB - Pack bytes with saturation
DB <dst> <src>        # UNPACKB - Unpack bytes
```

## Planned But Not Yet Implemented

The following instruction categories are **defined** in the opcode table but **not yet implemented**:

### SIMD/SSE Operations (0x80-0x9F) - 26 opcodes ⚠️

```
MOVAPS, MOVUPS, ADDPS, SUBPS, MULPS, DIVPS, SQRTPS, MAXPS, MINPS,
ANDPS, ORPS, XORPS, CMPPS,
MOVAPD, MOVUPD, ADDPD, SUBPD, MULPD, DIVPD, SQRTPD, MAXPD, MINPD,
ANDPD, ORPD, XORPD, CMPPD
```

**Status**: Opcodes defined, handlers not implemented
**Use Case**: Vector operations for parallel processing (SSE/SSE2)

### AVX Operations (0xC0-0xDF) - 20 opcodes ⚠️

```
VADDPS, VSUBPS, VMULPS, VDIVPS, VSQRTPS, VMAXPS, VMINPS,
VANDPS, VORPS, VXORPS,
VADDPD, VSUBPD, VMULPD, VDIVPD, VSQRTPD, VMAXPD, VMINPD,
VANDPD, VORPD, VXORPD
```

**Status**: Opcodes defined, handlers not implemented
**Use Case**: 256-bit vector operations (AVX)

### MMX Operations (0xE0-0xEF) - 11 opcodes ⚠️

```
MOVQ, PADDB, PADDW, PADDD, PSUBB, PSUBW, PSUBD,
PCMPEQB, PCMPEQW, PCMPEQD, EMMS
```

**Status**: Opcodes defined, handlers not implemented
**Use Case**: Multimedia/integer SIMD operations

### Additional 64-bit Operations - 18 opcodes ⚠️

```
MUL64, DIV64, AND64, OR64, XOR64, SHL64, SHR64, CMP64,
NOT64, INC64, DEC64,
MULEX, DIVEX, CMPEX, LOADEX, STOREX, PUSHEX, POPEX, MODEFLAG
```

**Status**: Partially implemented (ADD64, SUB64, MOV64, LOAD_IMM64 work)
**Use Case**: Extended 64-bit register operations

---

## Implementation Status Summary

| Category              | Defined | Implemented | Status  |
| --------------------- | ------- | ----------- | ------- |
| **Core Operations**   | 48      | 48          | ✅ 100% |
| **Extended 64-bit**   | 22      | 4           | ⚠️ 18%  |
| **FPU**               | 23      | 23          | ✅ 100% |
| **SIMD (Foundation)** | 8       | 8           | ✅ 100% |
| **SIMD/SSE**          | 26      | 0           | ❌ 0%   |
| **AVX**               | 20      | 0           | ❌ 0%   |
| **MMX**               | 11      | 0           | ❌ 0%   |
| **Mode Control**      | 1       | 1           | ✅ 100% |
| **TOTAL**             | **159** | **94**      | **59%** |

**Note**: Attempting to use unimplemented opcodes will result in an "Unknown opcode" error.

---

## See Also

- [README.md](../README.md) - Main project documentation
- [roadmap.md](../roadmap.md) - Future development plans
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues and solutions
