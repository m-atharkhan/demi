# SIMD (Single Instruction, Multiple Data) Reference

## Overview

DemiEngine provides foundational SIMD capabilities for parallel data processing. SIMD operations allow you to perform the same operation on multiple data elements simultaneously, significantly improving performance for data-parallel workloads.

## SIMD Register Architecture

### XMM Registers

- **XMM0 through XMM15**: 128-bit SIMD registers
- Each register can hold:
  - 4 × 32-bit floats
  - 2 × 64-bit doubles
  - 16 × 8-bit integers
  - 8 × 16-bit integers
  - 4 × 32-bit integers
  - 2 × 64-bit integers

### Register Naming

```
Register Ranges (Internal Enumeration):
  0-15      : General Purpose Registers (RAX, RCX, ..., R15)
  16-21     : Segment Registers (CS, DS, ...)
  22-30     : Control Registers (CR0-CR8)
  31-46     : Debug Registers (DR0-DR15)
  47-49     : Special (RIP, RFLAGS, MSW)
  50-81     : XMM Registers (XMM0-XMM15)
              Stored as pairs of 64-bit values (Low, High)
              XMM0 = 50 (Low), 51 (High)
              ...
              XMM15 = 80 (Low), 81 (High)
  82-97     : FPU Stack Registers (ST0-ST7)
              Stored as pairs (Value, Metadata)
```

## SIMD Instruction Set

### Vector Arithmetic

#### VADD - Vector Addition

```assembly
VADD                        ; element-wise addition (implicit XMM regs)
```

**Description**: Adds corresponding elements from two XMM registers.

**Format**:

- Opcode: `0xD4`
- Format: `VADD` (no operands — uses XMM0/R0-R3 and XMM1/R4-R7)

**Example**:

```assembly
; Add two vectors of floats
; XMM0 = [1.0, 2.0, 3.0, 4.0]
; XMM1 = [5.0, 6.0, 7.0, 8.0]
VADD XMM0, XMM1
; Result: XMM0 = [6.0, 8.0, 10.0, 12.0]
```

**Assembly Test Example**:

```assembly
.test "vector addition" {
    .description "Test parallel addition of vector elements using VADD"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector", "addition"

    ; Set up first vector in XMM0: [1.0, 2.0, 3.0, 4.0]
    LOAD_IMM XMM0_0, 1    ; XMM0 component 0 = 1.0
    LOAD_IMM XMM0_1, 2    ; XMM0 component 1 = 2.0
    LOAD_IMM XMM0_2, 3    ; XMM0 component 2 = 3.0
    LOAD_IMM XMM0_3, 4    ; XMM0 component 3 = 4.0

    ; Set up second vector in XMM1: [5.0, 6.0, 7.0, 8.0]
    LOAD_IMM XMM1_0, 5
    LOAD_IMM XMM1_1, 6
    LOAD_IMM XMM1_2, 7
    LOAD_IMM XMM1_3, 8

    ; Perform vector addition
    VADD XMM0, XMM1

    ; Verify results: XMM0 should be [6.0, 8.0, 10.0, 12.0]
    .assert_reg XMM0_0, 6
    .assert_reg XMM0_1, 8
    .assert_reg XMM0_2, 10
    .assert_reg XMM0_3, 12
}
```

#### VMUL - Vector Multiplication

```assembly
VMUL                        ; element-wise multiplication (implicit XMM regs)
```

**Description**: Multiplies corresponding elements from two XMM registers.

**Format**:

- Opcode: `0xD5`
- Format: `VMUL` (no operands — uses XMM0/R0-R3 and XMM1/R4-R7)

**Example**:

```assembly
; Multiply two vectors
; XMM0 = [2.0, 3.0, 4.0, 5.0]
; XMM1 = [10.0, 10.0, 10.0, 10.0]
VMUL XMM0, XMM1
; Result: XMM0 = [20.0, 30.0, 40.0, 50.0]
```

**Assembly Test Example**:

