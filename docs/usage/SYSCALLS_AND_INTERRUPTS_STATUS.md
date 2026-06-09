# DemiEngine: Syscalls & Interrupts Status

## Syscalls

- **Real Linux syscalls are supported via INT 0x80**
  - DemiEngine calls the actual Linux kernel for supported syscall numbers
  - Supported: sys_read, sys_write, sys_open, sys_close, sys_exit, sys_fork, sys_execve, sys_ioctl, sys_brk (simulated)
  - VM memory addresses are translated to host pointers for buffer/filename arguments
  - Error codes follow Linux convention (-errno returned in RAX)
  - All syscalls dispatched via an efficient enum-based handler

### Sandbox Mode (`--sandbox`)

The `--sandbox` flag enables a Virtual FileSystem jail that restricts guest
program access:

```
demi-engine --sandbox -A program.asm              # VFS jail only
demi-engine --sandbox --allow-write -A tool.asm   # + real file writes
demi-engine --sandbox --allow-read -A tool.asm    # + real file reads
demi-engine --sandbox --allow-exec -A build.asm   # + fork+execve
```

**Default behavior (no flags):** Full host access — the VM operates with the
same filesystem permissions as the demi-engine process.

**Sandbox behavior (`--sandbox`):**
- File I/O is routed through the VirtualFileSystem (VFS jail at /tmp/demi_vfs)
- Process execution (fork/execve) is denied
- Network (socket) is denied
- SYS_EXIT always works
- Unknown syscalls default to denied

**Capability flags (with `--sandbox`):**
- `--allow-read` — bypass VFS for real filesystem reads
- `--allow-write` — bypass VFS for real filesystem writes
- `--allow-exec` — permit fork+execve in sandbox mode

Syscall return values use proper Linux -errno convention for error reporting.

## Interrupts

- **All major x86-style interrupt opcodes implemented:**
  - `CLI` (disable interrupts)
  - `STI` (enable interrupts)
  - `INT <vector>` (trigger interrupt)
  - `IRET` (return from interrupt)
- **Interrupt Vector Table (IVT):**
  - IVT stored in VM memory, each entry is a 32-bit handler address
  - INT triggers either a syscall (0x80) or a handler from IVT
  - IRET restores CPU state from stack
- **Syscall/Interrupt Integration:**
  - INT 0x80 triggers syscall handler (Linux conventions)
  - Other INT vectors use IVT lookup
  - All interrupt logic integrated into CPU execution loop

## Test Coverage

- 283 unit tests (including SyscallDispatcher policy and VFS integration tests)
- 546 assembly tests (539 passing, 7 skipped including sandbox-only tests)
- 4 end-to-end sandbox tests (run with `--sandbox -at tests/sandbox_e2e.test.asm`)
- Example programs in `examples/syscalls/` demonstrate real file I/O
