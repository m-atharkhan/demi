# GitHub Issues to Create (Copy-paste these)

## Issue 1: [GOOD FIRST ISSUE] Implement FSUB (FPU Subtraction) Opcode

**Labels:** `good-first-issue`, `FPU`, `assembly`, `beginner-friendly`

**Description:**
We need to implement the FSUB (floating-point subtraction) opcode as part of our FPU expansion.

**What you'll learn:**
- CPU architecture and instruction design
- IEEE 754 floating-point arithmetic
- Systems programming in C++

**Implementation details:**
1. Create `src/engine/opcodes/fsub.hpp` (follow pattern from `fadd.hpp`)
2. Add opcode to `opcodes_consolidated.cpp`
3. Add mnemonic to `lexer.cpp`
4. Write test in `tests/asm/`

**Expected time:** 2-3 hours for a beginner

**Mentorship:** Core team will provide guidance and code review

---

## Issue 2: [GOOD FIRST ISSUE] Add Assembly Example: Fibonacci Calculator

**Labels:** `good-first-issue`, `documentation`, `examples`, `beginner-friendly`

**Description:**
Create a Fibonacci number calculator example to demonstrate DemiEngine's capabilities.

**What you'll learn:**
- DemiEngine assembly syntax
- Loop structures and control flow
- Educational programming

**Requirements:**
1. Create `examples/fibonacci.asm`
2. Include comments explaining each step
3. Add to examples README
4. Should calculate Fibonacci numbers up to a reasonable limit

**Expected time:** 1-2 hours

---

## Issue 3: [HELP WANTED] Implement SIMD Pack/Unpack Instructions

**Labels:** `help-wanted`, `SIMD`, `intermediate`, `performance`

**Description:**
Implement SIMD pack and unpack instructions for vector operations.

**What you'll learn:**
- SIMD programming concepts
- Vector register manipulation
- Performance optimization

**Instructions needed:**
- PACKUSWB (Pack with unsigned saturation)
- PUNPCKLBW (Unpack low bytes)
- PUNPCKHBW (Unpack high bytes)

**Expected time:** 4-6 hours

---

## Issue 4: [GOOD FIRST ISSUE] Improve Test Output Formatting

**Labels:** `good-first-issue`, `testing`, `user-experience`, `beginner-friendly`

**Description:**
Make test output more readable with better formatting and colors.

**What you'll learn:**
- Test framework design
- Console output formatting
- User experience design

**Improvements needed:**
- Add progress bars for test suites
- Color-code pass/fail results
- Show execution time per test
- Add summary statistics

**Expected time:** 2-3 hours

---

## Issue 5: [GOOD FIRST ISSUE] Add More FPU Constants (FLDLN2, FLDLG2, etc.)

**Labels:** `good-first-issue`, `FPU`, `mathematics`, `beginner-friendly`

**Description:**
Implement mathematical constant loading instructions for the FPU.

**Constants to implement:**
- FLDLN2 (Load natural log of 2)
- FLDLG2 (Load log base 2 of e)
- FLDL2T (Load log base 2 of 10)
- FLDL2E (Load log base 2 of e)

**What you'll learn:**
- Floating-point mathematics
- CPU instruction design
- IEEE 754 number representation

**Expected time:** 3-4 hours (includes research)