```assembly
.test "vector multiplication" {
    .description "Test parallel multiplication of vector elements using VMUL"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector", "multiplication"

    ; XMM0 = [2.0, 3.0, 4.0, 5.0]
    LOAD_IMM XMM0_0, 2
    LOAD_IMM XMM0_1, 3
    LOAD_IMM XMM0_2, 4
    LOAD_IMM XMM0_3, 5

    ; XMM1 = [10.0, 10.0, 10.0, 10.0]
    LOAD_IMM XMM1_0, 10
    LOAD_IMM XMM1_1, 10
    LOAD_IMM XMM1_2, 10
    LOAD_IMM XMM1_3, 10

    VMUL XMM0, XMM1

    ; Verify: [20.0, 30.0, 40.0, 50.0]
    .assert_reg XMM0_0, 20
    .assert_reg XMM0_1, 30
    .assert_reg XMM0_2, 40
    .assert_reg XMM0_3, 50
}
```

#### VDOT - Vector Dot Product

```assembly
VDOT                        ; dot product (implicit XMM regs)
```

**Description**: Computes the dot product of two vectors and stores result in first component.

**Format**:

- Opcode: `0xD6`
- Format: `VDOT` (no operands)

**Example**:

```assembly
; Compute dot product
; XMM0 = [1.0, 2.0, 3.0, 4.0]
; XMM1 = [5.0, 6.0, 7.0, 8.0]
VDOT XMM0, XMM1
; Result: XMM0[0] = 1*5 + 2*6 + 3*7 + 4*8 = 70.0
```

**Assembly Test Example**:

```assembly
.test "vector dot product" {
    .description "Test dot product of two vectors using VDOT"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector", "dot-product"

    ; XMM0 = [1.0, 2.0, 3.0, 4.0]
    LOAD_IMM XMM0_0, 1
    LOAD_IMM XMM0_1, 2
    LOAD_IMM XMM0_2, 3
    LOAD_IMM XMM0_3, 4

    ; XMM1 = [5.0, 6.0, 7.0, 8.0]
    LOAD_IMM XMM1_0, 5
    LOAD_IMM XMM1_1, 6
    LOAD_IMM XMM1_2, 7
    LOAD_IMM XMM1_3, 8

    VDOT XMM0, XMM1

    ; Verify: 1*5 + 2*6 + 3*7 + 4*8 = 70
    .assert_reg XMM0_0, 70
}
```

### Vector Comparison

#### VCMPGT - Vector Compare Greater Than

```assembly
VCMPGT                      ; elem-wise greater-than (implicit XMM regs)
```

**Description**: Compares elements and sets result to all 1s (true) or all 0s (false).

**Format**:

- Opcode: `0xD9`
- Format: `VCMPGT` (no operands)

**Example**:

```assembly
; Compare vectors
; XMM0 = [5.0, 3.0, 8.0, 2.0]
; XMM1 = [4.0, 6.0, 7.0, 2.0]
VCMPGT XMM0, XMM1
; Result: XMM0 = [-1, 0, -1, 0] (true, false, true, false)
```

### Vector Operations

#### VMAX - Vector Maximum

```assembly
VMAX                        ; elem-wise maximum (implicit XMM regs)
```

**Description**: Stores the maximum of each pair of corresponding elements.

**Format**:

- Opcode: `0xD7`
- Format: `VMAX` (no operands)

**Example**:

```assembly
; Find element-wise maximum
; XMM0 = [1.0, 8.0, 3.0, 7.0]
; XMM1 = [5.0, 2.0, 9.0, 6.0]
VMAX XMM0, XMM1
; Result: XMM0 = [5.0, 8.0, 9.0, 7.0]
```

**Assembly Test Example**:

```assembly
.test "vector max" {
    .description "Test finding maximum element in vector using VMAX"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector", "max"

    ; XMM0 = [1.0, 8.0, 3.0, 7.0]
    LOAD_IMM XMM0_0, 1
    LOAD_IMM XMM0_1, 8
    LOAD_IMM XMM0_2, 3
    LOAD_IMM XMM0_3, 7

    ; XMM1 = [5.0, 2.0, 9.0, 6.0]
    LOAD_IMM XMM1_0, 5
    LOAD_IMM XMM1_1, 2
    LOAD_IMM XMM1_2, 9
    LOAD_IMM XMM1_3, 6

    VMAX XMM0, XMM1

    ; Verify: [5.0, 8.0, 9.0, 7.0]
    .assert_reg XMM0_0, 5
    .assert_reg XMM0_1, 8
    .assert_reg XMM0_2, 9
    .assert_reg XMM0_3, 7
}
```

#### VBROADCAST - Vector Broadcast

```assembly
VBROADCAST <dest_xmm>, <src_reg>  ; Broadcast scalar to all elements
```

