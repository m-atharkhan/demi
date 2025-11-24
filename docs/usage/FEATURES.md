# DemiEngine Feature Overview

This document provides a comprehensive overview of all major features implemented in DemiEngine.

## Table of Contents
1. [Core Instruction Set](#core-instruction-set)
2. [Floating Point Unit (FPU)](#floating-point-unit-fpu)
3. [SIMD Vector Operations](#simd-vector-operations)
4. [Testing Framework](#testing-framework)
5. [I/O Subsystem](#io-subsystem)
6. [Assembly Language](#assembly-language)
7. [Debugging Tools](#debugging-tools)

---

## Core Instruction Set

### Basic Operations (63 opcodes)
DemiEngine implements a complete set of core CPU instructions modeled after x86 architecture:

#### Arithmetic
- **Integer Operations**: ADD, SUB, MUL, DIV, INC, DEC
- **64-bit Support**: ADD64, SUB64, MOV64, LOAD_IMM64
- **Comparison**: CMP (sets flags for conditional jumps)

#### Memory Operations
- **Load/Store**: LOAD, STORE (direct memory access)
- **Immediate Loading**: LOAD_IMM (8-bit immediates)
- **Address Operations**: LEA (Load Effective Address), SWAP

#### Control Flow
- **Unconditional**: JMP, CALL, RET
- **Conditional Jumps**:
  - JZ/JNZ (zero flag)
  - JS/JNS (sign flag)
  - JC/JNC (carry flag)
  - JO/JNO (overflow flag)
  - JG/JGE/JL/JLE (signed comparisons)

#### Stack Operations
- **Basic**: PUSH, POP
- **Function Support**: PUSH_ARG, POP_ARG, CALL, RET
- **Flags**: PUSH_FLAG, POP_FLAG (save/restore flags)

#### Bitwise Operations
- **Logical**: AND, OR, XOR, NOT
- **Shift**: SHL, SHR (with immediate amounts)

#### I/O Operations
- **Port Access**: IN, OUT (various sizes: byte, word, long)
- **String I/O**: INSTR, OUTSTR (null-terminated strings)

**Documentation**: See [QUICK_REFERENCE.md](QUICK_REFERENCE.md) for complete opcode listing

---

## Floating Point Unit (FPU)

### Overview
DemiEngine includes a complete x87-style FPU with 23 instructions supporting both 32-bit float and 64-bit double precision arithmetic.

**Current Status**: ✅ 23 FPU opcodes implemented (0xA0-0xB6)  
**Roadmap**: Additional FPU instructions and features planned before moving to SSE/AVX/MMX

### Architecture
- **Stack-Based**: 8-level register stack (ST(0) through ST(7))
- **Data Types**: 32-bit float, 64-bit double
- **Precision**: Full IEEE 754 floating-point support
- **Control**: Full status and control word management

### Instruction Categories

#### Arithmetic (4 opcodes)
```assembly
FADD    ; ST(0) = ST(0) + operand
FSUB    ; ST(0) = ST(0) - operand
FMUL    ; ST(0) = ST(0) * operand
FDIV    ; ST(0) = ST(0) / operand
```

#### Transcendental Functions (4 opcodes)
```assembly
FSIN    ; ST(0) = sin(ST(0))
FCOS    ; ST(0) = cos(ST(0))
FTAN    ; ST(0) = tan(ST(0))
FSQRT   ; ST(0) = sqrt(ST(0))
```

#### Sign Operations (2 opcodes)
```assembly
FABS    ; ST(0) = |ST(0)|
FCHS    ; ST(0) = -ST(0)
```

#### Load/Store - Floating Point (3 opcodes)
```assembly
FLD     ; Load float/double to ST(0)
FST     ; Store ST(0) without popping
FSTP    ; Store ST(0) and pop stack
```

#### Load/Store - Integer (3 opcodes)
```assembly
FILD    ; Load integer, convert to float
FIST    ; Store as integer without popping
FISTP   ; Store as integer and pop
```

#### Comparison (2 opcodes)
```assembly
FCOMPP  ; Compare ST(0) and ST(1), pop twice
FUCOMPP ; Unordered compare, pop twice
```

#### Control (5 opcodes)
```assembly
FINIT   ; Initialize FPU
FCLEX   ; Clear exceptions
FLDCW   ; Load control word
FSTCW   ; Store control word
FSTSW   ; Store status word
```

### Operand Format System
```
Format Byte Structure:
  Bits 0-1: Data type
    00 = 32-bit float
    01 = 64-bit double
    10 = 16-bit integer (FILD/FIST only)
    11 = 32-bit integer (FILD/FIST only)
  
  Bits 2-4: Addressing mode
    000 = ST(0) (stack top)
    001 = ST(i) (stack register)
    010 = Memory direct
    011 = Register indirect
    100 = Register + offset
```

### Example Usage
```assembly
; Calculate circle area: A = π * r²
FLD 0x00 radius_addr    ; Load radius (32-bit float)
FLD 0x00 radius_addr    ; Load radius again
FMUL                    ; r * r = r²
FLD 0x00 pi_addr        ; Load π
FMUL                    ; π * r²
FSTP 0x00 area_addr     ; Store result
```

### FPU Status Word
```
Bit 15: Busy (B)
Bit 14: Condition Code 3 (C3)
Bit 11: Stack Top Pointer (bits 0-2)
Bit 10: Condition Code 2 (C2)
Bit 9:  Condition Code 1 (C1)
Bit 8:  Condition Code 0 (C0)
Bits 0-5: Exception flags (Invalid, Denormal, Zero Divide, Overflow, Underflow, Precision)
```

**Documentation**: See [FPU_REFERENCE.md](FPU_REFERENCE.md) for complete FPU guide

---

## SIMD Vector Operations

### Overview
DemiEngine provides 8 foundational SIMD instructions for parallel data processing using XMM registers.

**Current Status**: ✅ 8 SIMD opcodes implemented (0xD4-0xDB)  
**Roadmap**: Additional SIMD vector operations planned before implementing SSE/AVX/MMX instruction sets

### Architecture
- **Registers**: 16 XMM registers (XMM0-XMM15)
- **Width**: 128-bit per register (4 × 32-bit components)
- **Data Types**: Primarily 32-bit integers/floats
- **Register Mapping**: XMM0-XMM15 mapped to R96-R159

### Instruction Set

#### Vector Arithmetic (2 opcodes)
```assembly
VADD <xmm_dst>, <xmm_src>   ; Element-wise addition
VMUL <xmm_dst>, <xmm_src>   ; Element-wise multiplication
```

#### Vector Reduction (1 opcode)
```assembly
VDOT <xmm_dst>, <xmm_src>   ; Dot product (result in component 0)
```

#### Vector Comparison (1 opcode)
```assembly
VCMPGT <xmm_dst>, <xmm_src> ; Element-wise greater-than comparison
```

#### Vector Operations (2 opcodes)
```assembly
VMAX <xmm_dst>, <xmm_src>       ; Element-wise maximum
VBROADCAST <xmm_dst>, <scalar>  ; Broadcast scalar to all components
```

#### Data Manipulation (2 opcodes)
```assembly
PACKB <xmm_dst>, <xmm_src>      ; Pack bytes with saturation
UNPACKB <xmm_dst>, <xmm_src>    ; Unpack bytes
```

### Register Access
```assembly
; Direct component access
LOAD_IMM XMM0_0, 10    ; XMM0 component 0
LOAD_IMM XMM0_1, 20    ; XMM0 component 1
LOAD_IMM XMM0_2, 30    ; XMM0 component 2
LOAD_IMM XMM0_3, 40    ; XMM0 component 3
```

### Example Usage
```assembly
; Vector scaling: v * scalar
; v = [1, 2, 3, 4], scalar = 10

; Load vector
LOAD_IMM XMM0_0, 1
LOAD_IMM XMM0_1, 2
LOAD_IMM XMM0_2, 3
LOAD_IMM XMM0_3, 4

; Broadcast scalar
LOAD_IMM R0, 10
VBROADCAST XMM1, R0

; Multiply
VMUL XMM0, XMM1
; Result: XMM0 = [10, 20, 30, 40]
```

### Use Cases
- Graphics processing (vertices, normals, colors)
- Physics simulations (particles, forces)
- Signal processing (filters)
- Matrix operations
- Audio/image processing

**Documentation**: See [SIMD_REFERENCE.md](SIMD_REFERENCE.md) for complete SIMD guide

---

## Testing Framework

### Overview
DemiEngine includes a comprehensive testing framework with **219 total tests** supporting unit tests, integration tests, and in-assembly tests.

### Test Types

#### 1. Unit Tests (101 tests)
- C++ unit tests using custom test framework
- Tests for individual components (assembler, CPU, devices)
- Located in `src/test/`

#### 2. Assembly Tests (118 tests) 
- In-assembly test definitions with self-documenting syntax
- Category and tag organization for easy filtering
- Located in `tests/*.test.asm`
- Comprehensive coverage of all instruction sets

### Test Execution
```bash
# Run all tests (219 total)
make test

# Run only unit tests
make test-unit

# Run only assembly tests  
make test-asm

# Run with verbose output
make test-verbose
```

### In-Assembly Test Format
```assembly
.test "test_name" {
    .description "What this test validates"
    .author "DemiEngine Team"
    .category "Test Category"
    .tag "tag1", "tag2"
    
    ; Test code
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    
    ; Assertions
    .assert_reg R0, 15
}
```

### Test Metadata
- **`.description`**: Brief explanation (recommended)
- **`.author`**: Test author/team (optional)
- **`.category`**: Group/category (optional)
- **`.tag`**: Comma-separated tags (optional)

### Assertion Commands
- **`.assert_reg <reg>, <value>`**: Register equals value
- **`.assert_ne <reg>, <value>`**: Register not equals value
- **`.assert_lt <reg>, <value>`**: Register less than value
- **`.assert_gt <reg>, <value>`**: Register greater than value

### CLI Test Commands
```bash
# Run all tests
./bin/demi-engine -t

# Run specific test types
./bin/demi-engine -ut           # Unit tests only
./bin/demi-engine -it           # Integration tests only
./bin/demi-engine -at           # Assembly tests only

# Quiet mode (summary only)
./bin/demi-engine -atq          # Assembly tests quiet
./bin/demi-engine -utq          # Unit tests quiet

# Combined flags
./bin/demi-engine -tq           # All tests, quiet mode
```

### Test Categories
Current test coverage includes:
- **Arithmetic**: Basic math operations
- **Bitwise**: Logical and shift operations
- **Control Flow**: Jumps and conditionals
- **Data Handling**: Memory and data operations
- **Devices**: I/O device functionality
- **FPU**: Floating-point operations
- **Functions**: Function calls and returns
- **Registers**: Register operations
- **SIMD**: Vector operations
- **Stack**: Stack management

### Test Output Features
- Category grouping
- Color-coded results (pass/fail)
- Timing information (per-test and total)
- Metadata display (description, author, tags)
- Pass/fail statistics

**Documentation**: See [TEST_FRAMEWORK_DESIGN.md](TEST_FRAMEWORK_DESIGN.md) for complete testing guide

---

## I/O Subsystem

### Device Architecture
DemiEngine uses a port-based I/O system with pluggable devices.

### Port Mapping
- **Port 1**: Console Device (text I/O)
- **Port 2**: File Device (file operations)
- **Port 3**: Counter Device (timing/counting)
- **Port 4**: Serial Port (serial communication)
- **Ports 5-255**: Available for custom devices

### I/O Instructions
```assembly
IN <dst>, <port>        ; Read from port
OUT <port>, <src>       ; Write to port
INB/OUTB               ; Byte operations
INW/OUTW               ; Word operations (16-bit)
INL/OUTL               ; Long operations (32-bit)
INSTR/OUTSTR           ; String operations
```

### Console Device (Port 1)
```assembly
; Output string
LOAD_IMM R0, string_addr
OUTSTR 1, R0

; Output character
LOAD_IMM R0, 65         ; ASCII 'A'
OUTB 1, R0
```

### File Device (Port 2)
```assembly
; Open file
LOAD_IMM R0, filename_addr
OUT 2, R0               ; Open command

; Read data
IN R1, 2                ; Read bytes

; Close file
LOAD_IMM R0, 0
OUT 2, R0               ; Close command
```

### Counter Device (Port 3)
```assembly
; Read counter value
IN R0, 3                ; Get current count

; Reset counter
LOAD_IMM R1, 0
OUT 3, R1               ; Reset to zero
```

### Device Manager
- Dynamic device registration
- Port mapping and routing
- Error handling for invalid ports
- Hot-swappable device support

**Documentation**: See module documentation in `docs/codebase/modules/`

---

## Assembly Language

### Syntax Overview
DemiEngine assembly supports both hex and mnemonic formats with modern features.

### Entry Point Behavior
**Important**: DemiEngine assembly does NOT require a `main:` label or specific entry point. Execution begins at the first instruction in the file (address 0x00).

- The `main:` label in example files is purely for readability and convention
- You can start with any instruction - the CPU begins executing from the top
- Test files (`.test` blocks) have special handling and don't need entry labels

**Examples**:
```assembly
; This works - starts immediately
LOAD_IMM R0, 42
ADD R0, R1
HALT

; This also works - 'main:' is just a label
main:
    LOAD_IMM R0, 42
    ADD R0, R1
    HALT
```

### Basic Syntax
```assembly
; Comments start with semicolon
LOAD_IMM R0, 42         ; Load immediate value
ADD R1, R0              ; Add registers
STORE 0x100, R1         ; Store to memory
```

### Data Directives
```assembly
.data
    message: .string "Hello, World!"
    value: .byte 42
    buffer: .space 64

.bss
    uninitialized: .space 256
```

### Labels and Jumps
```assembly
start:
    LOAD_IMM R0, 10
    
loop:
    DEC R0
    JNZ loop            ; Jump to label
    
    JMP end
    
end:
    HALT
```

### String Definitions
```assembly
; Null-terminated strings
.string "Hello"         ; Automatically adds \0

; Manual byte definition
.byte 'H', 'e', 'l', 'l', 'o', 0
```

### In-Assembly Tests
```assembly
.test "addition test" {
    .description "Test basic addition"
    
    LOAD_IMM R0, 5
    LOAD_IMM R1, 10
    ADD R0, R1
    
    .assert_reg R0, 15
}
```

### Comments and Documentation
```assembly
; Single-line comment

# Alternative comment style

/*
 * Multi-line comments
 * (if supported by assembler)
 */
```

### Assembler Features
- Two-pass assembly
- Label resolution
- Macro support (planned)
- Error reporting with line numbers
- Symbol table generation

**Documentation**: See assembler docs in `docs/codebase/modules/assembler.md`

---

## Debugging Tools

### GUI Debugger
DemiEngine includes an ImGui-based visual debugger with structured error reporting.

#### Features
- **CPU State Viewer**: Real-time register values
- **Memory Inspector**: View and edit memory
- **Stack Viewer**: Monitor stack operations  
- **Disassembly**: View program instructions
- **Breakpoints**: Set execution breakpoints
- **Step Execution**: Single-step through code
- **Device Monitor**: View I/O operations
- **Debug Log**: Categorized debug messages with error codes

#### Usage
```bash
# Start with GUI debugger
./bin/demi-engine -g program.hex

# Enable structured debug output
./bin/demi-engine --debug program.hex
```

### Structured Error Handling
DemiEngine provides comprehensive error handling with categorized error codes:

- **Error Codes**: Structured system (0x001-0x5FF) for precise error identification
- **Debug Categories**: CPU, Memory, Assembler, I/O, Tests, and Device debugging
- **Context Information**: Register states, memory contents, program counter values
- **Error Recovery**: Graceful error handling with detailed diagnostics

**Documentation**: See [Error Handling Implementation](ERROR_HANDLING_IMPLEMENTATION.md) for complete reference

### Command-Line Debugging  
```bash
# Verbose execution with structured output
./bin/demi-engine -v program.hex

# Enable specific debug categories
export DEBUG_CATEGORY=CPU,MEMORY
./bin/demi-engine program.hex

# Trace mode with full debug context
./bin/demi-engine -vv program.hex
```

### Logging System
- **Structured Levels**: ERROR, WARNING, INFO, DEBUG with error codes
- **Colored Output**: Terminal colors for log levels and categories
- **Timestamps**: Optional timestamp prefixes
- **Module Tags**: Tag logs by component (CPU, Memory, etc.)
- **Context Preservation**: Register dumps and memory states in error messages

**Documentation**: See `docs/codebase/modules/` for component-specific debugging

---

## Feature Roadmap

### Completed ✅
- Core instruction set (63 opcodes)
- FPU (23 opcodes)
- SIMD foundation (8 opcodes)
- Comprehensive testing framework
- I/O subsystem with multiple devices
- Assembly language with test support
- GUI debugger

### In Progress 🔄
- Extended 64-bit operations (4/22 opcodes)
- Documentation updates
- Performance optimization

### Planned 📋
- Additional FPU operations (transcendental functions, extended precision)
- Additional SIMD vector operations (more data types, wider operations)
- SSE/SSE2 operations (26 opcodes) - after core FPU/SIMD expansion
- AVX operations (20 opcodes) - after SSE implementation
- MMX operations (11 opcodes) - after SSE implementation
- Macro system
- Preprocessor directives
- Virtual memory system
- Hardware interrupts
- DMA controller
- Enhanced GUI features

---

## Performance Characteristics

### Execution Speed
- Typical instruction: ~10-100 CPU cycles
- FPU operations: ~50-200 CPU cycles
- SIMD operations: ~20-80 CPU cycles (parallelized)
- Memory access: ~30-50 CPU cycles

### Memory Usage
- Default memory: 64KB
- Configurable up to 1MB
- Stack size: Configurable (default 4KB)
- Virtual storage: Disk-backed (configurable size)

### Optimization Features
- Register allocation
- Instruction pipelining (planned)
- Branch prediction (planned)
- Cache simulation (planned)

---

## Integration Examples

### FPU + SIMD Integration
```assembly
; Compute vector magnitude using both FPU and SIMD
; magnitude = sqrt(x² + y² + z² + w²)

; Load vector into XMM0
LOAD_IMM XMM0_0, 3
LOAD_IMM XMM0_1, 4
LOAD_IMM XMM0_2, 0
LOAD_IMM XMM0_3, 0

; Dot product with itself
MOV XMM1_0, XMM0_0
MOV XMM1_1, XMM0_1
MOV XMM1_2, XMM0_2
MOV XMM1_3, XMM0_3
VDOT XMM0, XMM1         ; XMM0[0] = 25

; Transfer to FPU for square root
STORE temp_addr, XMM0_0
FLD 0x01 temp_addr      ; Load as double
FSQRT                   ; sqrt(25) = 5
FSTP 0x01 result_addr   ; Store result
```

### Complex I/O with FPU
```assembly
; Read floating-point values from file, process, and display

; Open file
LOAD_IMM R0, filename_addr
OUT 2, R0

; Read float
IN R1, 2                ; Read bytes
STORE temp_addr, R1
FLD 0x00 temp_addr      ; Load as float

; Process (e.g., square root)
FSQRT

; Store and output
FSTP 0x00 result_addr
LOAD result_addr, R2
OUT 1, R2               ; Print to console
```

---

## See Also

- [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Instruction reference
- [FPU_REFERENCE.md](FPU_REFERENCE.md) - Complete FPU guide
- [SIMD_REFERENCE.md](SIMD_REFERENCE.md) - Complete SIMD guide
- [TEST_FRAMEWORK_DESIGN.md](TEST_FRAMEWORK_DESIGN.md) - Testing guide
- [API_REFERENCE.md](codebase/API_REFERENCE.md) - C++ API documentation
- [README.md](../README.md) - Main project documentation
- [roadmap.md](../roadmap.md) - Future development plans
