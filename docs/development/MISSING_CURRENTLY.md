# Demi Implementation Roadmap

**Last Updated:** December 2025  
**Purpose:** Complete reference for all missing features, opcodes, and improvements needed in the Demi project.

---

## Table of Contents

1. [Priority Legend](#priority-legend)
2. [Opcodes - Missing Implementation](#opcodes---missing-implementation)
   - [Extended 64-bit Operations](#extended-64-bit-operations)
   - [Advanced SIMD/SSE Operations](#advanced-simdsse-operations)
   - [AVX Operations](#avx-operations)
   - [MMX Operations](#mmx-operations)
3. [SIMD Enhancements - Current Implementation Gaps](#simd-enhancements---current-implementation-gaps)
4. [FPU Fixes - Documentation and Implementation Consistency](#fpu-fixes---documentation-and-implementation-consistency)
5. [Development Tools](#development-tools)
6. [Testing Infrastructure](#testing-infrastructure)
7. [Performance Optimizations](#performance-optimizations)
8. [Code Quality and Maintenance](#code-quality-and-maintenance)
9. [Documentation Updates](#documentation-updates)

---

## Priority Legend

| Priority | Label | Description | Timeline |
|----------|-------|-------------|----------|
| **P0 Critical** | 🔴 Red Circle | Blocks other work, must be done first | Immediate |
| **P1 High** | 🟠 Orange Circle | Core functionality, high impact | 1-2 weeks |
| **P2 Medium** | 🟡 Yellow Circle | Important but not blocking | 2-4 weeks |
| **P3 Low** | 🟢 Green Circle | Nice to have, polish | When time permits |
| **P4 Backlog** | 🔵 Blue Circle | Future consideration | No timeline |

---

## Opcodes - Missing Implementation

### Extended 64-bit Operations (18 opcodes)

**Priority:** 🟡 P2 Medium  
**Rationale:** Completes register architecture, enables large program support  
**Timeline:** 2-3 weeks  
**Location:** `src/engine/opcodes/`

#### Memory Operations

##### Opcode 0x50 - LOADEX (Load Extended)
- **Description:** Load 64-bit value from memory with extended addressing
- **Operands:** reg64, addr64
- **Behavior:** `reg64 = memory[addr64]`
- **Instruction Size:** 10 bytes (opcode + reg + 8-byte address)

##### Opcode 0x51 - STOREEX (Store Extended)
- **Description:** Store 64-bit value to memory with extended addressing
- **Operands:** addr64, reg64
- **Behavior:** `memory[addr64] = reg64`
- **Instruction Size:** 10 bytes

##### Opcode 0x52 - LOAD_IMMEX (Load Immediate Extended)
- **Description:** Load 64-bit immediate to extended register
- **Operands:** reg64, imm64
- **Behavior:** `reg64 = imm64`
- **Instruction Size:** 10 bytes (opcode + reg + 8-byte immediate)

#### Control Flow Operations

##### Opcode 0x53 - JMPEX (Jump Extended)
- **Description:** Unconditional jump with 64-bit address
- **Operands:** addr64
- **Behavior:** `PC = addr64`
- **Instruction Size:** 9 bytes (opcode + 8-byte address)

##### Opcode 0x54 - CALLEX (Call Extended)
- **Description:** Call subroutine with 64-bit address
- **Operands:** addr64
- **Behavior:** `push(PC+9); PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x55 - JZEX (Jump if Zero Extended)
- **Description:** Jump to 64-bit address if zero flag set
- **Operands:** addr64
- **Behavior:** `if (ZF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x56 - JNZEX (Jump if Not Zero Extended)
- **Description:** Jump to 64-bit address if zero flag not set
- **Operands:** addr64
- **Behavior:** `if (!ZF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x57 - JGEX (Jump if Greater Extended)
- **Description:** Jump to 64-bit address if greater (signed)
- **Operands:** addr64
- **Behavior:** `if (!ZF && SF==OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x58 - JLEX (Jump if Less Extended)
- **Description:** Jump to 64-bit address if less (signed)
- **Operands:** addr64
- **Behavior:** `if (SF!=OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x59 - JGEEX (Jump if Greater or Equal Extended)
- **Description:** Jump to 64-bit address if greater than or equal (signed)
- **Operands:** addr64
- **Behavior:** ` if (SF==OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5A - JLEEX (Jump if Less or Equal Extended)
- **Description:** Jump to 64-bit address if less than or equal (signed)
- **Operands:** addr64
- **Behavior:** `if (ZF || SF!=OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5B - JCEX (Jump if Carry Extended)
- **Description:** Jump to 64-bit address if carry flag set
- **Operands:** addr64
- **Behavior:** ` if (CF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5C - JNCEX (Jump if No Carry Extended)
- **Description:** Jump to 64-bit address if carry flag not set
- **Operands:**  addr64
- **Behavior:** `if (! CF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5D - JOEX (Jump if Overflow Extended)
- **Description:** Jump to 64-bit address if overflow flag set
- **Operands:** addr64
- **Behavior:** ` if (OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5E - JNOEX (Jump if No Overflow Extended)
- **Description:** Jump to 64-bit address if overflow flag not set
- **Operands:** addr64
- **Behavior:** `if (!OF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x5F - JSEX (Jump if Sign Extended)
- **Description:** Jump to 64-bit address if sign flag set
- **Operands:** addr64
- **Behavior:** `if (SF) PC = addr64`
- **Instruction Size:** 9 bytes

##### Opcode 0x60 - JNSEX (Jump if No Sign Extended)
- **Description:** Jump to 64-bit address if sign flag not set
- **Operands:** addr64
- **Behavior:** `if (!SF) PC = addr64`
- **Instruction Size:** 9 bytes

Comparison Operations:

##### Opcode 0x61 - CMPEX (Compare Extended)
- **Description:**  Compare two 64-bit extended registers
- **Operands:** reg64_1, reg64_2
- **Behavior:** `flags = reg64_1 - reg64_2 (sets ZF, SF, CF, OF without storing result)`
- **Instruction Size:** 3 bytes (opcode + 2 register bytes)

Implementation Example for JZEX: 

void handle_jzex(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 8 >= program.size()) {
        DebugHandler::error(ErrorCode::INSUFFICIENT_BYTES, "JZEX requires 9 bytes");
        running = false;
        return;
    }
    
    // Read 64-bit address in little-endian format
    uint64_t target = 0;
    for (int i = 0; i < 8; i++) {
        target |= static_cast<uint64_t>(program[pc + 1 + i]) << (i * 8);
    }
    
    if (cpu.get_flags() & FLAG_ZERO) {
        cpu.set_pc(static_cast<uint32_t>(target));
    } else {
        cpu.set_pc(pc + 9);
    }
}

---

### Advanced SIMD/SSE Operations (18 opcodes)

- **Priority:** P1 High
- **Rationale:**  Biggest performance impact, enables vectorized computation for graphics, audio, ML
- **Timeline:** 4-6 weeks
- **Location:** `src/engine/opcodes/`

Packed Single-Precision Operations (128-bit, 4x float32):

##### Opcode 0xC0 - MOVPS (Move Packed Single)
- **Description:**  Move 128-bit aligned packed floats
- **Operands:** xmm_dst, xmm_src or mem128
- **Behavior:** `xmm_dst = src (must be 16-byte aligned)`
- **Instruction Size:** 3 bytes (opcode + dst + src)

##### Opcode 0xC1 - MOVUPS (Move Unaligned Packed Single)
- **Description:** Move 128-bit unaligned packed floats
- **Operands:** xmm_dst, xmm_src or mem128
- **Behavior:** `xmm_dst = src (no alignment requirement)`
- **Instruction Size:** 3 bytes

##### Opcode 0xC2 - MOVAPS (Move Aligned Packed Single)
- **Description:** Move 128-bit aligned packed floats with fault on misalignment
- **Operands:**  xmm_dst, xmm_src or mem128
- **Behavior:** `xmm_dst = src (generates GP fault if not 16-byte aligned)`
- **Instruction Size:** 3 bytes

##### Opcode 0xC3 - ADDPS (Add Packed Single)
- **Description:** Add 4 packed single-precision floats element-wise
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = xmm_dst[i] + xmm_src[i] for i = 0 to 3`
- **Instruction Size:**  3 bytes

##### Opcode 0xC4 - SUBPS (Subtract Packed Single)
- **Description:** Subtract 4 packed single-precision floats element-wise
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = xmm_dst[i] - xmm_src[i] for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xC5 - MULPS (Multiply Packed Single)
- **Description:**  Multiply 4 packed single-precision floats element-wise
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = xmm_dst[i] * xmm_src[i] for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xC6 - DIVPS (Divide Packed Single)
- **Description:** Divide 4 packed single-precision floats element-wise
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = xmm_dst[i] / xmm_src[i] for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xC7 - MAXPS (Maximum Packed Single)
- **Description:** Maximum of 4 packed single-precision floats element-wise
- **Operands:** xmm_dst, xmm_src
- **Behavior:** ` xmm_dst[i] = max(xmm_dst[i], xmm_src[i]) for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xC8 - MINPS (Minimum Packed Single)
- **Description:** Minimum of 4 packed single-precision floats element-wise
- **Operands:**  xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = min(xmm_dst[i], xmm_src[i]) for i = 0 to 3`
- **Instruction Size:**  3 bytes

##### Opcode 0xC9 - SQRTPS (Square Root Packed Single)
- **Description:** Square root of 4 packed single-precision floats
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] = sqrt(xmm_src[i]) for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xCA - RCPPS (Reciprocal Packed Single)
- **Description:** Approximate reciprocal of 4 packed floats
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst[i] is approximately 1.0 / xmm_src[i] for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0xCB - RSQRTPS (Reciprocal Square Root Packed Single)
- **Description:** Approximate reciprocal square root of 4 packed floats
- **Operands:** xmm_dst, xmm_src
- **Behavior:** ` xmm_dst[i] is approximately 1.0 / sqrt(xmm_src[i]) for i = 0 to 3`
- **Instruction Size:**  3 bytes

##### Opcode 0xCC - ANDPS (Bitwise AND Packed Single)
- **Description:** Bitwise AND of 128-bit packed values
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst = xmm_dst AND xmm_src (bitwise)`
- **Instruction Size:** 3 bytes

##### Opcode 0xCD - ORPS (Bitwise OR Packed Single)
- **Description:** Bitwise OR of 128-bit packed values
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst = xmm_dst OR xmm_src (bitwise)`
- **Instruction Size:** 3 bytes

##### Opcode 0xCE - XORPS (Bitwise XOR Packed Single)
- **Description:** Bitwise XOR of 128-bit packed values
- **Operands:** xmm_dst, xmm_src
- **Behavior:** `xmm_dst = xmm_dst XOR xmm_src (bitwise)`
- **Instruction Size:** 3 bytes

Scalar and Additional Operations:

##### Opcode 0xCF - MOVSS (Move Scalar Single)
- **Description:** Move lowest 32-bit float only
- **Operands:** xmm_dst, xmm_src or mem32
- **Behavior:** `xmm_dst[0] = src[0] (upper bits unchanged when src is xmm, zeroed when src is memory)`
- **Instruction Size:** 3 bytes

##### Opcode 0xD9 - CMPPS (Compare Packed Single)
- **Description:**  Compare 4 packed floats with predicate
- **Operands:** xmm_dst, xmm_src, imm8
- **Behavior:** `xmm_dst[i] = (xmm_dst[i] op xmm_src[i]) ? 0xFFFFFFFF : 0x00000000`
Predicate values in imm8: 0=EQ, 1=LT, 2=LE, 3=UNORD, 4=NEQ, 5=NLT, 6=NLE, 7=ORD
- **Instruction Size:** 4 bytes

##### Opcode 0xDA - SHUFPS (Shuffle Packed Single)
- **Description:** Shuffle single-precision floats based on immediate selector
- **Operands:** xmm_dst, xmm_src, imm8
- **Behavior:** ` Rearranges elements from both operands based on 2-bit selectors in imm8`
- **Instruction Size:**  4 bytes

XMM Register Structure for Implementation:

struct XMMRegister {
    union {
        float f32[4];           // 4x 32-bit single-precision floats
        double f64[2];          // 2x 64-bit double-precision floats
        int32_t i32[4];         // 4x 32-bit signed integers
        uint32_t u32[4];        // 4x 32-bit unsigned integers
        int64_t i64[2];         // 2x 64-bit signed integers
        uint64_t u64[2];        // 2x 64-bit unsigned integers
        int8_t i8[16];          // 16x 8-bit signed integers
        uint8_t u8[16];         // 16x 8-bit unsigned integers
        int16_t i16[8];         // 8x 16-bit signed integers
        uint16_t u16[8];        // 8x 16-bit unsigned integers
    };
};

Implementation Example for ADDPS:

void handle_ADDPS(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    uint32_t pc = cpu.get_pc();
    
    if (pc + 2 >= program.size()) {
        DebugHandler::error(ErrorCode::INSUFFICIENT_BYTES, "ADDPS requires 3 bytes");
        running = false;
        return;
    }
    
    uint8_t dst_reg = program[pc + 1];
    uint8_t src_reg = program[pc + 2];
    
    XMMRegister& dst = cpu.get_xmm_register(dst_reg);
    const XMMRegister& src = cpu.get_xmm_register(src_reg);
    
    for (int i = 0; i < 4; i++) {
        dst.f32[i] += src.f32[i];
    }
    
    DEBUG_TRACE(DebugCategory::SIMD_EXECUTION, 
        "ADDPS XMM{}, XMM{}:  [{}, {}, {}, {}]",
        dst_reg, src_reg, dst.f32[0], dst.f32[1], dst.f32[2], dst.f32[3]);
    
    cpu.set_pc(pc + 3);
}

---

### AVX Operations (20 opcodes)

- **Priority:** P3 Low
- **Rationale:** Advanced optimization for modern CPUs, 256-bit vectors (8x float32)
- **Timeline:** 3-4 weeks (after SIMD/SSE complete)
- **Location:** `src/engine/opcodes/`

256-bit Packed Operations:

##### Opcode 0xE0 - VMOVPS (Move Packed Single 256-bit)
- **Description:** Move 256-bit packed single-precision floats
- **Operands:** ymm_dst, ymm_src or mem256
- **Behavior:** `ymm_dst = src`
- **Instruction Size:** 4 bytes (VEX prefix + opcode + modrm)

##### Opcode 0xE1 - VMOVUPS (Move Unaligned Packed Single 256-bit)
- **Description:** Move 256-bit unaligned packed floats
- **Operands:** ymm_dst, ymm_src or mem256
- **Behavior:** `ymm_dst = src (no alignment requirement)`
- **Instruction Size:** 4 bytes

##### Opcode 0xE2 - VMOVAPS (Move Aligned Packed Single 256-bit)
- **Description:** Move 256-bit aligned packed floats
- **Operands:** ymm_dst, ymm_src or mem256
- **Behavior:** `ymm_dst = src (must be 32-byte aligned)`
- **Instruction Size:** 4 bytes

##### Opcode 0xE3 - VADDPS (Add Packed Single 256-bit)
- **Description:** Add 8 packed single-precision floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = ymm_src1[i] + ymm_src2[i] for i = 0 to 7`
- **Instruction Size:** 5 bytes (VEX prefix + opcode + modrm + src2)

##### Opcode 0xE4 - VSUBPS (Subtract Packed Single 256-bit)
- **Description:** Subtract 8 packed single-precision floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = ymm_src1[i] - ymm_src2[i] for i = 0 to 7`
- **Instruction Size:** 5 bytes

##### Opcode 0xE5 - VMULPS (Multiply Packed Single 256-bit)
- **Description:**  Multiply 8 packed single-precision floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = ymm_src1[i] * ymm_src2[i] for i = 0 to 7`
- **Instruction Size:** 5 bytes

##### Opcode 0xE6 - VDIVPS (Divide Packed Single 256-bit)
- **Description:** Divide 8 packed single-precision floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = ymm_src1[i] / ymm_src2[i] for i = 0 to 7`
- **Instruction Size:** 5 bytes

##### Opcode 0xE7 - VMAXPS (Maximum Packed Single 256-bit)
- **Description:** Element-wise maximum of 8 packed floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = max(ymm_src1[i], ymm_src2[i]) for i = 0 to 7`
- **Instruction Size:** 5 bytes

##### Opcode 0xE8 - VMINPS (Minimum Packed Single 256-bit)
- **Description:** Element-wise minimum of 8 packed floats
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst[i] = min(ymm_src1[i], ymm_src2[i]) for i = 0 to 7`
- **Instruction Size:** 5 bytes

##### Opcode 0xE9 - VSQRTPS (Square Root Packed Single 256-bit)
- **Description:** Square root of 8 packed single-precision floats
- **Operands:**  ymm_dst, ymm_src
- **Behavior:** `ymm_dst[i] = sqrt(ymm_src[i]) for i = 0 to 7`
- **Instruction Size:** 4 bytes

##### Opcode 0xEA - VANDPS (Bitwise AND 256-bit)
- **Description:** Bitwise AND of 256-bit packed values
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst = ymm_src1 AND ymm_src2 (bitwise)`
- **Instruction Size:** 5 bytes

##### Opcode 0xEB - VORPS (Bitwise OR 256-bit)
- **Description:** Bitwise OR of 256-bit packed values
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** `ymm_dst = ymm_src1 OR ymm_src2 (bitwise)`
- **Instruction Size:** 5 bytes

##### Opcode 0xEC - VXORPS (Bitwise XOR 256-bit)
- **Description:** Bitwise XOR of 256-bit packed values
- **Operands:** ymm_dst, ymm_src1, ymm_src2
- **Behavior:** ` ymm_dst = ymm_src1 XOR ymm_src2 (bitwise)`
- **Instruction Size:** 5 bytes

Scalar AVX Operations:

##### Opcode 0xED - VMOVSS (Move Scalar Single AVX)
- **Description:** Move scalar single with AVX encoding (zeroes upper bits)
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = xmm_src2[0], upper bits from xmm_src1 or zeroed`
- **Instruction Size:** 5 bytes

##### Opcode 0xEE - VADDSS (Add Scalar Single AVX)
- **Description:** Add single float with AVX three-operand form
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = xmm_src1[0] + xmm_src2[0], upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

##### Opcode 0xEF - VSUBSS (Subtract Scalar Single AVX)
- **Description:** Subtract single float with AVX three-operand form
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = xmm_src1[0] - xmm_src2[0], upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

##### Opcode 0xF0 - VMULSS (Multiply Scalar Single AVX)
- **Description:** Multiply single float with AVX three-operand form
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = xmm_src1[0] * xmm_src2[0], upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

##### Opcode 0xF1 - VDIVSS (Divide Scalar Single AVX)
- **Description:** Divide single float with AVX three-operand form
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = xmm_src1[0] / xmm_src2[0], upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

##### Opcode 0xF2 - VMAXSS (Maximum Scalar Single AVX)
- **Description:** Maximum of two single floats
- **Operands:** xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = max(xmm_src1[0], xmm_src2[0]), upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

##### Opcode 0xF3 - VMINSS (Minimum Scalar Single AVX)
- **Description:** Minimum of two single floats
- **Operands:**  xmm_dst, xmm_src1, xmm_src2
- **Behavior:** `xmm_dst[0] = min(xmm_src1[0], xmm_src2[0]), upper bits from xmm_src1`
- **Instruction Size:** 5 bytes

YMM Register Structure for Implementation: 

struct YMMRegister {
    union {
        float f32[8];           // 8x 32-bit single-precision floats
        double f64[4];          // 4x 64-bit double-precision floats
        int32_t i32[8];         // 8x 32-bit signed integers
        uint32_t u32[8];        // 8x 32-bit unsigned integers
        int64_t i64[4];         // 4x 64-bit signed integers
        uint64_t u64[4];        // 4x 64-bit unsigned integers
        XMMRegister xmm[2];     // 2x XMM registers (low 128-bit and high 128-bit)
    };
};

---

### MMX Operations (11 opcodes)

- **Priority:** P4 Backlog
- **Rationale:** Legacy support for integer SIMD, 64-bit packed operations
- **Timeline:** 1-2 weeks (when needed)
- **Location:** `src/engine/opcodes/`

##### Opcode 0x70 - MOVQ (Move Quadword)
- **Description:** Move 64-bit value between MMX registers or memory
- **Operands:** mm_dst, mm_src or mem64
- **Behavior:** `mm_dst = src (64-bit move)`
- **Instruction Size:** 3 bytes

##### Opcode 0x71 - PADDB (Packed Add Bytes)
- **Description:** Add 8 packed bytes with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] + mm_src[i] for i = 0 to 7 (8-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x72 - PADDW (Packed Add Words)
- **Description:** Add 4 packed 16-bit words with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] + mm_src[i] for i = 0 to 3 (16-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x73 - PADDD (Packed Add Doublewords)
- **Description:** Add 2 packed 32-bit doublewords with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] + mm_src[i] for i = 0 to 1 (32-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x74 - PSUBB (Packed Subtract Bytes)
- **Description:** Subtract 8 packed bytes with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] - mm_src[i] for i = 0 to 7 (8-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x75 - PSUBW (Packed Subtract Words)
- **Description:** Subtract 4 packed 16-bit words with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] - mm_src[i] for i = 0 to 3 (16-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x76 - PSUBD (Packed Subtract Doublewords)
- **Description:** Subtract 2 packed 32-bit doublewords with wraparound
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = mm_dst[i] - mm_src[i] for i = 0 to 1 (32-bit elements)`
- **Instruction Size:** 3 bytes

##### Opcode 0x77 - PMULLW (Packed Multiply Low Words)
- **Description:** Multiply 4 packed words and store low 16 bits of each result
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst[i] = (mm_dst[i] * mm_src[i]) AND 0xFFFF for i = 0 to 3`
- **Instruction Size:** 3 bytes

##### Opcode 0x78 - PAND (Packed Bitwise AND)
- **Description:** Bitwise AND of 64-bit MMX registers
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst = mm_dst AND mm_src (bitwise, 64-bit)`
- **Instruction Size:** 3 bytes

##### Opcode 0x79 - POR (Packed Bitwise OR)
- **Description:** Bitwise OR of 64-bit MMX registers
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst = mm_dst OR mm_src (bitwise, 64-bit)`
- **Instruction Size:** 3 bytes

##### Opcode 0x7A - PXOR (Packed Bitwise XOR)
- **Description:**  Bitwise XOR of 64-bit MMX registers
- **Operands:** mm_dst, mm_src
- **Behavior:** `mm_dst = mm_dst XOR mm_src (bitwise, 64-bit)`
- **Instruction Size:** 3 bytes

MM Register Structure for Implementation:

struct MMRegister {
    union {
        int8_t i8[8];           // 8x 8-bit signed bytes
        int16_t i16[4];         // 4x 16-bit signed words
        int32_t i32[2];         // 2x 32-bit signed doublewords
        int64_t i64;            // 1x 64-bit signed quadword
        uint8_t u8[8];          // 8x 8-bit unsigned bytes
        uint16_t u16[4];        // 4x 16-bit unsigned words
        uint32_t u32[2];        // 2x 32-bit unsigned doublewords
        uint64_t u64;           // 1x 64-bit unsigned quadword
    };
};

---

## SIMD Enhancements - Current Implementation Gaps

- **Priority:** P1 High
- **Rationale:** Current basic SIMD (8 opcodes) is too limited for real-world use
- **Location:** `src/engine/opcodes/v*. cpp`

The current SIMD implementation only supports:  VADD, VMUL, VDOT, VMAX, VBROADCAST, VCMPGT, PACKB, UNPACKB

Missing Arithmetic Operations: 

VMIN - Find minimum element in vector - Priority P1
- **Description:** Element-wise minimum or horizontal minimum
- **Behavior:** `For element-wise:  dst[i] = min(src1[i], src2[i]). For horizontal: dst = min(src[0.. 3])`

VSUB - Vector subtraction - Priority P1
- **Description:**  Subtract vectors element-wise
- **Behavior:** `dst[i] = dst[i] - src[i] for i = 0 to 3`

VDIV - Vector division - Priority P2
- **Description:** Divide vectors element-wise
- **Behavior:** `dst[i] = dst[i] / src[i] for i = 0 to 3`

VSQRT - Vector square root - Priority P2
- **Description:** Square root of each vector element
- **Behavior:** `dst[i] = sqrt(src[i]) for i = 0 to 3`

VABS - Vector absolute value - Priority P2
- **Description:** Absolute value of each vector element
- **Behavior:** `dst[i] = abs(src[i]) for i = 0 to 3`

VNEG - Vector negation - Priority P2
- **Description:** Negate each vector element
- **Behavior:** `dst[i] = -src[i] for i = 0 to 3`

Missing Bitwise Operations:

VAND - Vector bitwise AND - Priority P1
- **Description:** Bitwise AND of two vectors
- **Behavior:** `dst = dst AND src (128-bit bitwise)`

VOR - Vector bitwise OR - Priority P1
- **Description:** Bitwise OR of two vectors
- **Behavior:** ` dst = dst OR src (128-bit bitwise)`

VXOR - Vector bitwise XOR - Priority P1
- **Description:** Bitwise XOR of two vectors
- **Behavior:** `dst = dst XOR src (128-bit bitwise)`

VNOT - Vector bitwise NOT - Priority P2
- **Description:** Bitwise NOT of vector
- **Behavior:** `dst = NOT src (128-bit bitwise complement)`

Missing Shift Operations:

VSHL - Vector shift left - Priority P2
- **Description:** Shift each element left by specified amount
- **Behavior:** `dst[i] = src[i] << shift_amount for i = 0 to 3`

VSHR - Vector shift right (logical) - Priority P2
- **Description:**  Shift each element right (zero fill) by specified amount
- **Behavior:** ` dst[i] = src[i] >> shift_amount (logical) for i = 0 to 3`

VSAR - Vector shift right (arithmetic) - Priority P3
- **Description:** Shift each element right (sign extend) by specified amount
- **Behavior:** `dst[i] = src[i] >> shift_amount (arithmetic) for i = 0 to 3`

Missing Comparison Operations (currently only VCMPGT exists):

VCMPEQ - Vector compare equal - Priority P1
- **Description:**  Compare elements for equality
- **Behavior:** `dst[i] = (src1[i] == src2[i]) ? 0xFFFFFFFF : 0x00000000`

VCMPLT - Vector compare less than - Priority P1
- **Description:** Compare elements for less than
- **Behavior:** `dst[i] = (src1[i] < src2[i]) ? 0xFFFFFFFF :  0x00000000`

VCMPLE - Vector compare less or equal - Priority P2
- **Description:** Compare elements for less than or equal
- **Behavior:** `dst[i] = (src1[i] <= src2[i]) ? 0xFFFFFFFF :  0x00000000`

VCMPGE - Vector compare greater or equal - Priority P2
- **Description:** Compare elements for greater than or equal
- **Behavior:** `dst[i] = (src1[i] >= src2[i]) ? 0xFFFFFFFF :  0x00000000`

VCMPNE - Vector compare not equal - Priority P2
- **Description:** Compare elements for inequality
- **Behavior:** `dst[i] = (src1[i] != src2[i]) ? 0xFFFFFFFF : 0x00000000`

Missing Reduction Operations:

VSUM - Horizontal sum - Priority P2
- **Description:** Sum all elements of vector into scalar
- **Behavior:** `dst = src[0] + src[1] + src[2] + src[3]`

VHMIN - Horizontal minimum - Priority P3
- **Description:** Find minimum across all vector elements
- **Behavior:** `dst = min(src[0], src[1], src[2], src[3])`

VHMAX - Horizontal maximum - Priority P3
- **Description:** Find maximum across all vector elements
- **Behavior:** `dst = max(src[0], src[1], src[2], src[3])`

VHAVG - Horizontal average - Priority P4
- **Description:** Average of all vector elements
- **Behavior:** `dst = (src[0] + src[1] + src[2] + src[3]) / 4`

Missing Shuffle and Permute Operations:

VSHUFFLE - Shuffle elements within vector - Priority P3
- **Description:** Rearrange elements within single vector based on immediate selector
- **Behavior:** `dst[i] = src[selector[i]] where selector is encoded in immediate byte`

VPERMUTE - Permute elements across vectors - Priority P3
- **Description:** Select elements from two vectors based on selector
- **Behavior:** `dst[i] = (selector[i] < 4) ? src1[selector[i]] : src2[selector[i] - 4]`

VBLEND - Blend vectors conditionally - Priority P3
- **Description:** Select elements from two vectors based on mask
- **Behavior:** `dst[i] = mask[i] ? src2[i] : src1[i]`

VINSERT - Insert element into vector - Priority P3
- **Description:** Insert scalar value at specified position
- **Behavior:** `dst = src; dst[position] = scalar_value`

VEXTRACT - Extract element from vector - Priority P3
- **Description:** Extract single element to scalar register
- **Behavior:** `scalar_dst = src[position]`

Missing Data Type Support:

Float support in basic SIMD - Priority P1
- **Description:** Current implementation only supports 32-bit integers
- **Required:** Add float32 support to VADD, VMUL, VDOT, VMAX, VMIN, etc. 
- **Implementation:** Use union in vector register, add opcode variants or mode flag

8-bit and 16-bit element support - Priority P3
- **Description:** Support for byte and word packed operations
- **Required:**  VADDB (add bytes), VADDW (add words), etc.
- **Implementation:** Add element size specifier to opcodes

---

## FPU Fixes - Documentation and Implementation Consistency

- **Priority:** P2 Medium
- **Rationale:** Current documentation has conflicting opcode assignments that cause confusion
- **Location:** `src/engine/opcodes/f*.cpp and docs/reference/FPU_REFERENCE.md`

Opcode Assignment Conflicts to Resolve: 

FSIN is assigned both 0xAA and 0xB1 in different documentation files
Resolution: Audit implementation in fsin.cpp, pick correct value, update all docs

FCOS is assigned both 0xAB and 0xB2 in different documentation files
Resolution: Audit implementation in fcos.cpp, pick correct value, update all docs

FTAN is assigned both 0xAC and 0xB3 in different documentation files
Resolution: Audit implementation in ftan.cpp, pick correct value, update all docs

FSQRT is assigned both 0xAD and 0xB0 in different documentation files
Resolution:  Audit implementation in fsqrt. cpp, pick correct value, update all docs

FABS at 0xAE needs verification
Resolution: Verify implementation exists and matches documented behavior

FCHS at 0xAF needs verification
Resolution: Verify implementation exists and matches documented behavior

Missing FPU Constant Loaders: 

##### Opcode 0xB4 - FLDZ (Load Zero)
- **Description:** Push +0.0 onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(0.0)`
- **Instruction Size:** 1 byte
- **Priority:** P2

##### Opcode 0xB5 - FLD1 (Load One)
- **Description:** Push +1.0 onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(1.0)`
- **Instruction Size:** 1 byte
- **Priority:** P2

##### Opcode 0xB6 - FLDPI (Load Pi)
- **Description:** Push pi (3.14159265358979323846.. .) onto FPU stack
- **Operands:** None
- **Behavior:** ` FPU stack push(M_PI)`
- **Instruction Size:** 1 byte
- **Priority:** P2

##### Opcode 0xB7 - FLDL2E (Load Log2(e))
- **Description:** Push log base 2 of e (1.4426950408889634...) onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(M_LOG2E)`
- **Instruction Size:** 1 byte
- **Priority:** P3

##### Opcode 0xB8 - FLDL2T (Load Log2(10))
- **Description:** Push log base 2 of 10 (3.3219280948873623...) onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(log2(10))`
- **Instruction Size:** 1 byte
- **Priority:**  P3

##### Opcode 0xB9 - FLDLG2 (Load Log10(2))
- **Description:** Push log base 10 of 2 (0.3010299956639812...) onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(M_LOG10_2)`
- **Instruction Size:** 1 byte
- **Priority:** P3

##### Opcode 0xBA - FLDLN2 (Load Ln(2))
- **Description:** Push natural log of 2 (0.6931471805599453...) onto FPU stack
- **Operands:** None
- **Behavior:** `FPU stack push(M_LN2)`
- **Instruction Size:** 1 byte
- **Priority:** P3

Implementation Example for FLD1:

void handle_FLD1(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    cpu.fpu_push(1.0);
    
    DEBUG_TRACE(DebugCategory::FPU_EXECUTION,
        "[PC=0x{:04X}] [FLD1] Pushed 1.0, ST(0) = {}",
        cpu.get_pc(), cpu.fpu_peek(0));
    
    cpu.set_pc(cpu.get_pc() + 1);
}

---

## Development Tools

SUBSECTION 5.1: GDB-Style CLI Debugger

- **Priority:** P0 Critical
- **Rationale:** Essential for development since GUI debugger was removed due to bugs
- **Timeline:** 2-3 weeks
- **Location:** `src/debug/cli_debugger.cpp and src/debug/cli_debugger.hpp`

Execution Control Commands:

run (alias: r)
- **Description:** Start or restart program execution from the beginning
Usage: run
- **Behavior:** `Reset PC to entry point, clear breakpoint hit counts, begin execution`

continue (alias: c)
- **Description:** Continue execution until next breakpoint or program end
Usage: continue
- **Behavior:** ` Resume execution, stop at next breakpoint or HALT`

step (alias: s)
- **Description:** Execute single instruction
Usage: step or step 5 (step 5 instructions)
- **Behavior:** `Execute one instruction, update display, return to prompt`

stepi (alias: si)
- **Description:** Step into - same as step for assembly level
Usage: stepi
- **Behavior:** `Execute one instruction including into CALL targets`

next (alias: n)
- **Description:** Step over - execute CALL as single step
Usage: next
- **Behavior:** ` If current instruction is CALL, set temporary breakpoint after it and continue`

finish (alias: fin)
- **Description:** Run until current function returns
Usage: finish
- **Behavior:** `Execute until RET instruction at current call depth`

until (alias: u)
- **Description:** Run until specified address or past current line
Usage: until 0x200
- **Behavior:** `Set temporary breakpoint at address and continue`

Breakpoint Commands:

break (alias: b)
- **Description:** Set breakpoint at address or label
Usage: break 0x100 or break main or break *0x100
- **Behavior:** `Add breakpoint, assign ID, print confirmation`

delete (alias: d)
- **Description:** Delete breakpoint by ID or all breakpoints
Usage: delete 1 or delete all
- **Behavior:** `Remove specified breakpoint(s)`

disable
- **Description:** Disable breakpoint without deleting
Usage: disable 1
- **Behavior:** `Breakpoint remains but won't trigger`

enable
- **Description:** Enable previously disabled breakpoint
Usage: enable 1
- **Behavior:** `Breakpoint will trigger again`

info breakpoints (alias: i b)
- **Description:** List all breakpoints with status
Usage: info breakpoints
Output format: ID, Type, Address, Enabled, Hit Count, Condition

tbreak
- **Description:** Set temporary breakpoint (deleted after first hit)
Usage: tbreak 0x100
- **Behavior:** `Breakpoint triggers once then auto-deletes`

condition
- **Description:** Add condition to existing breakpoint
Usage: condition 1 $R0 == 5
- **Behavior:** `Breakpoint only triggers when condition is true`

Inspection Commands:

info registers (alias: i r)
- **Description:** Display all CPU registers
Usage: info registers
Output:  All 134 registers with current values in hex and decimal

info register (alias: i reg)
- **Description:** Display specific register
Usage: info register R0 or info register XMM0
Output: Single register value with details

print (alias: p)
- **Description:** Print expression, register, or memory value
Usage: print $R0 or print *0x100 or print $R0 + $R1
- **Behavior:** ` Evaluate expression and display result`

x (examine)
- **Description:** Examine memory at address
Usage: x/[count][format][size] address
Formats: x=hex, d=decimal, u=unsigned, o=octal, t=binary, a=address, c=char, s=string, i=instruction
Sizes: b=byte, h=halfword(2), w=word(4), g=giant(8)
Examples: x/16xb 0x100 (16 bytes in hex), x/4i 0x100 (4 instructions), x/s 0x200 (string)

display
- **Description:**  Automatically display expression after each stop
Usage: display $R0 or display/x *0x100
- **Behavior:** `Expression shown after every step/breakpoint`

undisplay
- **Description:** Remove automatic display
Usage: undisplay 1
- **Behavior:** `Stop showing that expression`

info flags (alias: i f)
- **Description:** Display CPU flags
Usage: info flags
Output: ZF, SF, CF, OF with current values and meaning

Modification Commands:

set
- **Description:**  Modify register or memory value
Usage: set $R0 = 42 or set *0x100 = 0xFF or set $ZF = 1
- **Behavior:** ` Change specified value immediately`

Memory and Disassembly Commands:

disassemble (alias: disas)
- **Description:** Show disassembled instructions
Usage:  disassemble or disassemble 0x100 0x120 or disassemble main
- **Behavior:** ` Decode and display instructions with addresses and opcodes`

backtrace (alias: bt)
- **Description:** Show call stack
Usage:  backtrace or bt full
- **Behavior:** `Display return addresses and call chain`

frame
- **Description:** Select stack frame
Usage: frame 2
- **Behavior:** `Switch context to specified stack frame`

Watchpoint Commands:

watch (alias:  w)
- **Description:** Set watchpoint on memory location
Usage: watch *0x200
- **Behavior:** `Break when memory value changes`

rwatch
- **Description:** Set read watchpoint
Usage: rwatch *0x200
- **Behavior:** `Break when memory is read`

awatch
- **Description:** Set access watchpoint
Usage: awatch *0x200
- **Behavior:** `Break when memory is read or written`

info watchpoints
- **Description:** List all watchpoints
Usage: info watchpoints
Output: ID, Type, Address, Enabled, Hit Count

Help and Control Commands:

help (alias: h)
- **Description:** Show help for commands
Usage: help or help break or help x
- **Behavior:** `Display usage information`

quit (alias: q)
- **Description:** Exit debugger
Usage: quit
- **Behavior:** `Prompt for confirmation if program running, then exit`

source
- **Description:** Execute commands from file
Usage: source commands.gdb
- **Behavior:** `Read and execute each line as command`

show
- **Description:** Show debugger settings
Usage: show or show prompt
- **Behavior:** `Display current configuration`

Breakpoint Data Structure:

enum class BreakpointType {
    ADDRESS,            // Break at specific PC address
    LABEL,              // Break at symbol/label name
    OPCODE,             // Break when specific opcode executes
    CONDITIONAL,        // Break when condition evaluates true
    TEMPORARY,          // One-shot breakpoint, deleted after hit
    HARDWARE            // Hardware-assisted breakpoint (future)
};

struct Breakpoint {
    uint32_t id;                    // Unique breakpoint ID
    BreakpointType type;            // Type of breakpoint
    uint32_t address;               // Target address (for ADDRESS type)
    std::string label;              // Label name (for LABEL type)
    uint8_t opcode;                 // Target opcode (for OPCODE type)
    std::string condition;          // Condition expression (e.g., "$R0 == 5")
    bool enabled;                   // Is breakpoint active
    uint32_t hit_count;             // Number of times triggered
    uint32_t ignore_count;          // Hits to ignore before stopping
    bool temporary;                 // Delete after first hit
    std::string commands;           // Commands to execute when hit
};

Watchpoint Data Structure:

enum class WatchpointType {
    WRITE,              // Break on write
    READ,               // Break on read
    ACCESS              // Break on read or write
};

struct Watchpoint {
    uint32_t id;                    // Unique watchpoint ID
    WatchpointType type;            // Type of watchpoint
    uint32_t address;               // Memory address to watch
    size_t size;                    // Size of watched region (1, 2, 4, or 8 bytes)
    bool enabled;                   // Is watchpoint active
    uint32_t hit_count;             // Number of times triggered
    uint32_t old_value;             // Previous value (for change detection)
};

CLI Debugger Class Structure:

class CLIDebugger {
public: 
    CLIDebugger(CPU& cpu, std::vector<uint8_t>& program, 
                const std::map<std::string, uint32_t>& symbols);
    
    void run();                     // Main REPL loop
    void attach();                  // Attach to running program
    void detach();                  // Detach from program
    
private:
    CPU& cpu_;
    std::vector<uint8_t>& program_;
    std::map<std::string, uint32_t> symbols_;
    std::map<uint32_t, std::string> reverse_symbols_;
    
    std::vector<Breakpoint> breakpoints_;
    std::vector<Watchpoint> watchpoints_;
    std::vector<std::string> display_expressions_;
    std::vector<std::string> command_history_;
    
    uint32_t next_bp_id_ = 1;
    uint32_t next_wp_id_ = 1;
    bool running_ = false;
    bool attached_ = false;
    
    // Command handlers
    void cmd_run(const std::vector<std::string>& args);
    void cmd_continue(const std::vector<std::string>& args);
    void cmd_step(const std::vector<std::string>& args);
    void cmd_next(const std:: vector<std::string>& args);
    void cmd_finish(const std::vector<std:: string>& args);
    void cmd_break(const std::vector<std:: string>& args);
    void cmd_delete(const std::vector<std::string>& args);
    void cmd_info(const std::vector<std:: string>& args);
    void cmd_print(const std::vector<std:: string>& args);
    void cmd_examine(const std::vector<std:: string>& args);
    void cmd_set(const std::vector<std:: string>& args);
    void cmd_disassemble(const std::vector<std::string>& args);
    void cmd_backtrace(const std::vector<std::string>& args);
    void cmd_watch(const std::vector<std:: string>& args);
    void cmd_help(const std::vector<std:: string>& args);
    void cmd_quit(const std::vector<std::string>& args);
    
    // Parsing utilities
    std::vector<std::string> tokenize(const std::string& input);
    uint32_t parse_address(const std::string& addr_str);
    int64_t evaluate_expression(const std::string& expr);
    
    // Display utilities
    void print_registers();
    void print_register(const std::string& name);
    void print_flags();
    void print_memory(uint32_t addr, size_t count, char format, char size);
    void print_disassembly(uint32_t start, uint32_t end);
    void print_breakpoints();
    void print_watchpoints();
    void print_backtrace();
    void print_displays();
    
    // Execution control
    bool check_breakpoints();
    bool check_watchpoints();
    bool evaluate_condition(const std::string& condition);
    void execute_breakpoint_commands(const Breakpoint& bp);
    
    // Readline integration
    std::string read_line(const std::string& prompt);
    void add_to_history(const std::string& line);
    std::vector<std::string> complete_command(const std::string& partial);
};

---

SUBSECTION 5.2: Interactive REPL

- **Priority:** P2 Medium
- **Rationale:**  Enables rapid prototyping and testing of assembly code
- **Timeline:** 1-2 weeks (after CLI debugger)
- **Location:** `src/demi/repl.cpp and src/demi/repl.hpp`

Required Features:

Execute Assembly Immediately - Priority P1
- **Description:** Type assembly instructions and execute them immediately
Example: User types "LOAD_IMM R0, 42" and it executes, showing result

Command History - Priority P1
- **Description:** Up/down arrows navigate through previous commands
- **Implementation:** Use readline library or custom implementation

Tab Completion - Priority P2
- **Description:** Complete instruction names, register names, labels
Example: User types "LOA" then tab, completes to "LOAD" or shows "LOAD, LOAD_IMM, LOADEX"

Multi-line Input - Priority P2
- **Description:** Support for entering multi-line programs with labels
Example: User can enter a loop with labels, then execute the whole block

State Inspection After Each Instruction - Priority P1
- **Description:**  Optionally show registers, memory, flags after each instruction
- **Implementation:** Toggle with . verbose command

Reset State - Priority P1
- **Description:** Clear CPU state and start fresh
Command: .reset

Load File - Priority P1
- **Description:** Load and execute assembly file within REPL
Command: .load filename. asm

Save Session - Priority P3
- **Description:** Save REPL history to file for later reference
Command: .save filename.txt

REPL Commands (dot-prefixed to distinguish from assembly):

.help - Show help for REPL commands
.regs - Display all registers
.reg NAME - Display specific register
.mem ADDR [COUNT] - Display memory at address
.flags - Display CPU flags
. reset - Reset CPU state to initial
.load FILE - Load and execute assembly file
.save FILE - Save session history to file
.verbose [on|off] - Toggle verbose output after each instruction
.step - Single step mode (pause after each instruction)
.run - Run loaded program until HALT
.quit - Exit REPL

Example REPL Session: 

demi> LOAD_IMM R0, 10
R0 = 10 (0x0000000A)

demi> LOAD_IMM R1, 20
R1 = 20 (0x00000014)

demi> ADD R0, R1
R0 = 30 (0x0000001E)
Flags: ZF=0 SF=0 CF=0 OF=0

demi> .regs
R0  = 0x0000001E (30)
R1  = 0x00000014 (20)
R2  = 0x00000000 (0)
...

demi> .mem 0x100 16
0x00000100: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00

demi> .reset
CPU state reset. 

demi> .quit
Goodbye! 

REPL Class Structure:

class REPL {
public:
    REPL();
    
    void run();                     // Main REPL loop
    
private:
    CPU cpu_;
    Assembler assembler_;
    std::vector<std::string> history_;
    bool verbose_ = false;
    bool step_mode_ = false;
    
    // Command handlers
    void handle_assembly(const std::string& line);
    void handle_dot_command(const std::string& cmd, const std::vector<std::string>& args);
    
    void cmd_help();
    void cmd_regs();
    void cmd_reg(const std::string& name);
    void cmd_mem(uint32_t addr, size_t count);
    void cmd_flags();
    void cmd_reset();
    void cmd_load(const std:: string& filename);
    void cmd_save(const std::string& filename);
    void cmd_verbose(bool enable);
    void cmd_step();
    void cmd_run();
    
    // Utilities
    std::string read_line();
    bool is_dot_command(const std::string& line);
    void print_instruction_result();
    std::vector<std::string> get_completions(const std::string& partial);
};

---

SUBSECTION 5.3: Live Code Reload / Hot-Swapping

- **Priority:** P3 Low
- **Rationale:** Improves development workflow by allowing code changes without restart
- **Timeline:** 1 week
- **Location:** `src/engine/hot_reload.cpp and src/engine/hot_reload.hpp`

Required Features:

File Watching - Priority P2
- **Description:**  Detect when . asm source files change on disk
- **Implementation:** Use inotify on Linux, FSEvents on macOS, ReadDirectoryChangesW on Windows

Incremental Assembly - Priority P3
- **Description:** Only reassemble sections that changed
- **Implementation:** Track section checksums, only regenerate modified sections

State Preservation - Priority P3
- **Description:** Keep register and memory state across reloads when possible
- **Implementation:** Save state before reload, restore after, handle address changes

Rollback on Error - Priority P4
- **Description:** If reload fails to assemble, revert to previous working version
- **Implementation:** Keep backup of last working bytecode

---

## Testing Infrastructure

SUBSECTION 6.1: Unit Test Expansion

- **Priority:** P1 High
- **Rationale:** Current test counts are inconsistent across documentation, need comprehensive coverage
- **Timeline:**  Ongoing
- **Location:** `src/test/ and tests/`

Documentation Issues to Fix: 
- README claims varying test counts (59, 78, 101 unit tests in different places)
- Need to audit actual test files and update all documentation consistently
- Integration tests have been removed, remove all references

Required New Unit Tests by Category:

#### Category:  Core Arithmetic Opcodes
Current Coverage: Good for basic cases
Needed Tests: Edge cases for overflow, underflow, boundary conditions
- **Priority:** P2
Test Cases:
- ADD with 0xFFFFFFFF + 1 (overflow, carry flag)
- ADD with 0x7FFFFFFF + 1 (signed overflow)
- SUB with 0 - 1 (underflow, carry/borrow flag)
- MUL with large numbers (overflow detection)
- DIV by zero (error handling)
- DIV with large dividend (overflow)
- MOD edge cases

#### Category:  Bitwise Operations
Current Coverage: Good
Needed Tests: All bit patterns, shift edge cases
- **Priority:** P3
Test Cases:
- SHL/SHR by 0 (no change)
- SHL/SHR by 32 (full shift)
- SHL/SHR by values > 32
- AND/OR/XOR with 0x00000000
- AND/OR/XOR with 0xFFFFFFFF
- NOT of various patterns

#### Category: Jump Instructions
Current Coverage:  Partial
Needed Tests: All flag combinations for conditional jumps
- **Priority:** P1
Test Cases:
- JZ with ZF=0 and ZF=1
- JNZ with ZF=0 and ZF=1
- JG with all combinations of ZF, SF, OF
- JL with all combinations of SF, OF
- JGE with all combinations of SF, OF
- JLE with all combinations of ZF, SF, OF
- JC/JNC with CF=0 and CF=1
- JO/JNO with OF=0 and OF=1
- JS/JNS with SF=0 and SF=1
- Jump to address 0
- Jump to max address
- Jump backward (negative offset)

#### Category: FPU Operations
Current Coverage:  Partial
Needed Tests: NaN, Infinity, denormal number handling
- **Priority:** P1
Test Cases:
- FADD with NaN operand
- FADD with +Infinity and -Infinity
- FDIV by zero (should produce Infinity, not crash)
- FDIV 0/0 (should produce NaN)
- FSQRT of negative number (NaN)
- FSQRT of 0 (should be 0)
- FSIN/FCOS/FTAN at boundary values (0, pi/2, pi)
- FPU stack overflow (push 9th value)
- FPU stack underflow (pop empty stack)
- Denormal number handling

#### Category:  SIMD Operations
Current Coverage: Basic only
Needed Tests: Float support, all element positions, edge cases
- **Priority:** P1
Test Cases:
- VADD with float values
- VADD with overflow in individual elements
- VMUL with float values
- VDOT accuracy verification
- VMAX with negative numbers
- VMAX with equal values
- All comparison operations with various orderings
- Operations on zeroed vectors
- Operations on max-value vectors

#### Category: 64-bit Operations
Current Coverage: Partial
Needed Tests: Full 64-bit value range
- **Priority:** P2
Test Cases:
- ADD64 with values > 32-bit
- SUB64 with large values
- MOV64 preserves all 64 bits
- CMP64 with large values
- Operations at 64-bit boundaries

#### Category: Memory Operations
Current Coverage: Partial
Needed Tests: All boundary conditions
- **Priority:** P1
Test Cases:
- LOAD from address 0
- LOAD from max valid address
- LOAD from address just past max (should error)
- STORE to address 0
- STORE to max valid address
- STORE to invalid address
- LOAD/STORE at unaligned addresses
- Memory operations near stack

#### Category: Stack Operations
Current Coverage: Good
Needed Tests:  Overflow and underflow conditions
- **Priority:** P2
Test Cases:
- PUSH until stack overflow
- POP from empty stack
- CALL with deep nesting (stack depth)
- RET with empty call stack
- PUSH_FLAG/POP_FLAG preserves all flags

#### Category: Assembler Lexer
Current Coverage: Good
Needed Tests: Unicode, escape sequences, edge cases
- **Priority:** P3
Test Cases:
- Unicode in strings and comments
- All escape sequences (newline, tab, etc.)
- Very long lines
- Empty files
- Files with only comments
- Mixed line endings (CR, LF, CRLF)

#### Category: Assembler Parser
Current Coverage: Good
Needed Tests: Error recovery, edge cases
- **Priority:** P2
Test Cases:
- Missing operand recovery
- Invalid register name handling
- Duplicate label detection
- Forward reference resolution
- Circular reference detection
- Maximum label length
- Reserved word as label

#### Category: Assembler Code Generation
Current Coverage:  Partial
Needed Tests: All addressing modes
- **Priority:** P1
Test Cases: 
- Immediate addressing (all sizes)
- Register direct addressing
- Memory direct addressing
- Register indirect addressing
- All instruction encodings verify byte-accurate

#### Category: Device I/O
Current Coverage: Partial for console, partial for file
Needed Tests: Error conditions
- **Priority:** P2