**Description**: Copies a single value to all elements of the destination vector.

**Format**:

- Opcode: `0xD8`
- Format: `VBROADCAST` (no operands)

**Example**:

```assembly
; Broadcast value 42 to all elements
LOAD_IMM R0, 42
VBROADCAST XMM0, R0
; Result: XMM0 = [42, 42, 42, 42]
```

**Assembly Test Example**:

```assembly
.test "vector broadcast" {
    .description "Test broadcasting scalar to vector using VBROADCAST"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "vector", "broadcast"

    ; Load scalar value
    LOAD_IMM R0, 42

    ; Broadcast to all XMM0 components
    VBROADCAST XMM0, R0

    ; Verify all components are 42
    .assert_reg XMM0_0, 42
    .assert_reg XMM0_1, 42
    .assert_reg XMM0_2, 42
    .assert_reg XMM0_3, 42
}
```

### Data Manipulation

#### PACKB - Pack Bytes

```assembly
PACKB                       ; pack bytes with saturation
```

**Description**: Packs bytes from source and destination into destination with saturation.

**Format**:

- Opcode: `0xDA`
- Format: `PACKB` (no operands)

**Example**:

```assembly
; Pack bytes
PACKB XMM0, XMM1
```

**Assembly Test Example**:

```assembly
.test "packed byte operations" {
    .description "Test packing and unpacking bytes using PACKB and UNPACKB"
    .author "DemiEngine Team"
    .category "SIMD"
    .tag "packed", "bytes"

    ; Set up source data
    LOAD_IMM XMM0_0, 0x12345678
    LOAD_IMM XMM1_0, 0x9ABCDEF0

    ; Pack bytes
    PACKB XMM0, XMM1

    ; Unpack bytes
    UNPACKB XMM2, XMM0

    ; Verify operation
    .assert_reg XMM2_0, 0x12345678
}
```

#### UNPACKB - Unpack Bytes

```assembly
UNPACKB                     ; unpack bytes
```

**Description**: Unpacks bytes from source register.

**Format**:

- Opcode: `0xDB`
- Format: `UNPACKB` (no operands)

**Example**:

```assembly
; Unpack bytes
UNPACKB XMM2, XMM0
```

## Complete SIMD Opcode List

**Note**: The SIMD opcodes are currently mapped to the 0xD4-0xDB range in the implementation.

| Opcode | Mnemonic   | Description                          |
| ------ | ---------- | ------------------------------------ |
| 0xD4   | VADD       | Vector addition (element-wise)       |
| 0xD5   | VMUL       | Vector multiplication (element-wise) |
| 0xD6   | VDOT       | Vector dot product                   |
| 0xD7   | VMAX       | Vector maximum (element-wise)        |
| 0xD8   | VBROADCAST | Broadcast scalar to vector           |
| 0xD9   | VCMPGT     | Vector compare greater than          |
| 0xDA   | PACKB      | Pack bytes with saturation           |
| 0xDB   | UNPACKB    | Unpack bytes                         |

## Register Access Patterns

### Direct Component Access

```assembly
; Access individual XMM components
LOAD_IMM XMM0_0, 10    ; XMM0 component 0
LOAD_IMM XMM0_1, 20    ; XMM0 component 1
LOAD_IMM XMM0_2, 30    ; XMM0 component 2
LOAD_IMM XMM0_3, 40    ; XMM0 component 3
```

### Register Index Mapping

```
XMM0: R96-R99     (components 0-3)
XMM1: R100-R103   (components 0-3)
XMM2: R104-R107   (components 0-3)
...
XMM15: R156-R159  (components 0-3)
```

## Programming Examples

### Vector Scaling

```assembly
; Scale a vector by a constant
; vector = [1, 2, 3, 4] * 10

; Load vector
LOAD_IMM XMM0_0, 1
LOAD_IMM XMM0_1, 2
LOAD_IMM XMM0_2, 3
LOAD_IMM XMM0_3, 4

; Create scaling vector
LOAD_IMM R0, 10
VBROADCAST XMM1, R0

; Multiply
VMUL XMM0, XMM1
; Result: XMM0 = [10, 20, 30, 40]
```

### Vector Normalization (Simplified)

