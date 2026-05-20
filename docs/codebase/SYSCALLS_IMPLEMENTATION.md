# Real Linux Syscalls Implementation

## Summary

DemiEngine now invokes **actual Linux kernel syscalls** instead of simulating them. When assembly code executes `INT 0x80`, the VM calls the real `syscall()` function from `<unistd.h>`, passing requests directly to the Linux kernel.

## What Changed

### Before

- `sys_write` was simulated by C++ code reading VM memory and using `std::cout`
- Only stdout/stderr were supported
- File operations were not possible
- Return values were fabricated

### After

```cpp
case 4: // sys_write
    if (arg2 < memory.size() && arg2 + arg3 <= memory.size()) {
        result = syscall(SYS_write, arg1, &memory[arg2], arg3);
        // ^^^ Real kernel syscall, not simulation!
    }
```

## Verification

Run any syscall test with `strace` to see real kernel calls:

```bash
$ strace -e trace=write ./bin/demi-engine-debug -A tests/syscall_hello.test.asm
write(1, "Hello, World!\n", 14)         = 14
```

The `write(1, "Hello, World!\n", 14) = 14` is a **real Linux kernel syscall**, not simulated!

## Supported Syscalls

| Number | Name      | Status         | Notes                          |
| ------ | --------- | -------------- | ------------------------------ |
| 1      | sys_exit  | ✅ Real        | Stops VM only, not host        |
| 3      | sys_read  | ✅ Real        | Reads into VM memory           |
| 4      | sys_write | ✅ Real        | Writes from VM memory          |
| 5      | sys_open  | ✅ Real        | Opens real files               |
| 6      | sys_close | ✅ Real        | Closes file descriptors        |
| 45     | sys_brk   | ⚠️ Simulated   | Returns current VM memory size |
| 54     | sys_ioctl | ✅ Real        | Pass-through to kernel         |
| 90     | sys_mmap  | ❌ Unsupported | Returns -ENOSYS                |
| 192    | sys_mmap2 | ❌ Unsupported | Returns -ENOSYS                |

## Examples

### Write to stdout

```asm
LOAD_IMM R0, 4      ; sys_write
LOAD_IMM R3, 1      ; stdout
LOAD_IMM R1, msg    ; buffer
LOAD_IMM R2, 13     ; length
INT 0x80            ; REAL syscall!
```

### Write to stderr

```asm
LOAD_IMM R0, 4      ; sys_write
LOAD_IMM R3, 2      ; stderr (note: fd=2)
LOAD_IMM R1, error
LOAD_IMM R2, 20
INT 0x80
```

### Close file descriptor

```asm
LOAD_IMM R0, 6      ; sys_close
LOAD_IMM R3, 100    ; fd (will fail with EBADF if invalid)
INT 0x80
; R0 now contains: 0 (success) or -9 (EBADF)
```

## Architecture

```
Assembly Code (INT 0x80)
         ↓
CPU::handle_pending_interrupts()
         ↓
CPU::handle_syscall()
         ↓
syscall(SYS_xxx, ...)  ← Linux kernel function
         ↓
Linux Kernel
         ↓
Real I/O / Real Files / Real Devices
```

## Memory Translation

VM addresses are automatically translated:

- **VM address**: `0x0000` to `memory.size()-1`
- **Host pointer**: `&memory[vm_address]`
- **Validation**: Bounds checking prevents out-of-bounds access

```cpp
// arg2 is VM address from register
if (arg2 < memory.size() && arg2 + arg3 <= memory.size()) {
    result = syscall(SYS_write, arg1, &memory[arg2], arg3);
    //                                 ^^^^^^^^^^^^^^^^
    //                                 VM address → Host pointer
}
```

## Error Codes

All Linux error codes are returned in R0:

- **EBADF** (-9): Bad file descriptor
- **EFAULT** (-14): Bad address (out of bounds)
- **ENOSYS** (-38): Function not implemented

## Security

### What's Protected

- ✅ VM memory isolation (can't access host memory)
- ✅ Automatic bounds checking
- ✅ sys_exit stops VM, not host process

### What's NOT Protected

- ⚠️ **Without `--sandbox`**, VM can access host filesystem (same permissions as demi-engine process)
- ⚠️ **Without `--sandbox`**, VM can write to any writable file
- ⚠️ **Without `--sandbox`**, VM can read any readable file

**Security Model**: Without explicit configuration, the VM has the same file access permissions as the demi-engine binary. 
If you need isolation, you must run it with the `--sandbox` and `--strict-io` flags, use `libdemi`, or run in a restricted container.

## Subsystem: Sandbox Syscall Interception

With the introduction of the Embedding API (`libdemi`), syscall dispatching is routed through the `SyscallDispatcher::validate()` gateway when `--sandbox` is enabled.

When `--sandbox` is active:
- Network and execution syscalls (`execve`, `socket`) result in a hardware `INT_SECURITY_FAULT`.
- File operations (`open`, `read`, `write`) are intercepted and diverted to the Jailed VirtualFileSystem (VFS).
- Strict bounds are maintained by preventing directory traversals.

## Test Files

All in `tests/` directory:

- `syscall_hello.test.asm` - Basic stdout write
- `syscall_demo.test.asm` - Multiple syscall examples
- Run with: `./bin/demi-engine-debug -A tests/syscall_demo.test.asm`
- Verify with: `strace -e trace=write,read,open,close ./bin/demi-engine-debug -A <test>`

## Future Enhancements

Potential additions:

- [ ] File creation/deletion (unlink, mkdir, rmdir)
- [ ] File seeking (lseek)
- [ ] File status (stat, fstat)
- [ ] Socket operations (socket, connect, send, recv)
- [ ] Process operations (fork, execve - complex!)
- [ ] Signal handling
