# Opcode Edge Cases Documentation

**Document Status:** Active Reference  
**Last Updated:** January 28, 2026  
**Purpose:** Comprehensive documentation of all implemented opcodes and their edge case test coverage

---

## Table of Contents

1. [Overview](#overview)
2. [Basic Operations (0x00-0x1F)](#basic-operations-0x00-0x1f)
3. [Memory & I/O Operations (0x20-0x4F)](#memory--io-operations-0x20-0x4f)
4. [64-bit Operations (0x50-0x5F)](#64-bit-operations-0x50-0x5f)
5. [Extended Register Operations (0x60-0x6F)](#extended-register-operations-0x60-0x6f)
6. [CPU Mode Control (0x70-0x7F)](#cpu-mode-control-0x70-0x7f)
7. [SSE Operations (0x80-0x99)](#sse-operations-0x80-0x99)
8. [FPU Operations (0xA0-0xBF)](#fpu-operations-0xa0-0xbf)
9. [SIMD Vector Operations (0xD4-0xDB)](#simd-vector-operations-0xd4-0xdb)
10. [Test Coverage Summary](#test-coverage-summary)
11. [Missing Edge Cases](#missing-edge-cases)
12. [Reserved Opcodes](#reserved-opcodes)

---

## Overview

This document catalogs all **133 implemented opcodes** in the DemiEngine virtual machine, organized by functional category. Each opcode entry includes:

- **Opcode Value:** Hexadecimal opcode number
- **Operand Format:** Expected operand types and encoding
- **Edge Cases Tested:** Currently covered test scenarios
- **Edge Cases Needed:** Recommended additional test coverage
- **Related Tests:** Test files that exercise this opcode

**For a comprehensive overview of all defined opcodes (including 24 reserved), see [OPCODE_STATUS.md](OPCODE_STATUS.md)**

### Legend

- ✅ **Fully Tested:** Comprehensive edge case coverage
- ⚠️ **Partially Tested:** Basic functionality covered, some edge cases missing
- ❌ **Minimal Testing:** Only basic success path tested
- 🔴 **Untested:** No dedicated edge case tests

---

## Basic Operations (0x00-0x1F)

### NOP (0x00) - No Operation
**Status:** ✅ Fully Tested  
**Operands:** None  
**Edge Cases Tested:**
- Sequential NOP execution
- NOP in various control flow contexts
- Performance impact of NOP chains

**Edge Cases Needed:**
- None (trivial operation)

**Related Tests:** `tests/basic_operations.test.asm`

---

### LOAD_IMM (0x01) - Load Immediate Value
**Status:** ✅ Fully Tested  
**Operands:** `reg, imm32`  
**Encoding:** `[opcode] [reg] [imm32_bytes]`

**Edge Cases Tested:**
- Decimal immediates: `42`, `1000`, `0`, `255`
- Hexadecimal immediates: `0x2A`, `0xFF`, `0x1000`, `0xABCD`
- Binary immediates: `0b1010`, `0b11111111`, `0b10101010`
- Octal immediates: `0o12`, `0o377`, `0o777`
- Zero value: `0`
- One value: `1`
- Maximum 32-bit value: `0xFFFFFFFF`
- Powers of 2: `1, 2, 4, 8, 16, 32, 64, 128, 256`

**Edge Cases Needed:**
- Negative values (signed interpretation)
- All register targets (R0-R15, extended registers)
- Immediate value overflow behavior

**Related Tests:** 
- `tests/opcode_operands.test.asm`
- `tests/basic_operations.test.asm`
- `tests/arithmetic.test.asm`

---

### ADD (0x02) - Add Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 + reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic addition: `5 + 3 = 8`
- Zero addition: `x + 0 = x`
- Addition with carry flag behavior
- Commutative operations

**Edge Cases Needed:**
- 32-bit overflow (wrap-around behavior)
- Addition of maximum values: `0xFFFFFFFF + 1`
- Addition resulting in zero: `0xFFFFFFFF + 1`
- Overflow flag verification
- Sign flag behavior for two's complement
- All register combinations (R0-R15)

**Related Tests:**
- `tests/arithmetic.test.asm`
- `tests/control_flow.test.asm` (for flag-based branching)

---

### SUB (0x03) - Subtract Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 - reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic subtraction: `8 - 3 = 5`
- Zero subtraction: `x - 0 = x`
- Self-subtraction: `x - x = 0`
- Borrow flag behavior

**Edge Cases Needed:**
- Underflow (wrap-around): `0 - 1 = 0xFFFFFFFF`
- Subtraction of larger from smaller value
- Carry flag verification
- Sign flag behavior
- Zero flag for exact equality

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### MOV (0x04) - Move Register to Register
**Status:** ✅ Fully Tested  
**Operands:** `dst, src` → `dst = src`  
**Encoding:** `[opcode] [dst] [src]`

**Edge Cases Tested:**
- Register-to-register copy
- Self-move: `MOV EAX, EAX` (idempotent)
- Zero value move
- Maximum value move
- All register pairs (R0-R7, extended)
- x64 register aliases (RAX, RBX, etc.)
- 32-bit register aliases (EAX, EBX, etc.)

**Edge Cases Needed:**
- Move between different register sizes (if supported)
- Concurrent moves in parallel execution (if applicable)

**Related Tests:**
- `tests/opcode_operands.test.asm`
- `tests/registers.test.asm`

---

### JMP (0x05) - Unconditional Jump
**Status:** ⚠️ Partially Tested  
**Operands:** `address` (absolute or label)  
**Encoding:** `[opcode] [address32]`

**Edge Cases Tested:**
- Forward jump
- Backward jump
- Jump to label
- Jump to absolute address

**Edge Cases Needed:**
- Jump to address 0 (start of program)
- Jump to end of program
- Jump to current instruction (infinite loop detection)
- Jump outside program bounds (error handling)
- Jump alignment issues

**Related Tests:**
- `tests/control_flow.test.asm`

---

### JZ/JE (0x0C/0x0D) - Jump if Zero/Equal
**Status:** ⚠️ Partially Tested  
**Operands:** `address`  
**Encoding:** `[opcode] [address32]`

**Edge Cases Tested:**
- Jump taken when zero flag set
- Jump not taken when zero flag clear
- After CMP equality
- After SUB resulting in zero

**Edge Cases Needed:**
- JZ immediately after program start (undefined flags)
- Multiple consecutive conditional jumps
- JZ with label resolution edge cases

**Related Tests:**
- `tests/control_flow.test.asm`

---

### JNZ/JNE (0x0E/0x0F) - Jump if Not Zero/Not Equal
**Status:** ⚠️ Partially Tested  
**Operands:** `address`  
**Encoding:** `[opcode] [address32]`

**Edge Cases Tested:**
- Jump taken when zero flag clear
- Jump not taken when zero flag set
- Loop continuation logic

**Edge Cases Needed:**
- Same as JZ/JE

**Related Tests:**
- `tests/control_flow.test.asm`

---

### CMP (0x0B) - Compare Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` (compare reg1 - reg2, set flags)  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Equality comparison
- Less-than comparison
- Greater-than comparison
- Flag setting without modifying operands

**Edge Cases Needed:**
- CMP with zero
- CMP with maximum value
- CMP with signed/unsigned interpretation
- All flag combinations (ZF, SF, CF, OF)

**Related Tests:**
- `tests/control_flow.test.asm`
- `tests/arithmetic.test.asm`

---

### MUL (0x10) - Multiply Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 * reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic multiplication: `3 * 4 = 12`
- Multiplication by zero: `x * 0 = 0`
- Multiplication by one: `x * 1 = x`

**Edge Cases Needed:**
- 32-bit overflow: `0xFFFF * 0xFFFF`
- Overflow flag behavior
- Multiplication of negative values (signed)
- Powers of 2 multiplication
- Large value multiplication

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### DIV (0x11) - Divide Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 / reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic division: `12 / 3 = 4`
- Division by one: `x / 1 = x`

**Edge Cases Needed:**
- **Division by zero** (exception handling) 🔴
- Remainder handling (if DIV only returns quotient)
- Division with truncation: `7 / 2 = 3`
- Division of negative values (signed)
- Maximum value division: `0xFFFFFFFF / 1`

**Related Tests:**
- `tests/arithmetic.test.asm`
- `tests/errors.test.asm` (for divide-by-zero)

---

### MOD (0x12) - Modulo (Remainder)
**Status:** ❌ Minimal Testing  
**Operands:** `reg1, reg2` → `reg1 = reg1 % reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic modulo: `10 % 3 = 1`

**Edge Cases Needed:**
- Modulo by zero (exception handling) 🔴
- Modulo by one: `x % 1 = 0`
- Modulo where dividend < divisor: `3 % 10 = 3`
- Modulo with negative values
- Modulo of zero: `0 % x = 0`

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### INC (0x13) - Increment Register
**Status:** ⚠️ Partially Tested  
**Operands:** `reg` → `reg = reg + 1`  
**Encoding:** `[opcode] [reg]`

**Edge Cases Tested:**
- Basic increment: `x → x+1`
- Flag behavior after increment

**Edge Cases Needed:**
- Overflow: `0xFFFFFFFF + 1 → 0` (with overflow flag)
- Zero flag after incrementing -1 (signed)
- Multiple consecutive increments

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### DEC (0x14) - Decrement Register
**Status:** ⚠️ Partially Tested  
**Operands:** `reg` → `reg = reg - 1`  
**Encoding:** `[opcode] [reg]`

**Edge Cases Tested:**
- Basic decrement: `x → x-1`
- Zero flag after decrementing 1

**Edge Cases Needed:**
- Underflow: `0 - 1 → 0xFFFFFFFF` (with carry flag)
- Decrement to zero
- Multiple consecutive decrements

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### AND (0x15) - Bitwise AND
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 & reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic AND operation
- AND with 0: `x & 0 = 0`
- AND with 0xFFFFFFFF: `x & 0xFFFFFFFF = x`

**Edge Cases Needed:**
- Self-AND: `x & x = x` (idempotent)
- AND for bit masking
- Zero flag behavior
- Sign flag behavior

**Related Tests:**
- `tests/logical.test.asm`

---

### OR (0x16) - Bitwise OR
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 | reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic OR operation
- OR with 0: `x | 0 = x`
- OR with 0xFFFFFFFF: `x | 0xFFFFFFFF = 0xFFFFFFFF`

**Edge Cases Needed:**
- Self-OR: `x | x = x` (idempotent)
- OR for bit setting
- Flag behavior

**Related Tests:**
- `tests/logical.test.asm`

---

### XOR (0x17) - Bitwise XOR
**Status:** ⚠️ Partially Tested  
**Operands:** `reg1, reg2` → `reg1 = reg1 ^ reg2`  
**Encoding:** `[opcode] [reg1] [reg2]`

**Edge Cases Tested:**
- Basic XOR operation
- XOR with 0: `x ^ 0 = x`
- Self-XOR: `x ^ x = 0` (common zeroing idiom)

**Edge Cases Needed:**
- Double XOR: `(x ^ y) ^ y = x` (XOR swap pattern)
- XOR with 0xFFFFFFFF (bit inversion)
- Flag behavior

**Related Tests:**
- `tests/logical.test.asm`

---

### NOT (0x18) - Bitwise NOT
**Status:** ⚠️ Partially Tested  
**Operands:** `reg` → `reg = ~reg`  
**Encoding:** `[opcode] [reg]`

**Edge Cases Tested:**
- Basic NOT operation
- NOT of 0: `~0 = 0xFFFFFFFF`
- NOT of 0xFFFFFFFF: `~0xFFFFFFFF = 0`

**Edge Cases Needed:**
- Double NOT: `~~x = x` (idempotent twice)
- Flag behavior

**Related Tests:**
- `tests/logical.test.asm`

---

### SHL (0x19) - Shift Left
**Status:** ⚠️ Partially Tested  
**Operands:** `reg, count` → `reg = reg << count`  
**Encoding:** `[opcode] [reg] [imm8]`

**Edge Cases Tested:**
- Shift by 1
- Shift by powers of 2

**Edge Cases Needed:**
- Shift by 0 (no-op)
- Shift by 31 (maximum meaningful shift)
- Shift by ≥32 (undefined or wrap behavior)
- Carry flag from MSB
- Overflow flag behavior

**Related Tests:**
- `tests/logical.test.asm`

---

### SHR (0x1A) - Shift Right
**Status:** ⚠️ Partially Tested  
**Operands:** `reg, count` → `reg = reg >> count`  
**Encoding:** `[opcode] [reg] [imm8]`

**Edge Cases Tested:**
- Shift by 1
- Shift by powers of 2

**Edge Cases Needed:**
- Shift by 0 (no-op)
- Shift by 31
- Shift by ≥32
- Carry flag from LSB
- Logical vs arithmetic shift (sign extension)

**Related Tests:**
- `tests/logical.test.asm`

---

### CALL (0x1B) - Call Subroutine
**Status:** ⚠️ Partially Tested  
**Operands:** `address`  
**Encoding:** `[opcode] [address32]`

**Edge Cases Tested:**
- Basic function call
- Return address pushed to stack
- Stack pointer adjustment

**Edge Cases Needed:**
- Maximum call depth (stack overflow detection)
- Recursive calls
- Call to address 0
- Call outside program bounds
- Call/return pairing mismatch

**Related Tests:**
- `tests/control_flow.test.asm`
- `tests/stack.test.asm`

---

### RET (0x1C) - Return from Subroutine
**Status:** ⚠️ Partially Tested  
**Operands:** None  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Basic return
- Stack pointer restoration
- Return address popped correctly

**Edge Cases Needed:**
- Return without matching CALL
- Stack underflow on return
- Multiple consecutive returns
- Return to invalid address

**Related Tests:**
- `tests/control_flow.test.asm`
- `tests/stack.test.asm`

---

### PUSH (0x09) - Push Register to Stack
**Status:** ⚠️ Partially Tested  
**Operands:** `reg`  
**Encoding:** `[opcode] [reg]`

**Edge Cases Tested:**
- Push single value
- Multiple pushes
- Stack growth

**Edge Cases Needed:**
- Stack overflow (push beyond memory)
- Push all registers sequentially
- Push/pop symmetry verification

**Related Tests:**
- `tests/stack.test.asm`

---

### POP (0x0A) - Pop Stack to Register
**Status:** ⚠️ Partially Tested  
**Operands:** `reg`  
**Encoding:** `[opcode] [reg]`

**Edge Cases Tested:**
- Pop single value
- Multiple pops
- LIFO order verification

**Edge Cases Needed:**
- Stack underflow (pop from empty stack)
- Pop to all registers
- Pop without matching push

**Related Tests:**
- `tests/stack.test.asm`

---

## Memory & I/O Operations (0x20-0x4F)

### LOAD (0x06) - Load from Memory
**Status:** ⚠️ Partially Tested  
**Operands:** `reg, [address]` → `reg = memory[address]`  
**Encoding:** `[opcode] [reg] [address32]`

**Notes:**
- LOAD is **byte-oriented**: it loads a single byte from memory and zero-extends it into the destination register.

**Edge Cases Tested:**
- Load from valid address
- Load after STORE

**Edge Cases Needed:**
- Load from address 0
- Load from uninitialized memory
- Load from out-of-bounds address (error handling)
- Load across memory boundaries
- Alignment issues (if applicable)

**Related Tests:**
- `tests/memory.test.asm`

---

### STORE (0x07) - Store to Memory
**Status:** ⚠️ Partially Tested  
**Operands:** `reg, [address]` → `memory[address] = reg`  
**Encoding:** `[opcode] [reg] [address32]`

**Notes:**
- STORE is **byte-oriented**: it stores only the low 8 bits of the source register into memory.

**Edge Cases Tested:**
- Store to valid address
- Store/load round-trip

**Edge Cases Needed:**
- Store to address 0
- Store to read-only memory (if supported)
- Store beyond memory bounds
- Overwrite existing data
- Multiple stores to same address

**Related Tests:**
- `tests/memory.test.asm`

---

### LOADR (0x41) - Load Register Indirect
**Status:** ⚠️ Partially Tested  
**Operands:** `dst, [src_reg]` → `dst = memory[src_reg]`  
**Encoding:** `[opcode] [dst] [src_reg]`

**Edge Cases Tested:**
- Indirect load through register
- Pointer dereferencing

**Edge Cases Needed:**
- LOADR with address in R0
- LOADR with invalid address in register
- LOADR through null pointer (address 0)
- Chain of indirect loads

**Related Tests:**
- `tests/memory.test.asm`
- `tests/loadr.test.asm`

---

### STORER (0x43) - Store Register Indirect
**Status:** ⚠️ Partially Tested  
**Operands:** `[addr_reg], value_reg` → `memory[addr_reg] = value_reg`  
**Encoding:** `[opcode] [addr_reg] [value_reg]`

**Notes:**
- STORER is **byte-oriented**: it stores only the low 8 bits of `value_reg` into `memory[address]`.

**Edge Cases Tested:**
- Indirect store through register
- Pointer write

**Edge Cases Needed:**
- STORER with invalid address
- STORER to null pointer
- STORER overwrite verification

**Related Tests:**
- `tests/memory.test.asm`

---

### LEA (0x20) - Load Effective Address
**Status:** ❌ Minimal Testing  
**Operands:** `reg, [expression]` → `reg = address_of(expression)`  
**Encoding:** `[opcode] [reg] [address32]`

**Edge Cases Tested:**
- Basic address loading

**Edge Cases Needed:**
- LEA with label
- LEA with offset calculation
- LEA for pointer arithmetic
- LEA of stack address

**Related Tests:**
- `tests/memory.test.asm`

---

### SWAP (0x21) - Swap Register and Memory
**Status:** ❌ Minimal Testing  
**Operands:** `reg, [address]` → `temp = reg; reg = memory[address]; memory[address] = temp`  
**Encoding:** `[opcode] [reg] [address32]`

**Edge Cases Tested:**
- Basic swap operation

**Edge Cases Needed:**
- Swap with same location (no-op?)
- Multiple consecutive swaps
- Swap at memory boundaries

**Related Tests:**
- `tests/memory.test.asm`

---

### IN (0x30-0x39) - I/O Input Operations
**Status:** ❌ Minimal Testing  
**Variants:** IN, INB, INW, INL, INSTR  
**Operands:** `reg, port`  
**Encoding:** `[opcode] [reg] [port]`

**Edge Cases Tested:**
- Basic input from port

**Edge Cases Needed:**
- Input from non-existent port
- Input with no data available
- Input buffer overflow
- Multiple consecutive inputs
- Input size variants (byte, word, long, string)

**Related Tests:**
- `tests/data.test.asm`

---

### OUT (0x31-0x39) - I/O Output Operations
**Status:** ❌ Minimal Testing  
**Variants:** OUT, OUTB, OUTW, OUTL, OUTSTR  
**Operands:** `port, reg`  
**Encoding:** `[opcode] [port] [reg]`

**Edge Cases Tested:**
- Basic output to port

**Edge Cases Needed:**
- Output to non-existent port
- Output buffer full
- Multiple consecutive outputs
- Output size variants

**Related Tests:**
- `tests/data.test.asm`

---

### DB (0x40) - Define Byte
**Status:** ⚠️ Partially Tested  
**Operands:** Inline data bytes  
**Encoding:** `[opcode] [byte...]`

**Edge Cases Tested:**
- Define single byte
- Define multiple bytes
- Define string data

**Edge Cases Needed:**
- DB with zero bytes
- DB with 0xFF (max value)
- DB with large arrays
- DB alignment

**Related Tests:**
- `tests/data.test.asm`

---

## 64-bit Operations (0x50-0x5F)

### ADD64 (0x50) - 64-bit Addition
**Status:** ⚠️ Partially Tested  
**Operands:** `reg_pair1, reg_pair2` (uses consecutive registers)  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- Basic 64-bit addition
- Addition across 32-bit boundary

**Edge Cases Needed:**
- 64-bit overflow: `0xFFFFFFFFFFFFFFFF + 1`
- Carry propagation from lower to upper 32 bits
- Addition with zero
- Maximum value addition

**Related Tests:**
- `tests/arithmetic.test.asm`
- `tests/64bit/` (if exists)

---

### SUB64 (0x51) - 64-bit Subtraction
**Status:** ⚠️ Partially Tested  
**Operands:** `reg_pair1, reg_pair2`  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- Basic 64-bit subtraction

**Edge Cases Needed:**
- 64-bit underflow: `0 - 1`
- Borrow propagation from lower to upper 32 bits
- Subtraction resulting in zero

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### MOV64 (0x52) - 64-bit Move
**Status:** ⚠️ Partially Tested  
**Operands:** `dst_pair, src_pair`  
**Encoding:** `[opcode] [dst_low] [src_low]`

**Edge Cases Tested:**
- 64-bit register copy

**Edge Cases Needed:**
- Self-move
- Move of zero
- Move of maximum 64-bit value

**Related Tests:**
- `tests/registers.test.asm`

---

### LOAD_IMM64 (0x53) - Load 64-bit Immediate
**Status:** ✅ Fully Tested  
**Operands:** `reg_pair, imm64`  
**Encoding:** `[opcode] [reg_low] [imm64_bytes]`

**Edge Cases Tested:**
- Various 64-bit immediate values
- Loading into XMM registers (128-bit, uses pairs)
- Zero value
- Maximum value

**Edge Cases Needed:**
- All power-of-2 values
- Values crossing 32-bit boundary

**Related Tests:**
- `tests/simd.test.asm` (XMM usage)
- `tests/64bit/`

---

### MUL64 (0x54) - 64-bit Multiplication
**Status:** ❌ Minimal Testing  
**Operands:** `reg_pair1, reg_pair2`  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- Basic 64-bit multiplication

**Edge Cases Needed:**
- Overflow behavior (128-bit result handling)
- Multiplication by zero
- Multiplication by one
- Large value multiplication

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### DIV64 (0x55) - 64-bit Division
**Status:** ❌ Minimal Testing  
**Operands:** `reg_pair1, reg_pair2`  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- Basic 64-bit division

**Edge Cases Needed:**
- Division by zero (exception) 🔴
- Division of maximum value
- Division with remainder

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### CMP64 (0x5C) - 64-bit Compare
**Status:** ❌ Minimal Testing  
**Operands:** `reg_pair1, reg_pair2`  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- Basic 64-bit comparison

**Edge Cases Needed:**
- Equality comparison
- Less-than with upper bits equal
- Flag setting for all comparisons

**Related Tests:**
- `tests/control_flow.test.asm`

---

### INC64 (0x5D) - 64-bit Increment
**Status:** ❌ Minimal Testing  
**Operands:** `reg_pair`  
**Encoding:** `[opcode] [reg_low]`

**Edge Cases Tested:**
- Basic increment

**Edge Cases Needed:**
- Overflow: `0xFFFFFFFFFFFFFFFF + 1`
- Carry from lower to upper 32 bits

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### DEC64 (0x5E) - 64-bit Decrement
**Status:** ❌ Minimal Testing  
**Operands:** `reg_pair`  
**Encoding:** `[opcode] [reg_low]`

**Edge Cases Tested:**
- Basic decrement

**Edge Cases Needed:**
- Underflow: `0 - 1`
- Borrow from upper 32 bits

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### MOD64 (0x5F) - 64-bit Modulo
**Status:** 🔴 Untested  
**Operands:** `reg_pair1, reg_pair2`  
**Encoding:** `[opcode] [reg_low] [reg_low]`

**Edge Cases Tested:**
- None

**Edge Cases Needed:**
- All edge cases from MOD (32-bit)
- Modulo by zero (exception) 🔴
- Large value modulo

**Related Tests:**
- None currently

---

## Extended Register Operations (0x60-0x6F)

### MOVEX (0x60) - Move Extended Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg_extended, reg_extended` (R8-R15)  
**Encoding:** `[opcode] [dst] [src]`

**Edge Cases Tested:**
- Move between R8-R15

**Edge Cases Needed:**
- Move to/from R0-R7
- All extended register pairs
- Move across register bank boundaries

**Related Tests:**
- `tests/registers.test.asm`

---

### ADDEX (0x61) - Add Extended Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg_extended, reg_extended`  
**Encoding:** `[opcode] [dst] [src]`

**Edge Cases Tested:**
- Basic extended register addition

**Edge Cases Needed:**
- Overflow behavior
- Mixing extended and base registers

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### SUBEX (0x62) - Subtract Extended Registers
**Status:** ⚠️ Partially Tested  
**Operands:** `reg_extended, reg_extended`  
**Encoding:** `[opcode] [dst] [src]`

**Edge Cases Tested:**
- Basic extended register subtraction

**Edge Cases Needed:**
- Underflow behavior
- Flag interactions

**Related Tests:**
- `tests/arithmetic.test.asm`

---

### LOADEX (0x66) - Load to Extended Register
**Status:** ❌ Minimal Testing  
**Operands:** `reg_extended, [address]`  
**Encoding:** `[opcode] [reg] [address32]`

**Edge Cases Tested:**
- Basic memory load to R8-R15

**Edge Cases Needed:**
- Load from various memory locations
- Boundary conditions

**Related Tests:**
- `tests/memory.test.asm`

---

### STOREX (0x67) - Store from Extended Register
**Status:** ❌ Minimal Testing  
**Operands:** `[address], reg_extended`  
**Encoding:** `[opcode] [address32] [reg]`

**Edge Cases Tested:**
- Basic memory store from R8-R15

**Edge Cases Needed:**
- Store to various memory locations
- Overwrite verification

**Related Tests:**
- `tests/memory.test.asm`

---

## CPU Mode Control (0x70-0x7F)

### MODE32 (0x70) - Switch to 32-bit Mode
**Status:** ⚠️ Partially Tested  
**Operands:** None  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Mode switch from 64-bit to 32-bit
- Register truncation behavior

**Edge Cases Needed:**
- Switch while executing 64-bit instructions
- Switch with data in upper 32 bits of registers
- Multiple mode switches

**Related Tests:**
- `tests/mode_switching.test.asm`

---

### MODE64 (0x71) - Switch to 64-bit Mode
**Status:** ⚠️ Partially Tested  
**Operands:** None  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Mode switch from 32-bit to 64-bit
- Register extension behavior

**Edge Cases Needed:**
- Switch with instructions already in flight
- Flag behavior across mode switches

**Related Tests:**
- `tests/mode_switching.test.asm`

---

### MODECMP (0x72) - Compare Current Mode
**Status:** ❌ Minimal Testing  
**Operands:** `mode_value`  
**Encoding:** `[opcode] [mode_byte]`

**Edge Cases Tested:**
- Basic mode comparison

**Edge Cases Needed:**
- Comparison in both modes
- Flag setting behavior

**Related Tests:**
- `tests/mode_switching.test.asm`

---

## SSE Operations (0x80-0x99)

### MOVAPS (0x80) - Move Aligned Packed Single
**Status:** ✅ Fully Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- XMM register to XMM register move
- Move of 4 packed floats (128-bit)
- Zero value move
- Large value move

**Edge Cases Needed:**
- Alignment violation detection (if enforced)
- Self-move: `MOVAPS XMM0, XMM0`

**Related Tests:**
- `tests/simd.test.asm`

---

### MOVUPS (0x81) - Move Unaligned Packed Single
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Unaligned move (VM doesn't enforce alignment)

**Edge Cases Needed:**
- Performance difference from MOVAPS (if any)
- Unaligned memory access patterns

**Related Tests:**
- `tests/simd.test.asm`

---

### ADDPS (0x82) - Add Packed Single
**Status:** ✅ Fully Tested  
**Operands:** `xmm_dst, xmm_src` → `dst[i] = dst[i] + src[i]` for i=0..3  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Addition of 4 packed floats
- Packed float bit pattern verification
- Zero addition
- Large value addition

**Edge Cases Needed:**
- Floating-point overflow (inf)
- NaN propagation
- Denormal number handling
- Precision edge cases

**Related Tests:**
- `tests/simd.test.asm`

---

### SUBPS (0x83) - Subtract Packed Single
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic packed float subtraction

**Edge Cases Needed:**
- Floating-point underflow
- Self-subtraction (zero result)
- NaN/inf handling

**Related Tests:**
- `tests/simd.test.asm`

---

### MULPS (0x84) - Multiply Packed Single
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic packed float multiplication

**Edge Cases Needed:**
- Multiplication by zero
- Multiplication by one
- Overflow to infinity
- Underflow to zero (denormals)

**Related Tests:**
- `tests/simd.test.asm`

---

### DIVPS (0x85) - Divide Packed Single
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic packed float division

**Edge Cases Needed:**
- Division by zero (inf result)
- Division of zero
- Division of inf
- NaN propagation

**Related Tests:**
- `tests/simd.test.asm`

---

### SQRTPS (0x86) - Square Root Packed Single
**Status:** ❌ Minimal Testing  
**Operands:** `xmm_dst, xmm_src` → `dst[i] = sqrt(src[i])`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic square root

**Edge Cases Needed:**
- Square root of zero
- Square root of negative (NaN)
- Square root of one
- Square root of very small numbers

**Related Tests:**
- `tests/simd.test.asm`

---

### MAXPS (0x87) - Maximum Packed Single
**Status:** ❌ Minimal Testing  
**Operands:** `xmm_dst, xmm_src` → `dst[i] = max(dst[i], src[i])`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic maximum selection

**Edge Cases Needed:**
- Max with NaN (NaN propagation)
- Max of equal values
- Max with +inf/-inf
- Max with zero (+0.0 vs -0.0)

**Related Tests:**
- `tests/simd.test.asm`

---

### MINPS (0x88) - Minimum Packed Single
**Status:** ❌ Minimal Testing  
**Operands:** `xmm_dst, xmm_src` → `dst[i] = min(dst[i], src[i])`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic minimum selection

**Edge Cases Needed:**
- Min with NaN
- Min with +inf/-inf
- Min with zero

**Related Tests:**
- `tests/simd.test.asm`

---

### ANDPS (0x89) - Bitwise AND Packed Single
**Status:** ❌ Minimal Testing  
**Operands:** `xmm_dst, xmm_src` (bitwise AND of float bit patterns)  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic bitwise AND

**Edge Cases Needed:**
- AND with zero mask (clear all)
- AND with 0xFFFFFFFF (no change)
- Sign bit manipulation

**Related Tests:**
- `tests/simd.test.asm`

---

### ORPS (0x8A) - Bitwise OR Packed Single
**Status:** ❌ Minimal Testing  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic bitwise OR

**Edge Cases Needed:**
- OR with zero (no change)
- OR with 0xFFFFFFFF (set all)

**Related Tests:**
- `tests/simd.test.asm`

---

### XORPS (0x8B) - Bitwise XOR Packed Single
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic bitwise XOR
- Self-XOR (common zeroing idiom): `XORPS XMM0, XMM0`

**Edge Cases Needed:**
- XOR for sign bit flip
- Double XOR (identity)

**Related Tests:**
- `tests/simd.test.asm`

---

### CMPPS (0x8C) - Compare Packed Single
**Status:** ✅ Fully Tested (Just Implemented)  
**Operands:** `xmm_dst, xmm_src, imm8`  
**Encoding:** `[opcode] [dst_xmm] [src_xmm] [predicate]`

**Edge Cases Tested:**
- All 8 comparison predicates (EQ, LT, LE, UNORD, NEQ, NLT, NLE, ORD)
- Result bit pattern (0xFFFFFFFF for true, 0x00000000 for false)

**Edge Cases Needed:**
- Comparison with NaN (unordered results)
- Comparison with +inf/-inf
- Comparison with zero (+0.0 vs -0.0)
- All predicate combinations

**Related Tests:**
- Create `tests/sse_compare.test.asm` 🔴

---

### MOVAPD (0x8D) - Move Aligned Packed Double
**Status:** ⚠️ Partially Tested  
**Operands:** `xmm_dst, xmm_src` (2 doubles)  
**Encoding:** `[opcode] [dst_xmm] [src_xmm]`

**Edge Cases Tested:**
- Basic move of 2 packed doubles

**Edge Cases Needed:**
- Alignment violations (if enforced)
- All XMM register pairs

**Related Tests:**
- `tests/simd.test.asm`

---

### Packed Double Operations (0x8D-0x99)
**Status:** ⚠️ Partially Tested for all

All packed double operations (MOVAPD, MOVUPD, ADDPD, SUBPD, MULPD, DIVPD, SQRTPD, MAXPD, MINPD, ANDPD, ORPD, XORPD, CMPPD) have similar edge cases to their packed single counterparts, but operating on 2 doubles instead of 4 floats.

**Additional Edge Cases Needed:**
- Double precision overflow/underflow
- Double precision NaN/inf handling
- Denormal double handling

**Related Tests:**
- `tests/simd.test.asm`
- Create `tests/sse_double.test.asm` 🔴

---

## FPU Operations (0xA0-0xBF)

### FLD (0xA0) - Load Floating Point
**Status:** ⚠️ Partially Tested  
**Operands:** `st(0), source` (push to FPU stack)  
**Encoding:** Varies

**Edge Cases Tested:**
- Load from memory
- Load immediate constants (FLDZ, FLD1, FLDPI)

**Edge Cases Needed:**
- Stack overflow (8 registers)
- Load denormals
- Load NaN/inf
- Load zero (-0.0 vs +0.0)

**Related Tests:**
- `tests/fpu.test.asm`

---

### FST/FSTP (0xA1/0xA2) - Store Floating Point
**Status:** ⚠️ Partially Tested  
**Operands:** `destination, st(0)` (FSTP also pops)  
**Encoding:** Varies

**Edge Cases Tested:**
- Store to memory
- Store and pop (FSTP)

**Edge Cases Needed:**
- Store with rounding modes
- Store overflow/underflow
- Store NaN/inf

**Related Tests:**
- `tests/fpu.test.asm`

---

### FADD/FSUB/FMUL/FDIV (0xA6-0xA9) - FPU Arithmetic
**Status:** ⚠️ Partially Tested  
**Operands:** `st(0), st(1)` or memory  
**Encoding:** Varies

**Edge Cases Tested:**
- Basic FPU arithmetic
- Stack operations

**Edge Cases Needed:**
- Overflow to infinity
- Underflow to zero
- Division by zero (inf)
- NaN propagation
- Denormal handling
- Precision control

**Related Tests:**
- `tests/fpu.test.asm`

---

### FSIN/FCOS/FTAN (0xAA-0xAC) - Trigonometric Functions
**Status:** ⚠️ Partially Tested  
**Operands:** `st(0)` (in-place)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Basic trigonometric operations

**Edge Cases Needed:**
- Argument reduction for large values
- Special values (0, π, π/2, etc.)
- NaN/inf input
- Precision at extremes

**Related Tests:**
- `tests/fpu.test.asm`

---

### FSQRT (0xAD) - Floating Point Square Root
**Status:** ⚠️ Partially Tested  
**Operands:** `st(0)` (in-place)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Basic square root

**Edge Cases Needed:**
- Square root of negative (NaN)
- Square root of zero
- Square root of infinity
- Denormal results

**Related Tests:**
- `tests/fpu.test.asm`

---

### FABS/FCHS (0xAE/0xAF) - Absolute Value / Change Sign
**Status:** ⚠️ Partially Tested  
**Operands:** `st(0)` (in-place)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Basic absolute value and sign change

**Edge Cases Needed:**
- FABS of NaN (preserve NaN, clear sign bit)
- FABS/FCHS of zero (+0.0/-0.0)
- FABS/FCHS of infinity

**Related Tests:**
- `tests/fpu.test.asm`

---

### FINIT (0xB0) - Initialize FPU
**Status:** ⚠️ Partially Tested  
**Operands:** None  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- FPU initialization
- Clear exception flags

**Edge Cases Needed:**
- FINIT after exceptions
- FINIT with values on stack
- Control word reset verification

**Related Tests:**
- `tests/fpu.test.asm`

---

### FCOMPP/FUCOMPP (0xB5/0xB6) - Compare and Pop
**Status:** ❌ Minimal Testing  
**Operands:** `st(0), st(1)` (compare and pop both)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Basic comparison

**Edge Cases Needed:**
- Comparison with NaN (unordered)
- Comparison with inf
- Comparison with zero
- Flag setting in RFLAGS/FSTSW

**Related Tests:**
- `tests/fpu.test.asm`

---

### FSTCW/FLDCW (0xB2/0xB3) - Control Word Management
**Status:** ❌ Minimal Testing  
**Operands:** Memory location  
**Encoding:** `[opcode] [address]`

**Edge Cases Tested:**
- Store/load control word

**Edge Cases Needed:**
- Rounding mode changes
- Precision control changes
- Exception mask changes

**Related Tests:**
- `tests/fpu.test.asm`

---

### FSTSW (0xB4) - Store Status Word
**Status:** ❌ Minimal Testing  
**Operands:** Memory or AX register  
**Encoding:** `[opcode] [dest]`

**Edge Cases Tested:**
- Basic status word storage

**Edge Cases Needed:**
- Status word after exceptions
- Status word after comparisons
- C0-C3 condition code bits

**Related Tests:**
- `tests/fpu.test.asm`

---

## SIMD Vector Operations (0xD4-0xDB)

### VADD (0xD4) - Vector Add (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R0-R3 = R0-R3 + R4-R7`  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Addition of 4-element vectors
- Element-wise addition
- Result in destination registers

**Edge Cases Needed:**
- Overflow behavior
- Large value vectors

**Related Tests:**
- `tests/simd.test.asm`

---

### VMUL (0xD5) - Vector Multiply (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R0-R3 = R0-R3 * R4-R7`  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Element-wise multiplication
- Vector result storage

**Edge Cases Needed:**
- Overflow in multiplication
- Multiplication by zero vector

**Related Tests:**
- `tests/simd.test.asm`

---

### VDOT (0xD6) - Vector Dot Product (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R0 = R0*R4 + R1*R5 + R2*R6 + R3*R7`  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Dot product calculation
- Result in single register

**Edge Cases Needed:**
- Overflow in accumulation
- Dot product with zero vector

**Related Tests:**
- `tests/simd.test.asm`

---

### VMAX (0xD7) - Vector Maximum (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R0 = max(R0, R1, R2, R3)`  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Maximum element selection from vector

**Edge Cases Needed:**
- All elements equal
- Negative values (signed)

**Related Tests:**
- `tests/simd.test.asm`

---

### VBROADCAST (0xD8) - Vector Broadcast (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R4-R7 = R0` (broadcast R0 to all)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Broadcast single value to vector

**Edge Cases Needed:**
- Broadcast zero
- Broadcast maximum value

**Related Tests:**
- `tests/simd.test.asm`

---

### VCMPGT (0xD9) - Vector Compare Greater Than (Custom)
**Status:** ⚠️ Partially Tested  
**Operands:** `R0-R3 = (R0-R3 > R4-R7) ? 1 : 0`  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Element-wise comparison

**Edge Cases Needed:**
- Comparison with equal values
- Comparison with negative values
- All-true and all-false results

**Related Tests:**
- `tests/simd.test.asm`

---

### PACKB (0xDA) - Pack Bytes (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R4 = pack(R0, R1, R2, R3)` (4 bytes into 32-bit)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Byte packing from 4 registers

**Edge Cases Needed:**
- Pack with zero bytes
- Pack with 0xFF bytes
- Endianness verification

**Related Tests:**
- `tests/simd.test.asm`

---

### UNPACKB (0xDB) - Unpack Bytes (Custom)
**Status:** ✅ Fully Tested  
**Operands:** `R0-R3 = unpack(R4)` (32-bit into 4 bytes)  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Byte unpacking to 4 registers
- Round-trip with PACKB

**Edge Cases Needed:**
- Unpack zero
- Unpack 0xFFFFFFFF

**Related Tests:**
- `tests/simd.test.asm`

---

## Special Operations

### INT (0xCD) - Software Interrupt
**Status:** ⚠️ Partially Tested  
**Operands:** `interrupt_number`  
**Encoding:** `[opcode] [int_num]`

**Edge Cases Tested:**
- System call invocation
- Basic interrupt handling

**Edge Cases Needed:**
- Invalid interrupt number
- Nested interrupts
- Interrupt return (IRET)
- Interrupt during interrupt

**Related Tests:**
- Need `tests/interrupts.test.asm` 🔴

---

### HALT (0xFF) - Halt Execution
**Status:** ✅ Fully Tested  
**Operands:** None  
**Encoding:** `[opcode]`

**Edge Cases Tested:**
- Program termination
- HALT as last instruction
- HALT in various contexts

**Edge Cases Needed:**
- HALT in infinite loop
- HALT after all test paths

**Related Tests:**
- All test files (termination)

---

## Test Coverage Summary

### Coverage Statistics

| Category | Opcodes | Fully Tested | Partially Tested | Minimal | Untested |
|----------|---------|--------------|------------------|---------|----------|
| Basic Ops | 32 | 5 (16%) | 22 (69%) | 3 (9%) | 2 (6%) |
| Memory/IO | 18 | 1 (6%) | 8 (44%) | 7 (39%) | 2 (11%) |
| 64-bit | 14 | 1 (7%) | 6 (43%) | 6 (43%) | 1 (7%) |
| Extended | 10 | 0 (0%) | 3 (30%) | 7 (70%) | 0 (0%) |
| Mode | 3 | 0 (0%) | 2 (67%) | 1 (33%) | 0 (0%) |
| SSE | 26 | 3 (12%) | 15 (58%) | 8 (30%) | 0 (0%) |
| FPU | 22 | 0 (0%) | 10 (45%) | 12 (55%) | 0 (0%) |
| Vector | 8 | 6 (75%) | 2 (25%) | 0 (0%) | 0 (0%) |
| **Total** | **133** | **16 (12%)** | **68 (51%)** | **44 (33%)** | **5 (4%)** |

### Test Files Overview

1. **tests/opcode_operands.test.asm** (11 tests, 42 assertions)
   - Operand format edge cases (decimal, hex, binary, octal)
   - Immediate value edge cases
   - Register addressing
   - Memory addressing

2. **tests/arithmetic.test.asm** (12 tests)
   - Basic arithmetic operations
   - Flag behavior
   - Overflow/underflow detection

3. **tests/simd.test.asm** (7 tests, 21 assertions)
   - SSE packed single operations
   - Custom vector operations
   - XMM register usage

4. **tests/control_flow.test.asm** (7 tests)
   - Conditional jumps
   - Function calls
   - Stack operations

5. **tests/memory.test.asm** (9 tests)
   - Load/store operations
   - Indirect addressing
   - Memory boundaries

6. **tests/fpu.test.asm** (7 tests)
   - Floating-point arithmetic
   - Transcendental functions
   - FPU stack management

7. **tests/logical.test.asm** (7 tests)
   - Bitwise operations
   - Shift operations
   - Flag behavior

8. **tests/stack.test.asm** (4 tests)
   - Push/pop operations
   - LIFO verification
   - Stack pointer management

9. **tests/registers.test.asm** (6 tests)
   - Register aliases
   - Extended registers
   - Register move operations

10. **tests/mode_switching.test.asm** (mode tests)
    - 32-bit/64-bit mode transitions
    - Register width changes

---

## Missing Edge Cases

### High Priority (Should Implement)

1. **Division by Zero Handling** 🔴
   - DIV, MOD, DIV64, MOD64
   - DIVPS, DIVPD (should produce inf)
   - Test exception/error reporting

2. ~~**SSE Comparison Tests**~~ ✅ **COMPLETED** (Phase 1)
   - ~~Create `tests/sse_compare.test.asm`~~
   - ~~Test all 8 CMPPS/CMPPD predicates~~
   - ~~Test NaN/inf handling in comparisons~~
   - **Status:** 20 tests, all passing

3. **Interrupt Handling** 🔴
   - Create `tests/interrupts.test.asm`
   - Test INT/IRET pairing
   - Test nested interrupts

4. **FPU Exception Handling** 🔴
   - Invalid operations (sqrt of negative)
   - Overflow/underflow
   - Denormals
   - NaN propagation

5. **Memory Boundary Tests** 🔴
   - Access at address 0
   - Access beyond memory size
   - Unaligned access (if applicable)

6. **Stack Overflow/Underflow** 🔴
   - Push beyond stack limit
   - Pop from empty stack
   - CALL depth limit

### Medium Priority (Nice to Have)

7. ~~**64-bit Arithmetic Overflow**~~ ✅ **COMPLETED** (Phase 2)
   - ~~Comprehensive 64-bit edge cases~~
   - ~~Carry/borrow propagation~~
   - **Status:** 4 tests (ADD64, SUB64, INC64, DEC64 boundary cases), all passing

8. ~~**32-bit Arithmetic Overflow/Underflow**~~ ✅ **COMPLETED** (Phase 2)
   - ~~ADD, SUB, INC, DEC overflow tests~~
   - ~~MUL overflow with large values~~
   - ~~Boundary value arithmetic~~
   - ~~Mixed operations~~
   - **Status:** 22 tests, all passing

9. **Extended Register Coverage**
   - All R8-R15 combinations
   - Mixed operations with R0-R7

10. **Shift Operation Extremes**
    - Shift by 0
    - Shift by > 32
    - Undefined shift behavior

11. **FPU Rounding Modes**
    - Test all rounding modes
    - Precision control

12. **SSE Packed Double Coverage**
    - Create `tests/sse_double.test.asm`
    - Mirror packed single tests

13. **I/O Port Edge Cases**
    - Non-existent ports
    - Buffer overflow/underflow

### Low Priority (Future Enhancement)

13. **Performance Edge Cases**
    - Instruction fusion opportunities
    - Branch prediction

14. **Concurrent Execution**
    - If threading supported

15. **Optimization Edge Cases**
    - Dead code elimination
    - Register allocation stress

---

## Recommendations

### Immediate Actions

1. **Create missing test files:**
   ```bash
   tests/interrupts.test.asm       # INT/IRET handling
   tests/division_errors.test.asm  # Division by zero
   tests/fpu_exceptions.test.asm   # FPU error handling
   tests/memory_bounds.test.asm    # Out-of-bounds access
   ```

2. **Enhance existing tests:**
   - ~~Add overflow/underflow tests to `arithmetic.test.asm`~~ ✅ **DONE** (created separate `overflow.test.asm`)
   - Add NaN/inf tests to `fpu.test.asm` and `simd.test.asm`
   - Add boundary tests to `memory.test.asm`

3. **Add negative test cases:**
   - Tests that expect errors/exceptions
   - Verify error reporting mechanisms

### Completed Recent Work

1. ✅ **Phase 1: SSE Comparison Tests** (January 14, 2026)
   - Created `tests/sse_compare.test.asm` with 20 comprehensive tests
   - All 8 CMPPS predicates tested (EQ, LT, LE, UNORD, NEQ, NLT, NLE, ORD)
   - All 8 CMPPD predicates tested
   - Edge cases: self-comparison, boundary values, zero comparisons
   - Result: 20/20 passing, coverage increased to ~75%

2. ✅ **Phase 2: Arithmetic Overflow Tests** (January 14, 2026)
   - Created `tests/overflow.test.asm` with 26 comprehensive tests
   - 32-bit overflow: ADD, INC, MUL wrap-around behavior
   - 32-bit underflow: SUB, DEC wrap-around behavior
   - 64-bit operations: ADD64, SUB64, INC64, DEC64 boundary cases
   - Multiplication overflow with large values
   - Boundary value arithmetic near overflow/underflow
   - Mixed operations (overflow→underflow chains)
   - Result: 26/26 passing, coverage increased to ~76%

### Test Framework Enhancements

1. **Add `.assert_error` directive:**
   - Verify expected exceptions occur
   - Check error codes

2. **Add `.assert_flags` directive:**
   - Verify specific flag combinations
   - Test carry, overflow, sign, zero flags

3. **Add performance assertions:**
   - Maximum cycle count
   - Instruction fusion verification

---

## Conclusion

The DemiEngine has **133 implemented opcodes** with a current test coverage of:
- ✅ 12% fully tested with comprehensive edge cases
- ⚠️ 51% partially tested with basic functionality
- ❌ 33% minimally tested
- 🔴 4% untested

### Test Suite Statistics

**As of January 14, 2026:**
- **Total Tests:** 506 (up from 460)
- **Tests Passing:** 503 (**99.4% pass rate**, 100% of non-skipped tests)
- **Tests Skipped:** 3 (intentionally skipped tests)
- **Total Assertions:** 727 (100% passing)
- **Comprehensive Coverage:** ~76% (up from ~72%)

**Recent Additions (46 new tests in Phase 2):**
- ✅ **tests/sse_compare.test.asm** (Phase 1): 20 SSE comparison tests (CMPPS/CMPPD with all 8 predicates) - 20/20 passing
- ✅ **tests/overflow.test.asm** (Phase 2): 26 arithmetic overflow/underflow tests - 26/26 passing
  - 32-bit overflow: ADD, INC, MUL causing overflow and wrap-around
  - 32-bit underflow: SUB, DEC causing underflow
  - 64-bit operations: ADD64, SUB64, INC64, DEC64 boundary cases
  - Multiplication overflow with large values
  - Boundary value arithmetic
  - Mixed operations (overflow→underflow chains)

**Previous Additions (88 tests in earlier phases):**
- ✅ **tests/shift_extremes.test.asm**: 16 shift operation edge cases (16/16 passing)
- ✅ **tests/idempotent_ops.test.asm**: 20 self-operation tests (20/20 passing)
- ✅ **tests/immediate_extremes.test.asm**: 19 boundary immediate tests (19/19 passing)
- ✅ **tests/register_exhaustive.test.asm**: 15 register combination tests (15/15 passing)
- ✅ **tests/flag_behavior.test.asm**: 15 CPU flag behavior tests (15/15 passing)

**Test Distribution by Category (all passing!):**
- 64-bit Operations: 44/44 ✅ (expanded with overflow tests)
- Algorithms: 5/5 ✅
- Arithmetic: 43/43 ✅ (expanded with overflow tests)
- Assembler: 8/8 ✅
- Basic: 2/2 ✅
- Benchmarks: 7/7 ✅
- Complex Tests: 1/1 ✅
- Control Flow: 8/8 ✅
- Core Instructions: 33/33 ✅
- Data: 10/10 ✅
- Error Handling: 6/6 ✅
- FPU: 7/7 ✅
- Flags: 16/16 ✅
- Include Test: 1/1 ✅
- Integration: 1/1 ✅
- Logical Operations: 30/30 ✅
- Macro Tests: 1/1 ✅
- Math Tests: 1/1 ✅
- Memory: 9/9 ✅
- Opcode Edgecases: 197/197 ✅
- Platform Tests: 1/1 ✅
- Preprocessor Tests: 5/5 ✅
- Register Tests: 1/1 ✅
- Registers: 21/21 ✅
- SIMD: 29/29 ✅ (includes SSE comparison tests)
- Stack: 4/4 ✅
- Stack Operations: 1/1 ✅
- Test Framework: 9/9 ✅
- Tutorial: 1/1 ✅
- Version Tests: 1/1 ✅
- Core Instructions: 33/33 ✅
- Data: 10/10 ✅
- Error Handling: 6/6 ✅
- FPU: 7/7 ✅
- Flags: 16/16 ✅
- Logical Operations: 30/30 ✅
- Memory: 9/9 ✅
- Opcode Edgecases: 197/197 ✅
- Registers: 21/21 ✅
- SIMD: 9/9 ✅
- Stack: 4/4 ✅
- Test Framework: 9/9 ✅
- **All other categories: 100% passing** ✅

**Coverage Gap:** ~110 additional tests needed for 90% comprehensive coverage (down from 142)

**Overall assessment:** Good basic coverage with improving edge case coverage. Remaining gaps in:
- Error handling (division by zero, memory bounds) - 25 tests needed
- Floating-point special values (NaN, inf, denormals) - 15 tests needed
- SSE comparison predicates - 20 tests needed
- Arithmetic overflow/underflow - 20 tests needed
- Stack overflow/underflow - 10 tests needed
- Interrupt handling - 12 tests needed
- Memory boundary conditions - 10 tests needed

**Next steps:**
1. Fix remaining test failures in immediate_extremes and register_exhaustive
2. Implement medium-priority tests (overflow, FPU exceptions, SSE comparisons)
3. Implement high-priority tests (error handling, division by zero, bounds checking)
4. Add negative test cases (error conditions)
5. Enhance test framework for flag/exception verification
6. Achieve >90% comprehensive edge case coverage

---

## Reserved Opcodes

The following opcodes are **defined but not yet implemented**. They are reserved for future phases:

### 64-bit Operations (5 reserved)

- **OR64 (0x57)** - 64-bit bitwise OR
- **XOR64 (0x58)** - 64-bit bitwise XOR
- **NOT64 (0x59)** - 64-bit bitwise NOT
- **SHL64 (0x5A)** - 64-bit shift left
- **SHR64 (0x5B)** - 64-bit shift right

**Status:** Planned for Phase 3 (Q1 2026)  
**Priority:** High - Required to complete 64-bit instruction set

### Extended Register Operations (5 reserved)

- **MULEX (0x63)** - Multiply with R8-R15 registers
- **DIVEX (0x64)** - Divide with R8-R15 registers
- **CMPEX (0x65)** - Compare R8-R15 registers
- **PUSHEX (0x68)** - Push R8-R15 to stack
- **POPEX (0x69)** - Pop to R8-R15 from stack

**Status:** Planned for Phase 3 (Q1 2026)  
**Priority:** Medium - Completes extended register support

### AVX Operations (23 reserved)

**256-bit SIMD operations** - 0xC0 through 0xD3

- VADDPS, VSUBPS, VMULPS, VDIVPS (packed single)
- VADDPD, VSUBPD, VMULPD, VDIVPD (packed double)
- VSQRTPS, VSQRTPD (square root)
- VMAXPS, VMINPS, VMAXPD, VMINPD (min/max)
- VANDPS, VORPS, VXORPS (logical ops, single)
- VANDPD, VORPD, VXORPD (logical ops, double)

**Status:** Planned for Phase 4 (Q2-Q3 2026)  
**Priority:** Medium-Low - Requires YMM register architecture  
**Note:** Can be emulated using two SSE operations initially

### MMX Operations (11 reserved)

**64-bit SIMD operations** - 0xE0 through 0xEA

- MOVQ (0xE0) - Move quadword
- PADDB, PADDW, PADDD (0xE1-0xE3) - Packed add
- PSUBB, PSUBW, PSUBD (0xE4-0xE6) - Packed subtract
- PCMPEQB, PCMPEQW, PCMPEQD (0xE7-0xE9) - Packed compare
- EMMS (0xEA) - Empty MMX state

**Status:** Planned for Phase 4 (Q2-Q3 2026)  
**Priority:** Low - Legacy technology superseded by SSE  
**Note:** MM registers overlap with FPU stack

### Other Reserved (2 opcodes)

- **DEBUG (0x42)** - Debug directive (print, break, dump)
- **MODEFLAG (0x73)** - Set mode flag in RFLAGS register

**Status:** Low priority  
**Note:** DEBUG may be implemented as needed for debugging support

---

**For comprehensive opcode implementation status, roadmap, and priorities, see [OPCODE_STATUS.md](OPCODE_STATUS.md)**

---

**Document maintained by:** DemiEngine Development Team  
**Contributions:** Submit edge case findings to improve test coverage