```assembly
; Normalize a 4D vector
; 1. Compute dot product (magnitude squared)
; 2. Take square root (requires FPU)
; 3. Broadcast reciprocal
; 4. Multiply vector by reciprocal

; Load vector [3, 4, 0, 0]
LOAD_IMM XMM0_0, 3
LOAD_IMM XMM0_1, 4
LOAD_IMM XMM0_2, 0
LOAD_IMM XMM0_3, 0

; Copy for dot product
MOV XMM1_0, XMM0_0
MOV XMM1_1, XMM0_1
MOV XMM1_2, XMM0_2
MOV XMM1_3, XMM0_3

; Compute magnitude squared
VDOT XMM0, XMM1
; Result: XMM0[0] = 9 + 16 = 25

; (Normalization would continue with FPU sqrt and division)
```

### Matrix-Vector Multiplication (4x4 × 4)

```assembly
; Multiply 4x4 matrix by 4D vector
; result[i] = dot(matrix_row[i], vector)

; Load vector into XMM0
LOAD_IMM XMM0_0, 1
LOAD_IMM XMM0_1, 2
LOAD_IMM XMM0_2, 3
LOAD_IMM XMM0_3, 4

; Process each row
; Row 0
LOAD_IMM XMM1_0, m00
LOAD_IMM XMM1_1, m01
LOAD_IMM XMM1_2, m02
LOAD_IMM XMM1_3, m03
VDOT XMM1, XMM0
; XMM1[0] now contains result[0]

; Row 1
LOAD_IMM XMM2_0, m10
LOAD_IMM XMM2_1, m11
LOAD_IMM XMM2_2, m12
LOAD_IMM XMM2_3, m13
VDOT XMM2, XMM0
; XMM2[0] now contains result[1]

; (Continue for rows 2 and 3)
```

### Parallel Comparison

```assembly
; Find which elements are greater than threshold

; Load data
LOAD_IMM XMM0_0, 5
LOAD_IMM XMM0_1, 3
LOAD_IMM XMM0_2, 8
LOAD_IMM XMM0_3, 2

; Load threshold
LOAD_IMM R0, 4
VBROADCAST XMM1, R0

; Compare
VCMPGT XMM0, XMM1
; Result: XMM0 = [-1, 0, -1, 0]
; (true for elements > 4)
```

## Performance Considerations

### Benefits of SIMD

1. **Parallelism**: Process 4 elements simultaneously
2. **Memory Efficiency**: Single instruction for multiple data
3. **Cache Friendly**: Contiguous data access patterns
4. **Throughput**: Higher operations per cycle

### Best Practices

1. **Align Data**: Ensure data is properly aligned for best performance
2. **Batch Operations**: Process data in chunks of 4 (XMM width)
3. **Minimize Transfers**: Keep data in XMM registers when possible
4. **Use Broadcasts**: Efficient for scalar-vector operations

### Common Use Cases

- Graphics processing (colors, vertices, normals)
- Physics simulations (particles, forces)
- Signal processing (filters, transforms)
- Machine learning (matrix operations)
- Audio processing (samples, mixing)
- Image processing (pixels, filters)

## Test Coverage

DemiEngine includes comprehensive SIMD testing:

- 6 SIMD tests in `tests/simd.test.asm`
- All vector operations tested
- Component-wise operations verified
- Dot product validated
- Broadcasting tested
- Packing/unpacking verified
- 100% SIMD instruction coverage

See `tests/simd.test.asm` for complete examples and test cases.

## Integration with FPU

SIMD operations can be combined with FPU operations for advanced computations:

```assembly
; Compute vector magnitude using SIMD + FPU
; magnitude = sqrt(x² + y² + z² + w²)

; Vector in XMM0
LOAD_IMM XMM0_0, 3
LOAD_IMM XMM0_1, 4
LOAD_IMM XMM0_2, 0
LOAD_IMM XMM0_3, 0

; Dot product with itself
MOV XMM1_0, XMM0_0
MOV XMM1_1, XMM0_1
MOV XMM1_2, XMM0_2
MOV XMM1_3, XMM0_3
VDOT XMM0, XMM1

; Transfer to FPU for sqrt
STORE XMM0_0, temp_addr
FLD 0x01 temp_addr
FSQRT
FST 0x01 result_addr
```

## Future SIMD Extensions

Planned enhancements:

- More vector sizes (256-bit, 512-bit)
- Additional comparison operations
- Shuffle and permute instructions
- Horizontal operations
- Conversion operations (int ↔ float)
- Masked operations
