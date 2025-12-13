# Calculator Debugging Session - December 13, 2025

## Problem Summary

The calculator program (`examples/x86/syscalls/calculator.asm`) was failing with "unsupported syscall" errors due to address mismatches in the two-pass assembler.

## Root Cause Identified and Fixed

### Issue: CMP with Immediate Operands Size Mismatch

**Problem:**
- **First Pass (get_instruction_size):** CMP instructions were being sized as 3 bytes (opcode + reg1 + reg2)
- **Second Pass (encode_instruction):** CMP with immediate operands actually emits:
  - LOAD_IMM instruction (6 bytes) to load immediate into temp register
  - CMP instruction (3 bytes) to compare registers
  - **Total: 9 bytes**

**Impact:**
- 5 CMP instructions with immediates between `_start` and `atoi`
- 5 instructions × 6 bytes error each = **30 bytes total mismatch**
- `atoi` label calculated at 0x3A6 in first pass
- `atoi` actually emitted at 0x3C4 in second pass
- CALL instruction targeted wrong address, causing failure

### Fix Applied

Modified `get_instruction_size()` in `src/assembler/assembler.cpp` (lines ~1697-1707):

```cpp
} else if (mnemonic == "CMP" || mnemonic == "CMP64" || mnemonic == "CMPEX") {
    // CMP with immediate requires LOAD_IMM + CMP (9 bytes total)
    // CMP with register is just CMP (3 bytes)
    if (operands.size() >= 2) {
        bool second_is_reg = dynamic_cast<const RegisterExpression*>(operands[1].get()) != nullptr;
        if (!second_is_reg) {
            return 9; // LOAD_IMM (6 bytes) + CMP (3 bytes)
        }
    }
    return 3; // opcode + reg1 + reg2
}
```

### Verification

**Before Fix:**
```
atoi                           0x03A6  (defined)
```
Actual location in bytecode: 0x3C4 (30 bytes off)

**After Fix:**
```
atoi                           0x03C4  (defined)
```
Matches actual bytecode location! ✅

## Debugging Steps Taken

### 1. Initial Investigation
- Suspected bracket syntax (`LOAD reg, [reg]`) conversion issues
- Added helper function `has_bracket_register_syntax()` for consistent detection
- Added comprehensive PASS1_SIZE and PASS2_EMIT debug logging

### 2. Bytecode Analysis
- Implemented `-ao` (assembly output) option to save bytecode to file
- Analyzed hexdump of assembled calculator.hex
- Discovered 30-byte offset between expected and actual function addresses

### 3. Symbol Table Analysis
- Added symbol table printing to show first-pass calculated addresses
- Compared calculated addresses with actual bytecode locations
- Verified `_start` at 0x200 was correct (no error accumulation before this point)

### 4. Systematic Elimination
- Ruled out LOAD/STORE bracket syntax (no such instructions before atoi)
- Ruled out LOAD_IMM sizing issues (correctly sized for 32-bit registers)
- Ruled out INT instruction sizing (correctly 2 bytes)
- Ruled out JMP instruction sizing (correctly 5 bytes)

### 5. Pattern Recognition
- Found exactly 5 CMP instructions with immediate operands between _start and atoi
- 5 CMPs × 6 bytes error = 30 bytes (exact match!)
- Examined encode_instruction() and found special handling for CMP with immediate
- Confirmed get_instruction_size() didn't account for this special case

## Code Changes Made

### 1. Helper Function (assembler.cpp ~lines 1645-1658)
```cpp
bool Assembler::AssemblerEngine::is_bracket_register_syntax(const Assembler::Expression* operand) {
    if (auto mem_expr = dynamic_cast<const Assembler::MemoryReferenceExpression*>(operand)) {
        if (dynamic_cast<const Assembler::RegisterExpression*>(mem_expr->base.get())) {
            return true;
        }
    }
    return false;
}
```

### 2. Debug Logging (assembler.cpp various locations)
- PASS1_SIZE category in get_instruction_size()
- PASS2_EMIT category in encode_instruction()
- PASS2_ADDR tracking in second_pass()

### 3. Assembly Output Option (main.cpp ~lines 851-856, 1339-1365)
- Added `-ao` / `--assembly-output` command-line option
- Writes binary bytecode to file and skips execution
- Prints symbol table after assembly

### 4. CMP Size Fix (assembler.cpp ~lines 1697-1707)
- Added special case for CMP/CMP64/CMPEX with immediate operands
- Returns 9 bytes instead of 3 when second operand is not a register

## Current Status

### ✅ FIXED: Address Mismatch
- CMP instruction sizing corrected
- All symbol addresses now match actual bytecode locations
- atoi function at correct address (0x3C4)

### ❌ NEW ISSUE: Entry Point Problem
**Symptom:**
```
Enter first number: [CRITICAL] CPU_EXECUTION: Invalid opcode: 0x45 at PC=256
```

**Analysis:**
- PC=256 (0x100) is trying to execute data, not code
- 0x100 contains "Enter first number" string (prompt1)
- `_start` label is at 0x200, but execution begins at 0x100
- Opcode 0x45 = ASCII 'E' from "Enter"

**Probable Cause:**
- Entry point calculation in second_pass() may be using first instruction address instead of `_start` label
- Could be related to how entry_address is set when DB/data directives appear before code

**Next Steps for Tomorrow:**
1. Investigate entry_address calculation in second_pass()
2. Check how entry_point_symbol ("_start") is being resolved
3. Verify .text section handling vs data placement
4. Look at lines 326-365 in assembler.cpp (second_pass entry point logic)

## Files Modified

- `src/assembler/assembler.cpp`:
  - Added `is_bracket_register_syntax()` helper
  - Added debug logging throughout
  - Fixed CMP instruction size calculation
  - Added PASS2_ADDR logging in second_pass()
  
- `src/main.cpp`:
  - Added `-ao` command-line option
  - Implemented bytecode file output
  - Added symbol table printing

- `src/config.hpp`:
  - Added `assembly_output` field for output filename

## Test Commands

### Assemble and Check Symbol Table
```bash
./bin/demi-engine-debug -A examples/x86/syscalls/calculator.asm -ao calc.hex 2>&1 | grep -A 100 "Symbol Table"
```

### Verify Specific Symbol Address
```bash
./bin/demi-engine-debug -A examples/x86/syscalls/calculator.asm -ao calc.hex 2>&1 | grep "atoi "
```

### Check Bytecode at Address
```bash
hexdump -C calc.hex -s 0x3C4 -n 16
```

### Test Execution (currently fails at entry point)
```bash
echo -e "5\n+\n3\n" | ./bin/demi-engine-debug -A examples/x86/syscalls/calculator.asm
```

## Key Insights

1. **Two-pass assembly requires perfect size consistency** - Any mismatch accumulates and causes address errors
2. **Special instruction handling must be reflected in size calculation** - CMP with immediate is implemented as two instructions
3. **Symbol table printing is invaluable** - Comparing calculated vs actual addresses quickly identifies issues
4. **Hexdump analysis is essential** - Verifying actual bytecode content confirms or refutes theories
5. **Systematic elimination works** - Methodically ruling out possibilities leads to the root cause

## References

- Calculator source: `examples/x86/syscalls/calculator.asm`
- Assembler implementation: `src/assembler/assembler.cpp`
- Two-pass assembly: first_pass() (lines ~230-325), second_pass() (lines ~326-430)
- CMP special handling: encode_instruction() (lines ~627-665)
- Size calculation: get_instruction_size() (lines ~1671-1760)
