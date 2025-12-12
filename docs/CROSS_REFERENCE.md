# Documentation Cross-Reference & Verification

This document tracks the relationship between documentation and the codebase, ensuring that documented features, opcodes, and APIs exist in the source code.

## Legend

- ✅ **Verified**: Symbol/Feature found in codebase.
- ⚠️ **Discrepancy**: Documentation differs from code (Fixed).
- ❌ **Missing**: Documented feature not found in code.

## Cross-Reference Table

| Document                                            | Section/Feature    | Code Location                            | Status      | Notes                                                                            |
| --------------------------------------------------- | ------------------ | ---------------------------------------- | ----------- | -------------------------------------------------------------------------------- |
| `docs/reference/QUICK_REFERENCE.md`                 | Opcode Hex Codes   | `src/engine/opcodes/opcode_registry.cpp` | ✅ Verified | Fixed discrepancies in hex codes (MUL, DIV, Jumps, etc.) to match code.          |
| `docs/testing/TEST_FLAGS.md`                        | CLI Flags          | `src/main.cpp`                           | ✅ Verified | Removed non-existent `-it` flag. Fixed formatting.                               |
| `docs/codebase/API_REFERENCE.md`                    | CPU Class Methods  | `src/engine/cpu.hpp`                     | ✅ Verified | Updated methods (`read_mem32`, `write_mem32`) and `Register` enum to match code. |
| `docs/reference/SIMD_REFERENCE.md`                  | Register Mapping   | `src/engine/cpu_registers.hpp`           | ✅ Verified | Fixed XMM register mapping (XMM0=R96).                                           |
| `docs/reference/FPU_REFERENCE.md`                   | Instruction Format | `src/assembler/assembler.cpp`            | ✅ Verified | Corrected bytecode format description.                                           |
| `docs/development/FUTURE_OPCODES.md`                | FPU/SIMD Status    | `src/engine/opcodes/opcode_registry.cpp` | ✅ Verified | Corrected FPU hex codes and updated SIMD implementation status.                  |
| `docs/usage/FEATURES.md`                            | FPU/SIMD Features  | `src/engine/opcodes/opcode_registry.cpp` | ✅ Verified | Verified FPU and SIMD feature descriptions and opcode lists.                     |
| `docs/usage/TROUBLESHOOTING.md`                     | Error Codes        | `src/debug/error_handler.hpp`            | ✅ Verified | Verified error codes match `ErrorCode` enum.                                     |
| `docs/development/ERROR_HANDLING_IMPLEMENTATION.md` | Error/Debug System | `src/debug/error_handler.hpp`            | ✅ Verified | Verified error codes and debug categories.                                       |
| `docs/development/IMPLEMENTATION_STATUS.md`         | Opcode Status      | `src/engine/opcodes/opcode_registry.cpp` | ✅ Verified | Confirmed opcode lists and hex codes are accurate.                               |
| `docs/reference/SIMD_COMPARISON.md`                 | SIMD Opcodes       | `src/engine/opcodes/opcode_registry.cpp` | ✅ Verified | Matches implemented opcodes.                                                     |
| `docs/DOCUMENTATION_INDEX.md`                       | Structure          | File System                              | ✅ Verified | Verified links and file structure.                                               |

## Verification Process

1. **Source of Truth Identification**: Identified the C++ files that define the actual behavior (e.g., `opcode_registry.cpp` for opcodes, `main.cpp` for flags).
2. **Cross-Check**: Compared documentation claims against the source code.
3. **Correction**: Updated documentation where discrepancies were found.
4. **Verification**: Re-checked to ensure accuracy.
