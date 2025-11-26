# System Call Implementation Plan

**Status**: Planning Phase
**Goal**: Implement `INT` opcode and Linux-style system calls (`int 0x80`)
**Target Date**: Immediate

---

## 1. Objective
Enable "traditional" assembly system calls using the `INT` instruction and register-based arguments, mimicking Linux x86 behavior.

**Example Usage:**
```asm
mov eax, 4      ; sys_write
mov ebx, 1      ; stdout
mov ecx, msg    ; buffer
mov edx, len    ; length
int 0x80        ; syscall
```

## 2. Implementation Steps

### Phase 1: Assembler Support
1.  **Register Aliases**:
    *   Update `src/assembler/assembler.cpp` to support 32-bit register names (`EAX`, `EBX`, `ECX`, `EDX`, `ESI`, `EDI`, `ESP`, `EBP`).
    *   Map them to the corresponding internal registers (likely `R0`-`R7` or `RAX`-`RBP`).
2.  **Opcode Definition**:
    *   Add `INT` (0xCD) to `Opcode` enum in `src/assembler/opcodes.hpp`.
    *   Add `INT` to `mnemonic_to_opcode` map in `src/assembler/assembler.cpp`.

### Phase 2: Engine Support
1.  **Opcode Handler**:
    *   Create `handle_int` in `src/engine/opcodes/system.hpp` (or similar).
    *   The handler must read one immediate byte (the interrupt vector).
    *   If vector is `0x80`, call the system call dispatcher.
2.  **Opcode Registration**:
    *   Register `0xCD` in `src/engine/opcodes/opcode_registry.cpp`.

### Phase 3: System Call Dispatcher
1.  **Syscall Module**:
    *   Create `src/engine/syscalls.hpp` and `src/engine/syscalls.cpp`.
    *   Define `SyscallHandler` class.
2.  **Dispatch Logic**:
    *   Read `RAX` (or `R0`) for syscall number.
    *   Read `RBX`, `RCX`, `RDX` for arguments.
    *   Switch on syscall number.
3.  **Implemented Syscalls**:
    *   `sys_exit` (1): Terminate program (set `running = false`).
    *   `sys_write` (4): Write buffer to file descriptor (support stdout/stderr).

### Phase 4: Testing
1.  **Test Case**:
    *   Create `tests/syscall_simple.asm` implementing the "Hello World" example.
    *   Verify output matches expected string.
    *   Verify exit code.

## 3. Technical Details

**Register Mapping (Proposed):**
| x86 32-bit | x86 64-bit | Internal |
|------------|------------|----------|
| EAX        | RAX        | R0       |
| EBX        | RBX        | R1       |
| ECX        | RCX        | R2       |
| EDX        | RDX        | R3       |
| ESI        | RSI        | R4       |
| EDI        | RDI        | R5       |
| ESP        | RSP        | R6       |
| EBP        | RBP        | R7       |

**Opcode Format:**
`INT <imm8>` -> `CD <imm8>` (2 bytes)

## 4. Future Expansion
- Support more syscalls (`sys_read`, `sys_open`, `sys_close`).
- Support custom interrupt vectors (if needed).
