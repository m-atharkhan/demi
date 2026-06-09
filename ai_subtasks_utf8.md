# Demi Engineering Backlog: ai_subtasks.md

---

# Phase 1: VM Performance Optimization (Current Priority)

VM optimization is the current focus. Threaded code dispatch is complete; next steps are instruction fusion expansion, dispatch profiling, and memory safety hardening.

## TASK-001 (Completed)
- [x] Title: Add computed goto dispatch to VM interpreter
- Category: VM - Dispatch
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/opcodes/opcode_dispatcher_threaded.cpp, src/engine/opcodes/opcode_dispatcher_threaded.hpp, src/engine/opcodes/opcode_dispatcher_inlined.cpp, src/engine/opcodes/opcode_dispatcher_inlined.hpp, src/engine/opcodes/opcode_dispatcher_unified.cpp, src/engine/opcodes/opcode_dispatcher_unified.hpp, src/engine/opcodes/opcode_dispatcher_predictive.cpp, src/engine/opcodes/opcode_dispatcher_predictive.hpp, src/engine/opcodes/opcode_dispatcher.hpp
- Goal: Replace switch-based opcode dispatch with computed goto for faster instruction dispatch.
- Constraints: Must preserve all existing opcode semantics and error handling.
- Definition of Done: VM uses computed goto for dispatch, all tests pass, no regressions.
- Suggested Tests: Full VM test suite, performance benchmarks.
- Risk Level: Medium
- Engineering Notes: Computed goto can improve dispatch speed by 2-3x.

## TASK-002 (Completed)
- [x] Title: Add bounds validation to opcode dispatch table
- Category: VM - Dispatch
- Priority: High
- Estimated Complexity: 1
- Files Required: src/engine/opcodes/opcode_dispatcher.hpp, src/engine/opcodes/opcode_dispatcher_threaded.cpp, src/engine/opcodes/opcode_dispatcher_inlined.cpp, src/engine/opcodes/opcode_dispatcher_unified.cpp, src/engine/opcodes/opcode_dispatcher_predictive.cpp
- Goal: Ensure all opcode indices are validated before dispatch to prevent out-of-bounds access.
- Constraints: No performance regression in release builds.
- Definition of Done: Out-of-bounds opcodes are safely rejected, all tests pass.
- Suggested Tests: Invalid opcode values, fuzzing.
- Risk Level: Low

## TASK-003 (Completed)
- [x] Title: Expand instruction fusion coverage
- Category: VM - Optimization
- Priority: High
- Estimated Complexity: 3
- Files Required: src/engine/opcodes/instruction_fusion.cpp, src/engine/opcodes/instruction_fusion.hpp, src/engine/opcodes/opcodes_consolidated.cpp, src/test/fusion_tests.cpp
- Goal: Extend instruction fusion beyond ADD+STORE to cover common patterns like LOAD+OP, OP+STORE, PUSH+POP sequences.
- Constraints: Must not break existing fusion patterns. Fusion must be optional/disable-able.
- Definition of Done: 5+ fusion patterns implemented, benchmarks show 1.5-2x speedup on fused sequences, all tests pass.
- Suggested Tests: fusion_tests.cpp expansion, benchmark suite, edge cases for each pattern.
- Risk Level: Medium
- Engineering Notes: Patterns identified in instruction_fusion.cpp. Key targets: LOAD_IMM+STORE, ADD+STORE, PUSH+PUSH+CALL.

