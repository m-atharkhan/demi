# Demi Engine v1.0 — C Engine API + Native Compilation

**Release:** 2026-06-09 | **Branch:** `C-Engine-API` → `main`  
**Commits:** 273 | **Files changed:** 150 | **Delta:** +23,220 / -11,671

---

This is the largest update in Demi's history. It ships the C embedding API, native
x86-64 code compilation, a full sandbox with persistent VirtualDisk-backed
filesystem jail, 8 new syscalls, and comprehensive security hardening.

---

## C Engine API (`libdemi`)

Demi can now be embedded as a library in host applications. The C API exposes 30+
functions for engine lifecycle, memory management, CPU introspection, I/O hooking,
and VirtualDisk interaction.

**Header:** `include/demi/engine_c_api.h`

```c
demi_config_t cfg = demi_engine_default_config();
cfg.enable_sandbox = true;
demi_engine_t vm = demi_engine_create(&cfg);
demi_engine_load_executable(vm, bytecode, size, 0);
demi_engine_run(vm);
demi_engine_destroy(vm);
```

**New files:** `src/engine/engine_c_api.cpp` (623 lines), `include/demi/engine_c_api.h` (127 lines)  
**Docs:** `docs/usage/EMBEDDING_API.md`

---

## Native x86-64 Code Compilation

DASM bytecode can now be compiled to native x86-64 ELF executables with `-o`:

```bash
demi-engine -o my_program -H program.hex    # bytecode → native ELF
./my_program                                 # runs natively, no VM overhead
```

**Pipeline:** DASM bytecode → `DISAToX86Compiler` → `X86Encoder` → `ELFEmitter` → native ELF

**New files:**
- `src/codegen/disa_compiler.cpp/.hpp` — 1,299 lines, VM-to-native translation
- `src/codegen/x86_encoder.cpp/.hpp` — 486 lines, x86 opcode emission
- `src/codegen/register_allocator.hpp` — VM register → x86 register mapping
- `src/codegen/elf_emitter.cpp` — ELF header/program header generation

**Supported:** Arithmetic, logic, control flow, I/O, FPU, SIMD, mode-aware 32/64-bit

---

## Sandbox + VirtualDisk Filesystem

Opt-in security boundary isolates guest bytecode from the host.

```bash
demi-engine --sandbox -H program.hex                     # basic jail
demi-engine --sandbox --allow-read --allow-write -H p.hex  # granular access
demi-engine --sandbox --vfs-container disk.vfs -H p.hex    # persistent storage
```

**Capability flags:** `--allow-read`, `--allow-write`, `--allow-exec`, `--allow-ioctl`

**VirtualDisk (.vfs):** Binary container format at `/tmp/demi_sandbox.vfs`.
Persists across VM runs. Host-side API for reading, writing, listing, and deleting
files programmatically.

**New files:**
- `src/engine/sandbox_policy.hpp` — syscall allow/deny decision engine
- `src/engine/vfs.hpp` — VirtualFileSystem path jail with `io_root_path`
- `src/engine/virtual_disk.cpp/.hpp` — 1,249 lines, .vfs container format
- `tests/sandbox_e2e.test.asm` — 71-line end-to-end sandbox test
- `tests/vdisk_integration.test.asm` — VirtualDisk integration test

---

## Syscall Expansion

8 new Linux syscalls wired through the INT 0x80 gateway, all gated behind
sandbox capability flags:

| Syscall | Number | Description | Sandbox Gate |
|---|---|---|---|
| `SYS_LSEEK` | 8 | File seek | `--allow-read` or `--allow-write` |
| `SYS_STAT` | 106 | File metadata | `--allow-read` |
| `SYS_FSTAT` | 108 | Fd metadata | `--allow-read` |
| `SYS_ACCESS` | 33 | File accessibility | `--allow-read` |
| `SYS_GETCWD` | 183 | Current directory | `--allow-read` |
| `SYS_GETDENTS` | 220 | Directory listing | `--allow-read` |
| `SYS_READLINK` | 89 | Symlink target | `--allow-read` |
| `SYS_UNLINK` | 10 | Delete file | `--allow-write` |