## TASK-004 (Completed)
- [x] Title: Validate VM memory access boundaries
- Category: VM - Safety
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/cpu.cpp, src/engine/cpu.hpp, src/test/unit_tests.cpp
- Goal: Ensure all memory accesses (load/store) check valid VM memory regions before access.
- Constraints: Debug-only validation, no performance regression in release builds.
- Definition of Done: Out-of-bounds accesses are detected and rejected, error reported, all tests pass.
- Suggested Tests: memory_bounds.test.asm expansion, overflow/underflow/invalid addresses.
- Risk Level: Medium
- Engineering Notes: Added debug-only validate_memory_read/write methods (#ifndef NDEBUG). Updated LOAD, STORE, LOADR, STORER, SWAP opcodes with proper error reporting. Fixed SWAP to report errors on out-of-bounds (was silently failing). Updated read_mem32/write_mem32 to use validation. Added 10 memory bounds unit tests covering all memory access opcodes. All 177 tests pass.

## TASK-005 (Completed)
- [x] Title: Instrument VM for opcode execution profiling
- Category: VM - Optimization
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/engine/opcodes/opcode_dispatcher.hpp, src/engine/opcodes/opcode_dispatcher_threaded.cpp, src/engine/opcodes/opcode_dispatcher_inlined.cpp, src/engine/opcodes/opcode_dispatcher_unified.cpp, src/engine/opcodes/opcode_dispatcher_predictive.cpp, src/engine/opcodes/instruction_fusion.cpp
- Goal: Add optional profiling hooks to count opcode executions for hotspot analysis.
- Constraints: Profiling must be disabled by default in release builds, zero overhead when off.
- Definition of Done: Profiling data available via DebugHandler, no perf impact when disabled, all tests pass.
- Suggested Tests: benchmarks/ suite with profiling enabled/disabled.
- Risk Level: Low
- Engineering Notes: Created OpcodeProfiler singleton class (opcode_profiler.hpp/cpp) with per-opcode counters (256 entries). Added OPCODE_PROFILE macro that compiles to no-op in release builds (#ifdef NDEBUG). Added profiling hooks to all dispatchers (threaded, inlined, unified, predictive) and instruction_fusion.cpp. Added Config::profiling flag. Added print_summary/print_detailed methods for hotspot analysis. Added 5 unit tests covering basic counting, zero-overhead when disabled, reset, hotspots, and exact counts. All 182 tests pass.

## TASK-006 (Completed)
- [x] Title: Add runtime stack overflow detection
- Category: VM - Safety
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/cpu.cpp, src/engine/cpu_registers.cpp, src/engine/cpu.hpp
- Goal: Detect and report stack overflow/underflow at runtime during PUSH/POP/CALL/RET.
- Constraints: Debug-only validation preferred.
- Definition of Done: Stack violations are detected, error reported, all tests pass.
- Suggested Tests: Deep recursion, excessive PUSH sequences, tests/stack.test.asm expansion.
- Risk Level: Medium
- Engineering Notes: Added STACK_LIMIT register (register 134) for configurable stack overflow threshold. Stack limit persists across CPU resets (like max_call_depth_override). validate_stack_push/pop now check against stack_limit_ and return values are properly checked: debug builds throw CPUException on violation. Added stack overflow/underflow validation to save_interrupt_state/restore_interrupt_state. Added 3 new stack limit unit tests (2 negative + 1 positive). Expanded tests/stack.test.asm with 4 overflow/underflow scenarios. All 189 tests pass.

---

# Phase 2: Native Code Generation (x86_64)

The x86_encoder provides the foundation for translating VM bytecode to native x86-64 instructions. These tasks expand it from basic coverage to full production capability.

## TASK-007
- [x] Title: Expand x86_encoder arithmetic opcode coverage
- Category: Codegen
- Priority: High
- Estimated Complexity: 3
- Files Required: src/codegen/x86_encoder.cpp, src/codegen/x86_encoder.hpp, src/codegen/register_allocator.hpp
- Goal: Map remaining VM arithmetic opcodes (SUB, MUL, DIV, INC, DEC, 64-bit variants) to native x86-64 instructions in x86_encoder.
- Constraints: Must maintain correct EFLAGS handling. All translations must be tested.
- Definition of Done: 30+ arithmetic opcodes have native x86-64 translations, encoder tests pass.
- Suggested Tests: Compare VM execution vs native translation output for each opcode.
- Risk Level: Medium
- Engineering Notes: Currently x86_encoder handles basic set; expand using existing patterns in opcodes_consolidated.cpp.

## TASK-008
- [x] Title: Expand x86_encoder logic and control flow coverage
- Category: Codegen
- Priority: High
- Estimated Complexity: 3
- Files Required: src/codegen/x86_encoder.cpp, src/codegen/x86_encoder.hpp
- Goal: Map VM logic (AND, OR, XOR, NOT, SHL, SHR) and control flow (JMP, JZ, JNZ, CALL, RET) opcodes to native x86-64.
- Constraints: Jump targets must use correct x86-64 relative offsets.
- Definition of Done: 15+ logic and 10+ control flow opcodes translated, tests pass.
- Suggested Tests: all tests/opcodes/*.test.asm patterns verified after translation.
- Risk Level: Medium

## TASK-009
- [x] Title: Implement register allocator for VM-to-x86 mapping
- Category: Codegen
- Priority: High
- Estimated Complexity: 3
- Files Required: src/codegen/register_allocator.hpp, src/codegen/x86_encoder.cpp, src/engine/cpu_registers.hpp
- Goal: Create smart register allocation mapping 134 VM registers to 16 x86-64 registers with spill/fill logic.
- Constraints: Must handle all VM register types (general, SIMD vector, FPU stack). Spill to stack when registers exhausted.
- Definition of Done: Register allocator handles all register classes, spilling works, benchmarks show perf improvement.
- Suggested Tests: register_exhaustive.test.asm stress test, register allocation unit tests.
- Risk Level: High
- Engineering Notes: register_allocator.hpp is a stub; needs full implementation.

## TASK-010
- [x] Title: Implement VM-to-native translation pipeline
- Category: Codegen
- Priority: High
- Estimated Complexity: 3
- Files Required: src/codegen/disa_compiler.hpp, src/codegen/x86_encoder.cpp, src/codegen/x86_encoder.hpp, src/engine/opcodes/opcodes_consolidated.cpp
- Goal: Build the translation layer that walks VM bytecode and emits equivalent x86-64 machine code via x86_encoder.
- Constraints: Must handle all 94+ opcodes. Unsupported opcodes should fall back to VM interpretation.
- Definition of Done: Translator produces valid x86-64 machine code for all supported opcodes, all tests pass.
- Suggested Tests: Full test suite running natively via translation output.
- Risk Level: High
- Engineering Notes: disa_compiler.hpp is a stub. Pipeline: bytecode -> x86_encoder -> ELF emitter.

## TASK-011
- [x] Title: Implement ELF generation for native executables
- Category: Codegen
- Priority: Medium
- Estimated Complexity: 3
- Files Required: src/codegen/x86_encoder.hpp (or new src/codegen/elf_emitter.cpp)
- Goal: Generate valid ELF64 binaries from translated x86-64 machine code, with proper ELF headers, sections, and program headers.
- Constraints: Output must be a valid Linux x86-64 ELF executable.
- Definition of Done: `demi-engine -c test.asm -o test` produces a runnable native ELF, all tests pass.
- Suggested Tests: Run generated ELF binaries and verify correct behavior.
- Risk Level: High
- Engineering Notes: New file likely needed. ELF structure: ELF64 header, .text section, section headers.

## TASK-012
- [x] Title: Translate I/O opcodes in DISA compiler
- Category: Codegen - Compiler
- Priority: High
- Estimated Complexity: 2
- Files Required: src/codegen/disa_compiler.cpp
- Goal: Implement native x86-64 translation for IN, OUT, INB, OUTB, INW, OUTW, INL, OUTL, INSTR, OUTSTR opcodes.
- Constraints: Must integrate with the register file / memory model used by the compiler.
- Definition of Done: I/O programs compile to working native code, all tests pass.
- Suggested Tests: Compile and run programs using OUT/OUTSTR; verify console output.
- Risk Level: Medium
- Engineering Notes: Currently emits INT3 fallback for all I/O. Need to emit syscall or buffer-based I/O in the compiled function.

## TASK-013
- [x] Title: Support .data section bytes in native compiler
- Category: Codegen - Compiler
- Priority: High
- Estimated Complexity: 3
- Files Required: src/codegen/disa_compiler.cpp, src/codegen/elf_emitter.cpp
- Goal: Extract .data/.rodata bytes from the VM bytecode blob and embed them into the compiled memory buffer so string constants and initialized data are available to native code.
- Constraints: Must maintain correct addresses between code and data references.
- Definition of Done: Programs with .data sections (e.g., hello_world.asm) compile and run correctly.
- Suggested Tests: Compile hello_world.asm, run resulting ELF, verify correct output.
- Risk Level: High
- Engineering Notes: The assembler produces a flat byte-addressable blob with text and data interleaved. The compiler needs to extract data bytes at their correct offsets and copy them into the memory buffer before executing compiled code.

## TASK-014
- [x] Title: Replace HALT fallback with proper exit syscall
- Category: Codegen - Compiler
- Priority: Medium
- Estimated Complexity: 1
- Files Required: src/codegen/disa_compiler.cpp
- Goal: Change HALT translation from `jmp 0` (infinite loop) to a clean exit syscall that returns to the _start stub.
- Constraints: Must not crash or produce undefined behavior.
- Definition of Done: HALT terminates program execution cleanly with exit code 0.
- Suggested Tests: Compile and run a program ending with HALT; verify exit code 0.
- Risk Level: Low
- Engineering Notes: HALT currently maps to `emit_jmp_rel32(0)` which loops forever. Replace with `emit_ret()` to return to _start stub which calls sys_exit.

# Phase 2B: Example Audit Bug Fixes

Bug fixes discovered from running all 55 example programs under the emulator with verbose debug output. 44/55 ran successfully; these 5 issues caused assembly or runtime failures.

## TASK-015
- [x] Title: Fix .data/.text section overlap causing _start to point into data bytes
- Category: Assembler - Bug
- Priority: High
- Estimated Complexity: 3
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp, src/assembler/parser.cpp
- Goal: Programs with both `.data` and `.text` sections using `.org` directives cause the `_start` label to resolve inside DB data bytes instead of code. When both sections set `.org 0x100`, DB writes data at 0x100, then `.text` instructions are appended after data (>0x119), but `_start` resolves to 0x100 (data). Execution starts on "Hello from label" ASCII bytes, causing "Invalid opcode: 0x48" at PC=256.
- Constraints: Must not break programs without section directives or with only one section.
- Definition of Done: `labels_and_strings.asm` (both x86 and x64) assemble and execute correctly. All 229 tests pass.
- Suggested Tests: labels_and_strings.asm execution, mixed .data/.text with overlapping addresses, entries without _start.
- Risk Level: High
- Engineering Notes: Root cause: the assembler tracks `current_address` globally across sections. `.org` in `.data` sets the address, then `.org` in `.text` resets it backward, causing overlap. Fix: sections should maintain independent address counters, or `.text` `.org` should be relative to section start rather than the global address map.

## TASK-016
- [X] Title: Fix OUT instruction "argument not found" error in x86 counting_loop
- Category: Assembler - Bug
- Priority: High
- Estimated Complexity: 2
- Files Required: src/assembler/parser.cpp, src/assembler/assembler.cpp, src/codegen/disa_compiler.cpp
- Goal: `OUT ECX, 1` in `examples/x86/control_flow/counting_loop.asm` fails with "Runtime error: argument not found". The OUT instruction takes register+port but the assembler/compiler doesn't resolve the second argument correctly in this context.
- Constraints: Must not break existing OUT/IN instruction handling in other examples.
- Definition of Done: `counting_loop.asm` runs to completion, outputs "1 2 3 4 5 ". All 229 tests pass.
- Suggested Tests: counting_loop.asm, all existing OUT/IN test cases, edge cases with variable port arguments.
- Risk Level: Medium
- Engineering Notes: The error occurs at PC=0x124 (OUT instruction). The x86 encoding of `OUT ECX, 1` might be miscounting or misparsing operands. Compare handling between x86 and x64 OUT instruction forms.

## TASK-017
- [x] Title: Implement STOREX opcode for x64 indexed addressing
- Category: Assembler - Opcode
- Priority: High
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/parser.cpp, src/engine/opcodes/opcodes_consolidated.cpp
- Goal: x64 data examples (`indirect_addressing.asm`, `string_reverse.asm`) use `STOREX` instruction which is not defined in the assembler. Implement STOREX as an x64 indexed store opcode or update examples to use valid opcodes.
- Constraints: Must not break existing STORE variants. Must work in x64 mode.
- Definition of Done: `indirect_addressing.asm` and `string_reverse.asm` (x64) assemble and execute correctly.
- Suggested Tests: Indirect addressing assembly, indexed store operations.
- Risk Level: Medium
- Engineering Notes: STOREX appears to be intended as "store extended" or "store with index" on x64. Check if this maps to the existing STORE opcode with a different encoding, or if it needs a new opcode handler.

## TASK-018 (Completed)
- [x] Title: Fix x64 3-operand requirement for MUL64/DIV64/AND64/OR64/XOR64
- Category: Assembler - Opcode
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/parser.cpp, src/engine/opcodes/opcodes_consolidated.cpp
- Goal: x64 examples use 2-operand forms (e.g., `MUL64 R1, R2`) but the assembler expects 3 operands (`MUL64 dst, src1, src2`). Either add 2-operand aliases or update the examples.
- Constraints: Must not break existing 3-operand forms.
- Definition of Done: `core_instructions.asm` and `decimal_output.asm` (x64) assemble and execute correctly.
- Suggested Tests: Both 2-operand and 3-operand forms for MUL64/DIV64/AND64/OR64/XOR64.
- Risk Level: Medium
- Engineering Notes: Created OR64/XOR64 runtime handlers (or64.hpp/cpp, xor64.hpp/cpp) since they had opcode definitions but no implementation. Registered in opcode registry and all dispatchers. Fixed assembler double-opcode emission for 2-operand forms (removed redundant emit_byte at line 1486). Added NOT64 handler (opcode 0x59, missing in all dispatchers). Fixed predictive dispatcher default: case to use dispatch_opcode() (single instruction) instead of dispatch_opcode_inlined_optimized() (entire program), eliminating "made no progress" false positives for INC64/RET. Fixed fusion engine DEBUG_INFO missing pattern_name() argument. All 229 tests pass, all 61 examples run clean.

## TASK-019 (Completed)
- [x] Title: Increase default call stack depth or fix factorial_recursive test
- Category: VM - Config
- Priority: Low
- Estimated Complexity: 1
- Files Required: src/engine/cpu.cpp, examples/x86/advanced/factorial_recursive.asm
- Goal: `factorial_recursive.asm` (x86) hits 256-level call stack limit when computing factorial(10). Either increase the default max call depth, or adjust the example to use a smaller input.
- Constraints: Stack depth increase must not cause excessive memory usage.
- Definition of Done: factorial_recursive.asm runs to completion. All tests pass.
- Suggested Tests: factorial_recursive.asm with various inputs, stack overflow detection still works.
- Risk Level: Low
- Engineering Notes: Root cause was a stack offset bug in the x86 factorial_recursive, not the call depth limit. The function's prologue includes PUSH EBP; MOV EBP,ESP; PUSH EBX, adding 12 bytes to the stack before argument access. LOAD_IMM EBX, 8 should have been LOAD_IMM EBX, 12 to reach the argument at EBP+8 (passing through saved EBP at +4 and saved EBX at +0). The wrong offset loaded the return address as n, causing runaway recursion. Fixed the offset and the function correctly computes factorial(10). Also fixed calculator.asm itoa STORER operands which were swapped (addr_reg and value_reg reversed).

---

# Phase 3: Audit Security & Bug Fixes (CRITICAL + HIGH Priority)

These tasks were identified during automated code review of the full codebase (2026-05-29). They address real vulnerabilities, bugs, and performance issues discovered across all source files. See ~/notes/demi/SECURITY_AUDIT.md and ~/notes/demi/OPTIMIZATION_AUDIT.md for full reports.

### Critical Security

## TASK-020
- [x] Title: Fix C1 - Enable memory validation in all builds
- Category: Security - Critical
- Priority: Critical
- Estimated Complexity: 2
- Files Required: src/engine/cpu.cpp, src/engine/cpu.hpp
- Goal: All four validation functions (validate_memory_read, validate_memory_write, validate_stack_push, validate_stack_pop) are wrapped entirely in `#ifndef NDEBUG`. In release builds they always return true, bypassing all bounds checks. Any opcode handler using memory[addr] directly can produce heap buffer over-reads/over-writes in the host process.
- Constraints: Release builds must have the bounds check (addr + size > memory.size()) but without the fmt::format/diagnostic overhead. Keep the debug-only logging wrapped in #ifndef NDEBUG, but do the actual bounds check unconditionally.
- Definition of Done: Bounds checks execute in all build configurations. Debug builds get full diagnostics; release builds get the bare bounds check with return false. All tests pass.
- Suggested Tests: Memory bounds tests in debug and release. Run existing test suite with NDEBUG defined to verify.
- Risk Level: Critical
- Engineering Notes: The fix pattern: move `if (addr + size > memory.size())` outside the `#ifndef NDEBUG`. Keep the fmt::format + ErrorHandler::report_runtime inside `#ifndef NDEBUG` only. The bounds check itself runs in all builds. This gives us safety with near-zero overhead. The handle_call #ifndef NDEBUG guard on validate_stack_push(8) (M3) is also fixed as part of this.

### High Security

## TASK-021
- [x] Title: Fix H1+L2 - Integer overflow in SYS_READ/SYS_WRITE/fetch_operand bounds
- Category: Security - High
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/cpu.cpp
- Goal: SYS_READ and SYS_WRITE use `arg2 + arg3 <= memory.size()` where both are uint32_t. If arg2 is in-bounds but the sum overflows uint32_t, the bound check passes but arg3 (the original large value) is used as the loop/read bound. Also fix the same pattern in fetch_operand() where `get_pc() + 1 >= memory.size()` can wrap if PC is 0xFFFFFFFF.
- Constraints: No performance regression. Must handle 64-bit arithmetic correctly on all platforms.
- Definition of Done: Overflow-safe bounds checks. All tests pass.
- Suggested Tests: Large arg2/arg3 values near UINT32_MAX, edge cases.
- Risk Level: High
- Engineering Notes: Use `static_cast<size_t>(arg2) + static_cast<size_t>(arg3) <= memory.size()` for the 32-bit args. The write loop uses `memory[arg2 + i]` which needs the same check. For fetch_operand, use size_t cast.

## TASK-022
- [x] Title: Fix H2 - uint64_t overflow in public API bounds checks
- Category: Security - High
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/engine_api.cpp
- Goal: map_memory, write_memory, and read_memory all check `virtual_address + size > cpu_.get_memory_size()` using uint64_t arithmetic. If both values are near UINT64_MAX, the sum wraps to a small value. These are the public API entry points.
- Constraints: Must maintain backward compatibility. No breaking changes to API signatures.
- Definition of Done: Overflow-safe bounds checks in all three functions. All tests pass.
- Suggested Tests: Large address/size combinations, edge cases near UINT64_MAX.
- Risk Level: High
- Engineering Notes: Use `size > cpu_.get_memory_size() || virtual_address > cpu_.get_memory_size() - size` pattern to avoid the overflow. For map_memory the resize path is special ÔÇö check overflow before the resize call, and note that resize_memory takes size_t which may truncate.

## TASK-023
- [x] Title: Fix H3+H4+M4 - Syscall boundary hardening (flags, fd tracking, close guard)
- Category: Security - High
- Priority: High
- Estimated Complexity: 3
- Files Required: src/engine/cpu.cpp, src/engine/cpu.hpp
- Goal: Three syscall boundary issues: (1) SYS_OPEN passes guest-controlled arg2 (flags) and arg3 (mode) directly to ::open() ÔÇö a guest can create files with O_CREAT|O_WRONLY within the VFS. (2) SYS_READ/SYS_WRITE pass any non-stdin/stdout fd directly to ::read()/::write(), bypassing sandbox. (3) SYS_CLOSE passes guest-controlled fd to ::close() ÔÇö can close host stdin/stdout.
- Constraints: Must not break existing programs that legitimately use file I/O through the VFS. Existing stdout_hook behavior must be preserved.
- Definition of Done: (1) SYS_OPEN strips unsafe flags (O_CREAT, O_TRUNC) unless sandbox explicitly permits. (2) SYS_READ/WRITE/CLOSE track which fds were opened by the VM and reject unrecognized fds. Stdout (1) and stderr (2) handled via hooks as before. (3) SYS_CLOSE prohibits closing fds 0/1/2. All tests pass.
- Suggested Tests: File open/create attempts, fd reuse, close of stdin/stdout/stderr.
- Risk Level: High
- Engineering Notes: Add a small std::unordered_set<int> vm_fds_ to track fds opened by SYS_OPEN. Check it in SYS_READ/WRITE/CLOSE. For the flag stripping, use a whitelist: O_RDONLY, O_WRONLY, O_RDWR, O_APPEND, O_NONBLOCK. Strip O_CREAT, O_TRUNC, O_EXCL unless sandbox allows file creation.

## TASK-024
- [x] Title: Fix H5+L6 - C API dangling pointer and memcpy bounds
- Category: Security - High
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/engine_c_api.cpp, include/demi/engine_c_api.h
- Goal: (H5) demi_engine_last_error() returns `ctx->engine->last_error().c_str()` ÔÇö a raw const char* into a std::string buffer that gets invalidated on the next engine API call. (L6) demi_engine_read_memory() does `std::memcpy(data, bytes.data(), bytes.size())` with no size check on the output buffer `data`.
- Constraints: Must not change the C API function signatures (no ABI break). Must maintain backward compatibility.
- Definition of Done: (H5) Last error string remains valid across subsequent API calls ÔÇö either copy into a thread-local or EngineContext-owned buffer. (L6) memcpy is bounds-checked against the requested size parameter. All tests pass.
- Suggested Tests: Rapid successive last_error calls, read_memory with various buffer sizes.
- Risk Level: High
- Engineering Notes: For H5, the simplest fix is to store a copy of the error string in the EngineContext (ctx->last_error_str = ...) and return its .c_str(). The EngineContext already has ctx->last_error for the C++ side. For L6, add a check that the returned bytes match the requested size before memcpy, or use std::copy_n(data, size, bytes.data()).

---

# Phase 3B: Audit Optimization Fixes (HIGH Priority)

High-impact performance issues in hot code paths.

## TASK-025
- [x] Title: Fix O1+O2+O3 - Logger and debug handler optimizations
- Category: Optimization - High
- Priority: High
- Estimated Complexity: 2
- Files Required: src/debug/logger.cpp, src/debug/debug_handler.cpp, src/debug/debug_config.cpp
- Goal: Three hot-path optimizations: (O1) logger.cpp constructs a new std::regex on every log message for ANSI stripping. (O2) debug_handler.cpp has two identical 107-line category_to_string switches; log_debug() duplicates the mapping instead of calling category_to_string(). (O3) debug_config.cpp's get_all_categories() returns a 40-entry std::unordered_map by value and is called from ~10 method bodies, each allocating/deallocating the entire map.
- Constraints: No behavioral changes. Output must be identical.
- Definition of Done: (O1) regex is static const. (O2) log_debug calls category_to_string() instead of duplicating. (O3) get_all_categories() returns const static unordered_map&. All tests pass.
- Suggested Tests: Debug output comparison, logging stress test.
- Risk Level: Low
- Engineering Notes: For O1, use `static const std::regex ansi_regex("\033\\[[0-9;]*m");` ÔÇö this compiles once. For O2, simply delete the duplicate switch in log_debug and use `category_to_string(category)` instead. For O3, change return type from `std::unordered_map` to `const std::unordered_map&` and declare static const inside the function.

---

# Phase 3C: Audit Security Fixes (MEDIUM Priority)

## TASK-026
- [x] Title: Fix M1+M2+M10 - Interrupt safety hardening
- Category: Security - Medium
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/engine/interrupt_controller.cpp, src/engine/interrupt_controller.hpp, src/engine/cpu.cpp
- Goal: (M1) interrupt queue has no size cap ÔÇö attacker flooding INT instructions can exhaust host memory. (M2) enter_interrupt logs error when depth exceeds MAX_NESTING_LEVEL but doesn't reject the interrupt. (M10) IVT lookup computes `ivt_base + (vector * 4)` which can overflow 32 bits ÔÇö the subsequent read_mem32 has no bounds check in release builds (see C1).
- Constraints: Must not break legitimate interrupt usage patterns.
- Definition of Done: (M1) Queue size limited (e.g., 256 max). (M2) Exceeding MAX_NESTING_LEVEL causes interrupt rejection, not just a log message. (M10) IVT computation uses 64-bit arithmetic with bounds check. All tests pass.
- Suggested Tests: Interrupt flood, deep nesting, edge vector numbers.
- Risk Level: Medium
- Engineering Notes: For M1, add check before push_back: `if (interrupt_queue_.size() >= MAX_QUEUED_INTERRUPTS) return false;`. For M2, after logging the error, return without enqueuing. For M10, use uint64_t for the computation and check < memory.size() - 4.

## TASK-027
- [x] Title: Fix M5 - Path traversal in preprocessor .include
- Category: Security - Medium
- Priority: Medium
- Estimated Complexity: 1
- Files Required: src/assembler/preprocessor.cpp
- Goal: .include directive concatenates paths with `base_path / include_path` but doesn't sanitize `../` sequences, allowing reading of arbitrary host files during assembly.
- Constraints: Must not break legitimate relative includes (e.g., `../common/macros.inc` that stays within the project directory).
- Definition of Done: Path traversal attempts are detected and rejected. Legitimate relative includes work.
- Suggested Tests: include with ../ escape attempts, normal relative includes.
- Risk Level: Medium
- Engineering Notes: After resolving the path, canonicalize it and verify it's still within the original base directory (or a configured include root). Use fs::canonical or fs::weakly_canonical and check the prefix.

## TASK-028
- [x] Title: Fix M6+M7+M8 - Assembler state and table bugs
- Category: Assembler - Bug
- Priority: Medium
- Estimated Complexity: 3
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp
- Goal: (M6) Static mutable `db_next_addr` is shared across all AssemblerEngine instances ÔÇö concurrent assembly runs corrupt each other. (M7) init_opcode_table() maps SSE/FPU opcodes twice (via enum then via hex), second set overwrites first. (M8) get_instruction_size has a copy-paste bug: `effective_mnemonic == "STOREX" || effective_mnemonic == "STOREX"` ÔÇö a different mnemonic (LOADEX) was almost certainly intended.
- Constraints: Must not break existing opcode encoding or table structure.
- Definition of Done: (M6) db_next_addr is per-instance. (M7) One correct set of SSE/FPU mappings. (M8) Duplicate check fixed (identified the correct mnemonic intended). All tests pass.
- Suggested Tests: Concurrent assembly stress test, SSE/FPU opcode encoding, instruction size calculation for affected mnemonics.
- Risk Level: Medium
- Engineering Notes: For M6, move db_next_addr from file-static to a member of AssemblerEngine. For M7, remove the redundant mapping set ÔÇö keep whichever is correct, verify all opcode values in cpu.hpp. For M8, examine the context. LOADEX is opcode 0x5B for extended register addressing. If that's what was intended, fix the second comparison.

## TASK-029
- [x] Title: Fix M9 - token::as_xxx() silent type mismatch defaults
- Category: Assembler - Bug
- Priority: Medium
- Estimated Complexity: 1
- Files Required: src/assembler/token.hpp
- Goal: as_int(), as_uint(), as_float(), as_string() all silently return 0/0.0/text when the variant holds a different type. A lexer/consumer type mismatch produces zero-values with no diagnostic ÔÇö data corruption without error reporting.
- Constraints: Callers may rely on current behavior ÔÇö audit all call sites before changing defaults.
- Definition of Done: Type mismatches are detected and reported, or failing that, at minimum the behavior is documented. No silent data corruption.
- Suggested Tests: Type mismatch scenarios for each as_xxx() method.
- Risk Level: Medium
- Engineering Notes: Options: (A) Use std::get instead of std::get_if and let the variant throw std::bad_variant_access (caught upstream). (B) Add a debug assert before the silent default. (C) Add a parameter to control strict vs lenient mode. Simplest: replace the silent fallback with a throw or a log+throw pattern.

---

# Phase 3D: Audit Optimization Fixes (MEDIUM Priority - Correctness)

## TASK-030
- [ ] Title: Fix O12+O13 - Missing CPU flags in DIV and MUL64
- Category: Optimization - Medium/Correctness
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/engine/opcodes/opcodes_consolidated.cpp, src/engine/opcodes/mul64.cpp
- Goal: (O12) handle_div computes quotient/remainder but doesn't set ZF/SF/CF/OF flags. Contrast with handle_div64 which does set flags. Callers relying on flags after DIV get stale state. (O13) MUL64 doesn't set ZF/SF/CF/OF despite computing the full 128-bit result. Other 64-bit arithmetic ops set flags correctly.
- Constraints: Must match flag semantics of x86 DIV/MUL or at minimum be internally consistent with other ops.
- Definition of Done: handle_div and handle_mul64 set ZF/SF/CF/OF correctly. All tests pass.
- Suggested Tests: DIV followed by JZ/JNZ, MUL64 flag checks.
- Risk Level: Medium
- Engineering Notes: For DIV: ZF = (quotient == 0), SF = sign bit of quotient, CF/OF cleared (x86 convention: DIV leaves CF/OF undefined, but zeroing is safest). For MUL64: ZF = (result == 0), SF = sign bit, CF/OF set if high 64 bits of 128-bit result are non-zero (x86 MUL convention).

## TASK-031
- [ ] Title: Fix O4+O5 - I/O batching and fmt::format debug guard ordering
- Category: Optimization - Medium
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/engine/devices/file_device.hpp, src/engine/opcodes/instruction_fusion.cpp, src/engine/opcodes/fmul.hpp, src/engine/opcodes/fsin.hpp, src/engine/opcodes/vdot.cpp, src/engine/opcodes/storex.cpp, src/engine/opcodes/addex.cpp, src/engine/opcodes/movex.cpp
- Goal: (O4) file_device.hpp::saveToFile() is called on every single byte write ÔÇö opens file, writes entire buffer, closes. 1000 bytes = 1000 full I/O cycles. (O5) FUSION_DEBUG and DEBUG_INFO macros evaluate fmt::format as macro arguments before the if(Config::debug) guard, so format strings are constructed on every hot path even with debug off.
- Constraints: File device behavior must be externally identical. Debug output must be identical when enabled.
- Definition of Done: (O4) File device uses dirty-flag flush on close/reset. (O5) fmt::format calls moved inside the conditional block in all affected macros/handlers. All tests pass.
- Suggested Tests: File device write stress test, debug-off performance comparison.
- Risk Level: Medium
- Engineering Notes: For O4, add a bool dirty_ flag. saveToFile is only a flush mechanism ÔÇö the actual write to the buffer happens in write(). Change from per-byte save to dirty-flag batch. For O5, restructure macros to put the format call inside the if block, e.g.: `do { if (Config::test_debug) { DebugHandler::instance().report(..., fmt::format(...)); } } while(0)`.

## TASK-032
- [ ] Title: Fix O6+O7+O8 - Assembler pipeline and preprocessor optimizations
- Category: Optimization - Medium
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/assembler/preprocessor.cpp, src/assembler/demi_assembler.cpp
- Goal: (O6) expand_macros compiles a new regex on every iteration for every function macro on every line. (O7) should_include_line copies the entire conditional stack (~5 times per line from process_line). (O8) demi_assembler.cpp has three copies of the same preprocess-lex-parse-assemble pipeline.
- Constraints: No behavioral changes. Macro expansion must be identical.
- Definition of Done: (O6) Regex cached. (O7) Stack reference or depth counter instead of full copy. (O8) Single private method shared by three public entry points. All tests pass.
- Suggested Tests: Macro-heavy assembly, nested conditionals, all three compile modes.
- Risk Level: Medium
- Engineering Notes: For O6, cache compiled regexes in an unordered_map keyed by macro name. For O7, instead of copying the stack, compute whether any inactive state exists by iterating. For O8, create a private `pipeline()` method that takes mode/args and is called by the three public methods.

## TASK-033
- [ ] Title: Fix O9 - Replace encode_instruction 1200-line if-else with map lookup
- Category: Optimization - Medium
- Priority: Medium
- Estimated Complexity: 3
- Files Required: src/assembler/assembler.cpp
- Goal: encode_instruction uses a 1200-line if-else chain of string comparisons for every instruction encoding. An unordered_map<std::string, handler_fn> would be O(1) instead of O(n).
- Constraints: Must be behaviorally identical. All existing opcodes must be handled.
- Definition of Done: Map-based dispatch produces identical bytecode for all test inputs. All tests pass. Benchmarks show improved assembly time.
- Suggested Tests: Full test suite, assembly time comparison.
- Risk Level: Medium
- Engineering Notes: Build the map lazily on first call. Each handler takes the same args as the current if-else branches. This is a significant refactor but the payoff is substantial for large programs.

## TASK-034
- [ ] Title: Fix O10+O11 - Register allocator and RAM disk optimizations
- Category: Optimization - Medium
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/codegen/register_allocator.cpp, src/engine/devices/ramdisk_device.hpp
- Goal: (O10) evict_least_recently_used() does O(n) linear scan of virt_to_phys on every spill. (O11) getStorage() returns std::vector<uint8_t> by value ÔÇö large allocation for big RAM disks.
- Constraints: Must not break register allocation correctness. RAM disk behavior must be identical.
- Definition of Done: (O10) LRU uses priority queue or timestamped container. (O11) getStorage() returns const std::vector<uint8_t>&. All tests pass.
- Suggested Tests: Register pressure stress test, RAM disk storage comparison.
- Risk Level: Medium
- Engineering Notes: For O10, add a last_access_counter_ and track per-physical-reg access times. For O11, simply change the return type to const reference (the internal buffer lives as long as the device).

## TASK-035
- [ ] Title: Fix O21+O22+O23 - FPU and debug handler micro-optimizations
- Category: Optimization - Low
- Priority: Low
- Estimated Complexity: 1
- Files Required: src/engine/opcodes/fabs.hpp, src/engine/opcodes/fchs.hpp, src/engine/opcodes/fcos.hpp, src/engine/opcodes/fsin.hpp, src/engine/opcodes/fsqrt.hpp, src/engine/opcodes/ftan.hpp, src/debug/debug_handler.cpp
- Goal: (O21) 6 FPU unary opcodes all do pop()+push(result) instead of fpu_store(0, result). (O22) ~10 FPU handlers duplicate the same 8-line little-endian double decode. (O23) debug_handler::record_session_event does vector::erase(begin()) ÔÇö O(n) per push when full.
- Constraints: FPU behavior must be bit-identical.
- Definition of Done: (O21) Unary ops use fpu_store(0, result). (O22) Shared decode utility function. (O23) Session recording uses std::deque or ring buffer. All tests pass.
- Suggested Tests: FPU accuracy tests, debug session recording stress test.
- Risk Level: Low

---

# Phase 3E: AI Slop Cleanup (Code)

## TASK-036
- [ ] Title: Remove AI slop from source code
- Category: Maintenance - Code Quality
- Priority: Low
- Estimated Complexity: 2
- Files Required: src/engine/cpu.cpp, src/engine/opcodes/opcode_dispatcher_threaded.hpp, src/engine/opcodes/opcode_dispatcher_inlined.hpp, src/engine/opcodes/opcode_dispatcher_predictive.hpp, src/engine/opcodes/instruction_fusion.hpp, ~50 opcode header files, src/assembler/assembler.cpp, src/assembler/demi_assembler.cpp, src/test/test_framework.hpp
- Goal: Remove obvious-code comments (~40 in cpu.cpp), trim 15-25 line Doxygen blocks from dispatcher headers to 1-2 sentence descriptions, strip redundant "Opcode: NAME" comments from ~50 opcode .hpp files, remove dead code artifacts (else if (false), commented-out corpses, no-op dead statements), remove gratuitous log messages.
- Constraints: Must not remove any actual functionality. Doc-blocks must still convey purpose.
- Definition of Done: Clean source files with only useful comments. All tests pass.
- Suggested Tests: Full test suite to verify no behavioral changes.
- Risk Level: Low
- Engineering Notes: See AI_SLOP_AUDIT.md for the comprehensive list of comments and dead code to remove. Key files: cpu.cpp lines 163,196,199,225,254,279,299,318,321,350,359,30-33; opcode_dispatcher_*.hpp file header blocks; all opcode/*.hpp header comments; assembler.cpp line 572, demi_assembler.cpp lines 169-170, test_framework.hpp line 74.

---

# Phase 3F: Documentation Accuracy

## TASK-037
- [x] Title: Fix documentation accuracy
- Category: Documentation
- Priority: Medium
- Estimated Complexity: 3
- Files Required: docs/usage/README.md, docs/reference/FPU_REFERENCE.md, docs/reference/SIMD_REFERENCE.md, docs/README.md, DOCUMENTATION_INDEX.md
- Goal: Fix dangerously wrong opcode hex values in docs/usage/README.md (LOAD=0x02 vs actual 0x06, etc. ÔÇö nearly every instruction is wrong). Fix FPU/SIMD reference syntax that doesn't match the assembler. Collapse DOCUMENTATION_INDEX.md into docs/README.md (they're duplicates). Pick one canonical source for opcode/test counts and delete outdated alternatives.
- Constraints: Documentation must match actual implementation.
- Definition of Done: docs/usage/README.md has correct opcodes. Reference files match actual assembler syntax. One canonical source for status information.
- Suggested Tests: Verify documented opcodes against cpu.hpp. Verify documented syntax against actual assembler parsing.
- Risk Level: Low
- Engineering Notes: See DOCUMENTATION_AUDIT.md for the full opcode-to-actual comparison table. The usage README hex values appear to be one-based where actual values are zero-based or vice versa, plus some are entirely wrong.

---

# Phase 4: Assembler Macro System & Conditional Assembly

## TASK-038
- [ ] Title: Implement assembler macro definition and expansion
- Category: Assembler - Macros
- Priority: High
- Estimated Complexity: 3
- Files Required: src/assembler/preprocessor.cpp, src/assembler/preprocessor.hpp, src/assembler/assembler.cpp, src/assembler/parser.cpp
- Goal: Add .macro/.endmacro support with parameterized macros, local labels, and macro expansion in the preprocessor.
- Constraints: Must not break existing assembly syntax. Macro expansion must be visible in debug output.
- Definition of Done: Macros with parameters, default values, and local labels work; all tests pass.
- Suggested Tests: tests/preprocessor.test.asm, tests/preprocessor_advanced.test.asm expansion.
- Risk Level: Medium
- Engineering Notes: preprocessor.cpp already has .define/.include; extend with .macro/.endmacro.

## TASK-039
- [ ] Title: Implement conditional assembly directives
- Category: Assembler - Directives
- Priority: High
- Estimated Complexity: 2
- Files Required: src/assembler/preprocessor.cpp, src/assembler/preprocessor.hpp, src/assembler/parser.cpp
- Goal: Add .ifdef, .ifndef, .if, .elif, .else, .endif directives for conditional compilation.
- Constraints: Must support constant expressions in .if conditions. Nesting must work.
- Definition of Done: Conditional directives work in all combinations, tests pass.
- Suggested Tests: Conditional assembly edge cases, nested conditionals, preprocessor.test.asm expansion.
- Risk Level: Medium

## TASK-040
- [ ] Title: Implement structured data definition directives
- Category: Assembler - Data
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp, src/assembler/parser.cpp
- Goal: Add .struct/.endstruct, .enum/.endenum, .array directives for structured data definition.
- Constraints: Must integrate with existing symbol/label resolution.
- Definition of Done: Structs, enums, and arrays can be defined and accessed; tests pass.
- Suggested Tests: tests/data.test.asm expansion, field access via labels.
- Risk Level: Medium

## TASK-041
- [ ] Title: Implement register aliases
- Category: Assembler - Syntax
- Priority: Medium
- Estimated Complexity: 1
- Files Required: src/assembler/parser.cpp, src/assembler/parser.hpp, src/assembler/token.hpp
- Goal: Add .alias directive to define register aliases (e.g., `.alias Accumulator R0`).
- Constraints: Aliases must be scoped and not shadow existing reserved words.
- Definition of Done: Register aliases usable in instructions, tests pass.
- Suggested Tests: tests/directives.test.asm expansion.
- Risk Level: Low

## TASK-042
- [ ] Title: Implement loop and high-level control flow directives
- Category: Assembler - Syntax
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/parser.cpp
- Goal: Add .loop/.endloop, .while/.endwhile, .for/.endfor high-level loop directives.
- Constraints: Must expand to correct jump/compare instructions during assembly.
- Definition of Done: All loop types work correctly, tests pass.
- Suggested Tests: Nested loops, edge cases, performance comparison with manual loop asm.
- Risk Level: Medium

## TASK-043
- [ ] Title: Implement scoped labels and namespaces
- Category: Assembler - Syntax
- Priority: Medium
- Estimated Complexity: 2
- Files Required: src/assembler/parser.cpp, src/assembler/assembler.cpp, src/assembler/symbols.hpp (likely new file)
- Goal: Add .namespace/.endnamespace and local scope labels (.label) with qualified name resolution.
- Constraints: Must support namespace nesting. Local labels must be accessible only within parent scope.
- Definition of Done: Namespaced labels resolve correctly, tests pass.
- Suggested Tests: tests/directives.test.asm expansion, namespace resolution edge cases.
- Risk Level: Medium

## TASK-044
- [ ] Title: Implement inline data directives (.hex, .bin)
- Category: Assembler - Data
- Priority: Low
- Estimated Complexity: 1
- Files Required: src/assembler/parser.cpp, src/assembler/assembler.cpp
- Goal: Add .hex and .bin directives for inline hex and binary data insertion.
- Constraints: Must validate input format. Hex strings must have even length.
- Definition of Done: .hex and .bin insert correct bytes, tests pass.
- Suggested Tests: tests/data.test.asm expansion.
- Risk Level: Low

## TASK-045
- [ ] Title: Implement debug directives
- Category: Assembler - Directives
- Priority: Low
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/parser.cpp, src/debug/debug_handler.hpp
- Goal: Add .debug_start, .debug_info, .debug_breakpoint, .debug_watch, .debug_end directives.
- Constraints: Debug directives must be no-ops when debug mode is disabled.
- Definition of Done: Debug directives emit debug events when enabled, ignored when disabled, tests pass.
- Suggested Tests: tests/directives.test.asm expansion, debug output validation.
- Risk Level: Low

### Error Handling & Validation

## TASK-046
- [ ] Title: Ensure assembler rejects undefined symbol references
- Category: Assembler - Validation
- Priority: High
- Estimated Complexity: 1
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp
- Goal: Detect and report undefined symbol references during assembly.
- Constraints: Error message must include symbol name and location.
- Definition of Done: Undefined symbols cause assembly failure with clear message, tests pass.
- Suggested Tests: tests/errors.test.asm expansion.
- Risk Level: Low

## TASK-047
- [ ] Title: Add error recovery for unterminated string literals in assembler
- Category: Assembler - Validation
- Priority: High
- Estimated Complexity: 1
- Files Required: src/assembler/lexer.cpp, src/assembler/parser.cpp, src/main.cpp
- Goal: Ensure assembler reports and recovers from unterminated string literals.
- Constraints: No crash, clear error message with file/line.
- Definition of Done: Unterminated strings reported, assembly continues for further errors, tests pass.
- Suggested Tests: tests/errors.test.asm expansion.
- Risk Level: Low

## TASK-048
- [ ] Title: Add test for assembler macro expansion edge cases
- Category: Assembler - Macros
- Priority: Medium
- Estimated Complexity: 2
- Files Required: tests/preprocessor.test.asm, tests/preprocessor_advanced.test.asm, src/assembler/preprocessor.cpp
- Goal: Ensure macro system handles nesting, recursion limits, and parameter edge cases.
- Constraints: New tests only; no changes to preprocessor core unless bugs found.
- Definition of Done: All edge case tests pass.
- Suggested Tests: Nested macros, empty parameters, default values, recursion limit.
- Risk Level: Low

## TASK-049
- [ ] Title: Add test for assembler data definition edge cases
- Category: Assembler - Data
- Priority: Low
- Estimated Complexity: 1
- Files Required: tests/data.test.asm, tests/directives.test.asm
- Goal: Ensure assembler handles large arrays, alignment, and complex data definitions.
- Constraints: Test-only.
- Definition of Done: All edge case tests pass.
- Suggested Tests: Large .db arrays, mixed data types, alignment scenarios.
- Risk Level: Low

---

# Phase 5: Program Structure & Native Integration

## TASK-050
- [ ] Title: Implement section directives (.text, .data, .bss, .rodata)
- Category: Assembler - Structure
- Priority: High
- Estimated Complexity: 3
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp, src/assembler/parser.cpp, src/engine/init.cpp
- Goal: Add section directive support in assembler and map sections to separate VM memory regions.
- Constraints: Default section is .text. Sections must be non-overlapping in memory.
- Definition of Done: Programs with section directives assemble correctly, memory layout has proper separation, tests pass.
- Suggested Tests: tests/structure.test.asm, tests/entry_point.test.asm expansion.
- Risk Level: High
- Engineering Notes: This is the critical foundation for proper memory section architecture. See roadmap "Memory Section Architecture" section.

## TASK-051
- [ ] Title: Implement global/export symbol directive
- Category: Assembler - Structure
- Priority: High
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/assembler/assembler.hpp, src/assembler/parser.cpp
- Goal: Add .global directive to mark symbols as exported/visible to linker.
- Constraints: Must track symbol visibility in symbol table.
- Definition of Done: Global symbols resolved correctly, non-global symbols private to module, tests pass.
- Suggested Tests: tests/structure.test.asm expansion, symbol visibility tests.
- Risk Level: Medium

## TASK-052
- [ ] Title: Implement _start entry point resolution
- Category: Loader - VM
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/init.cpp, src/engine/cpu.cpp, src/engine/engine_api.cpp
- Goal: If `_start` label is defined in program, use it as the VM entry point instead of bytecode offset 0.
- Constraints: Must fall back to bytecode offset 0 if _start not defined (backward compatible).
- Definition of Done: Programs with `_start` execute correctly from entry point, programs without work as before, tests pass.
- Suggested Tests: tests/entry_point.test.asm expansion, _start defined and undefined cases.
- Risk Level: Medium
- Engineering Notes: Works with TASK-050 section support.

## TASK-053
- [ ] Title: Implement memory protection flags in VM
- Category: VM - Memory
- Priority: High
- Estimated Complexity: 3
- Files Required: src/engine/cpu.cpp, src/engine/cpu.hpp, src/engine/init.cpp
- Goal: Enforce memory protection (R/W/X) per memory region, preventing code execution from data sections.
- Constraints: Backward compatible for programs without section directives (all memory RWX).
- Definition of Done: Code in .text executes, code in .data/.bss/.rodata causes fault, tests pass.
- Suggested Tests: Execute from each section type, verify correct fault behavior.
- Risk Level: High
- Engineering Notes: Requires section-aware memory layout from TASK-050.

---

# Phase 6: Embedding API & Bindings

The C and C++ embedding API is functional with 7 example projects. These tasks harden, extend, and add language bindings.

## TASK-054
- [ ] Title: Review and harden embedding API security boundaries
- Category: Embedding API - Security
- Priority: High
- Estimated Complexity: 2
- Files Required: include/demi/engine.hpp, include/demi/engine_c_api.h, src/engine/engine_api.cpp, src/engine/engine_c_api.cpp, src/engine/sandbox_policy.hpp, src/engine/vfs.hpp, docs/usage/EMBEDDING_API.md
- Goal: Audit and strengthen all FFI, syscall, sandbox, and VFS boundaries.
- Constraints: No changes to public API signatures without approval.
- Definition of Done: Security review complete, critical issues addressed, tests pass.
- Suggested Tests: Fuzzing, boundary tests, sandbox escape attempts, path traversal tests.
- Risk Level: High

## TASK-055
- [ ] Title: Implement resource quota enforcement for embedded VMs
- Category: Embedding API - Safety
- Priority: High
- Estimated Complexity: 2
- Files Required: include/demi/engine.hpp, include/demi/engine_c_api.h, src/engine/engine_api.cpp, src/engine/engine.hpp, docs/usage/EMBEDDING_API.md
- Goal: Enforce granular quotas for memory, execution ticks, file handles, and syscalls.
- Constraints: Must not break existing embedding API. Quotas must be configurable.
- Definition of Done: Quotas enforced, violations reported via ErrorHandler, tests pass.
- Suggested Tests: Quota exhaustion, denial-of-service scenarios, normal operation.
- Risk Level: Medium

## TASK-056
- [ ] Title: Expand syscall interception and emulation
- Category: Embedding API - Integration
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/engine_api.cpp, src/engine/engine.hpp, src/engine/syscalls.hpp, include/demi/engine.hpp, docs/codebase/SYSCALLS_IMPLEMENTATION.md, docs/usage/EMBEDDING_API.md
- Goal: Allow richer host/guest interaction by expanding syscall interception hooks.
- Constraints: No performance regression. All changes must be tested.
- Definition of Done: Host can intercept, emulate, or block all syscalls, tests pass.
- Suggested Tests: Custom syscall hooks, error injection, policy enforcement.
- Risk Level: Medium

## TASK-057
- [ ] Title: Add asynchronous execution and cancellation support
- Category: Embedding API - API
- Priority: Medium
- Estimated Complexity: 3
- Files Required: include/demi/engine.hpp, include/demi/engine_c_api.h, src/engine/engine_api.cpp, src/engine/engine.hpp, docs/usage/EMBEDDING_API.md
- Goal: Enable async execution and safe cancellation of running guest code.
- Constraints: Must not break synchronous API. Thread-safe.
- Definition of Done: Async API available, cancellation works, all tests pass.
- Suggested Tests: Async run/cancel, resource cleanup, race conditions.
- Risk Level: High

## TASK-058
- [ ] Title: Implement Node.js FFI binding for embedding API
- Category: Embedding API - Bindings
- Priority: Medium
- Estimated Complexity: 3
- Files Required: (new) bindings/nodejs/, include/demi/engine_c_api.h, docs/usage/EMBEDDING_API.md
- Goal: Provide official Node.js binding using N-API for DemiEngine embedding.
- Constraints: Must not break C API. Must be documented and tested.
- Definition of Done: Node.js binding compiles, loads, and runs DemiEngine programs.
- Suggested Tests: Example Node.js integration, error handling, resource limits.
- Risk Level: Medium
- Engineering Notes: bindings/nodejs/ directory does not exist yet; needs creation.

## TASK-059
- [ ] Title: Implement Rust binding for embedding API
- Category: Embedding API - Bindings
- Priority: Medium
- Estimated Complexity: 3
- Files Required: (new) bindings/rust/, include/demi/engine_c_api.h, docs/usage/EMBEDDING_API.md
- Goal: Provide official Rust binding using bindgen and C API.
- Constraints: Must not break C API. Must be documented and tested.
- Definition of Done: Rust binding compiles, loads, and runs DemiEngine programs.
- Suggested Tests: Example Rust integration, error handling, FFI edge cases.
- Risk Level: Medium
- Engineering Notes: bindings/rust/ directory does not exist yet; needs creation.

## TASK-060
- [ ] Title: Unify and enhance error reporting and diagnostics
- Category: Embedding API - DX
- Priority: Medium
- Estimated Complexity: 2
- Files Required: include/demi/engine.hpp, include/demi/engine_c_api.h, src/engine/engine_api.cpp, src/engine/engine_c_api.cpp, src/debug/error_handler.hpp, docs/usage/EMBEDDING_API.md
- Goal: Provide unified error codes, stack traces, and diagnostics across C/C++/C API.
- Constraints: No breaking changes to existing error codes.
- Definition of Done: All APIs return clear, actionable errors, tests pass.
- Suggested Tests: Error injection, invalid input, edge cases.
- Risk Level: Low

## TASK-061
- [ ] Title: Refactor callback and hook ergonomics
- Category: Embedding API - DX
- Priority: Medium
- Estimated Complexity: 2
- Files Required: include/demi/engine.hpp, include/demi/engine_c_api.h, src/engine/engine_api.cpp, examples/embedding/*/main.cpp, docs/usage/EMBEDDING_API.md
- Goal: Improve callback lifetime, thread safety, and user data handling.
- Constraints: No breaking changes to existing hooks without approval.
- Definition of Done: All hooks safe, ergonomic, well-documented, tests pass.
- Suggested Tests: Lifetime tests, multi-threaded scenarios, user data edge cases.
- Risk Level: Medium

## TASK-062
- [ ] Title: Expand embedding documentation and examples
- Category: Documentation
- Priority: Medium
- Estimated Complexity: 1
- Files Required: docs/usage/EMBEDDING_API.md, examples/embedding/*/README.md
- Goal: Add more language examples, step-by-step guides, and usage notes.
- Constraints: Documentation must match implementation.
- Definition of Done: Docs and examples complete and reviewed.
- Risk Level: Low

## TASK-063
- [ ] Title: Prepare for official package distribution
- Category: Tooling
- Priority: Low
- Estimated Complexity: 2
- Files Required: Makefile, build scripts, docs/usage/EMBEDDING_API.md
- Goal: Lay groundwork for future official distribution (PyPI, npm, NuGet, etc.).
- Constraints: No public release until API is stable.
- Definition of Done: Packaging infrastructure in place, but not published.
- Risk Level: Low

---

# Phase 7: Testing & Quality Assurance

## Test Expansion Tasks

## TASK-064
- [ ] Title: Add test for invalid bytecode headers
- Category: VM - Testing
- Priority: High
- Estimated Complexity: 1
- Files Required: src/test/unit_tests.cpp, src/engine/init.cpp
- Goal: Ensure VM rejects bytecode files with invalid or corrupted headers.
- Constraints: No changes to VM core logic. Test-only.
- Definition of Done: VM rejects invalid headers, accepts valid ones.
- Suggested Tests: Corrupt header bytes, missing magic, wrong version.
- Risk Level: Low

## TASK-065
- [ ] Title: Add test for invalid tokens in lexer
- Category: Lexer - Testing
- Priority: High
- Estimated Complexity: 1
- Files Required: src/test/test_assembler.cpp, src/assembler/lexer.cpp
- Goal: Ensure lexer rejects invalid or unknown tokens.
- Constraints: Test-only.
- Definition of Done: Invalid tokens cause lexer error, valid tokens pass.
- Suggested Tests: Unknown characters, invalid sequences.
- Risk Level: Low

## TASK-066
- [ ] Title: Add test for FPU instruction edge cases
- Category: Testing - FPU
- Priority: High
- Estimated Complexity: 2
- Files Required: tests/fpu.test.asm
- Goal: Ensure FPU instructions handle division by zero, NaN, infinity, underflow.
- Constraints: Test-only.
- Definition of Done: All FPU edge cases pass.
- Suggested Tests: fdiv by zero, fsqrt negative, overflow, NaN propagation.
- Risk Level: Medium

## TASK-067
- [ ] Title: Add test for SIMD instruction edge cases
- Category: Testing - SIMD
- Priority: High
- Estimated Complexity: 2
- Files Required: tests/simd.test.asm
- Goal: Ensure SIMD instructions handle overflow, all-zero, max-value vectors.
- Constraints: Test-only.
- Definition of Done: All SIMD edge case tests pass.
- Suggested Tests: tests/simd.test.asm expansion for VADD, VMUL, VDOT, VMAX overflow/underflow.
- Risk Level: Medium

## TASK-068
- [ ] Title: Add test for 64-bit opcode edge cases
- Category: Testing - 64bit
- Priority: High
- Estimated Complexity: 2
- Files Required: tests/64bit/combined.test.asm, tests/opcodes/add64.test.asm, tests/opcodes/div64.test.asm, tests/opcodes/mul64.test.asm, tests/opcodes/sub64.test.asm
- Goal: Ensure 64-bit opcodes handle edge cases: overflow, large immediates, boundary values.
- Constraints: Test-only.
- Definition of Done: All 64-bit edge case tests pass.
- Suggested Tests: 64-bit overflow, max/min values, mixed 32/64-bit operand combinations.
- Risk Level: Low

## TASK-069
- [ ] Title: Add test for x86/x64 mode switching edge cases
- Category: Testing - Platform
- Priority: Medium
- Estimated Complexity: 2
- Files Required: tests/mode_switching.test.asm
- Goal: Ensure VM handles mode switching correctly in all edge cases.
- Constraints: Test-only.
- Definition of Done: All mode switching tests pass.
- Suggested Tests: Repeated mode switches, code at mode boundaries, mixed register usage.
- Risk Level: Low

## TASK-070
- [ ] Title: Add test for assembler ambiguous grammar cases
- Category: Testing - Assembler
- Priority: Medium
- Estimated Complexity: 1
- Files Required: tests/opcode_operands.test.asm, tests/errors.test.asm
- Goal: Ensure assembler handles ambiguous operand forms correctly.
- Constraints: Test-only.
- Definition of Done: All ambiguous cases parse correctly.
- Suggested Tests: tests/opcode_operands.test.asm expansion for all ambiguous forms.
- Risk Level: Low

## TASK-071
- [ ] Title: Add test for runtime memory leak detection
- Category: Testing - VM
- Priority: Medium
- Estimated Complexity: 1
- Files Required: src/test/unit_tests.cpp, src/engine/cpu.cpp
- Goal: Create test that triggers and detects memory leaks.
- Constraints: Test-only.
- Definition of Done: Leak detection test passes.
- Risk Level: Low

## TASK-072
- [ ] Title: Add test for opcode injection attacks
- Category: Testing - Security
- Priority: High
- Estimated Complexity: 2
- Files Required: src/test/unit_tests.cpp, src/engine/opcodes/opcodes_consolidated.cpp
- Goal: Ensure VM rejects opcode injection attacks (invalid opcodes embedded in bytecode).
- Constraints: Test-only.
- Definition of Done: VM rejects injected opcodes with appropriate error.
- Suggested Tests: Malicious bytecode, out-of-range opcodes.
- Risk Level: Medium

## TASK-073
- [ ] Title: Add test for path traversal protection
- Category: Testing - Security
- Priority: High
- Estimated Complexity: 2
- Files Required: src/engine/devices/file_device.hpp, src/test/unit_tests.cpp
- Goal: Ensure FileDevice prevents path traversal attacks.
- Constraints: Test-only.
- Definition of Done: Path traversal attempts rejected.
- Suggested Tests: ../, absolute paths, symlink attacks.
- Risk Level: Medium

## TASK-074
- [ ] Title: Diagnose and fix org_and_db_integration test output anomaly
- Category: Assembler - Bug
- Priority: High
- Estimated Complexity: 2
- Files Required: src/assembler/assembler.cpp, src/engine/opcodes/opcodes_consolidated.cpp, src/test/unit_tests.cpp
- Goal: Investigate and resolve persistent output bug where "First stringSecond stringecond string" instead of expected.
- Constraints: No regression in assembler or VM behavior.
- Definition of Done: Test outputs correct strings, memory layout validated.
- Suggested Tests: org_and_db_integration, memory inspection, OUTSTR validation.
- Risk Level: Medium

---

# Platform Support

## TASK-075
- [ ] Title: Add stub for ARM64 architecture detection
- Category: Platform
- Priority: Low
- Estimated Complexity: 1
- Files Required: src/main.cpp, src/engine/system_info.hpp
- Goal: Add stub detection for ARM64 architecture.
- Constraints: No ARM64 backend implementation. Stub only.
- Definition of Done: ARM64 detection code present, no regressions.
- Suggested Tests: Run on ARM64 and x86-64 systems.
- Risk Level: Low

---

# Documentation

## TASK-076
- [ ] Title: Update documentation for new assembler directives
- Category: Documentation
- Priority: Medium
- Estimated Complexity: 1
- Files Required: docs/reference/QUICK_REFERENCE.md, docs/development/FUTURE_FEATURES_ROADMAP.md
- Goal: Document all new assembler directives and features as they are implemented.
- Constraints: Documentation must match implementation.
- Definition of Done: All new directives documented.
- Recommended: Review against codebase after each Phase 3/4 feature.
- Risk Level: Low

## TASK-077
- [ ] Title: Document error codes in ERROR_CODES guide
- Category: Documentation
- Priority: Low
- Estimated Complexity: 1
- Files Required: docs/development/ERROR_HANDLING_IMPLEMENTATION.md, docs/testing/TEST_FLAGS.md
- Goal: Ensure all error codes are documented with clear descriptions.
- Constraints: Documentation must match implementation.
- Definition of Done: All error codes documented.
- Risk Level: Low

---

# End of Backlog