Total mapped syscalls: 13 (up from 5). See `docs/usage/SYSCALLS_AND_INTERRUPTS_STATUS.md`.

---

## Security Hardening

### Codacy cleanup: 884 → 88 issues

| Metric | Before | After |
|---|---|---|
| Total issues | 884 | ~88 |
| Security (Critical) | 43 | 0 |
| High severity | 49 | ~3 (stale) |

**Key changes:**
- `safe_bitcast<T>()` — zero-overhead type-punning with `static_assert` size verification (`src/engine/safe_memcpy.hpp`)
- 43 Security issues resolved: memcpy bounds, strcpy→snprintf, strlen guards, device API safety
- `reinterpret_cast` UB replaced with safe memcpy across FPU, SIMD, and test code
- 70+ explicit constructors added across AST, CPU, and test headers
- `find()==0` → `compare(0, n, s)==0` for all C++17 `starts_with()` equivalents
- `max_execution_ticks` default set to 5,000,000 (was unlimited) — prevents DoS via infinite loops

### Penetration test: 10/10 tests pass

Full report at `notes/demi/pentest-c-engine-api.md`. Findings:
- Memory bounds enforced (OOB read/write blocked)
- Sandbox prevents execve, socket, and unauthorized file I/O
- C API validates all null/invalid pointers
- VirtualDisk API blocks path traversal
- Watchdog terminates infinite loops at 5M ticks

### Memory bounds in all builds

`validate_memory_write()` is now unconditional — was previously `#ifndef NDEBUG`
only. All builds enforce memory safety.

---

## Code Quality

- **273 commits** of cleanup, refactoring, and hardening
- `encode_instruction` 1,200-line if-else chain → map-based opcode dispatch
- 3 duplicate assemble pipeline methods → single `assemble_internal()`
- `fpu_store(0, result)` in 6 unary FPU ops (was `fpu_pop` + `fpu_push`)
- `DEBUG_*` macros guarded behind `Config::debug` check
- Removed AI-generated slop comments, Doxygen noise, redundant sections
- `.codacy.yml` fully configured with metric exclusions and flawfinder rules

---

## CLI Additions

```
--sandbox                  Enable sandboxed execution
--allow-read               Grant host filesystem read access
--allow-write              Grant host filesystem write access
--allow-exec               Grant process execution (fork/execve)
--allow-ioctl              Grant raw ioctl access
--vfs-container <path>     Use persistent .vfs disk image
--hexdump                  Enable bytecode hex dump after assembly
-ao <file>                 Output assembled bytecode to file
-hxo <file>                Output as formatted hex bytes
```

---

## New Tests

| Type | Count | Notes |
|---|---|---|
| Unit tests | 283 | +30 encoder map dispatch, +14 VFS/VirtualDisk, +6 sandbox policy |
| Assembly tests | 548 | +71 sandbox E2E, +58 vdisk integration, +128 stack |
| Penetration tests | 10 | C API exploit harness (`scripts/pentest_c_api.py`) |

---

## Breaking Changes

- `max_execution_ticks` defaults to 5,000,000 (was 0 = unlimited). Trusted workloads should set to 0 explicitly.
- Sandbox mode is now fully wired — `--sandbox` without flags blocks all host I/O.
- `SYS_OPEN` strips `O_CREAT`/`O_TRUNC`/`O_APPEND` — only existing files can be opened.
- `read_mem32`/`write_mem32` require 4-byte aligned addresses.

---

## Contributors

bobrossrtx — architecture, engine, sandbox, VFS, native compiler, C API, tests  
Hermes Agent — security audit, Codacy cleanup, penetration test, safe_bitcast, docs
