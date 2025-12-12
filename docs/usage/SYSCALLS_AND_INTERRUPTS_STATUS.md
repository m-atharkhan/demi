# DemiEngine: Syscalls & Interrupts Status (as of 2025-11-26)

## Syscalls

- **Real Linux syscalls are supported via INT 0x80**
  - DemiEngine now calls the actual Linux kernel `syscall()` for supported numbers
  - Supported: `sys_read`, `sys_write`, `sys_open`, `sys_close`, `sys_exit`, `sys_ioctl`, `sys_brk` (simulated)
  - VM memory addresses are translated to host pointers for buffer/filename arguments
  - Error codes from Linux are returned in RAX (negative for errors)
  - Security: VM can access host files with same permissions as demi-engine process
  - All syscalls are dispatched via an efficient enum-based handler
  - **Limitation:** Only 8-bit immediates are supported for most instructions (e.g., LOAD_IMM), so flags/modes >255 require workarounds or new instructions (e.g., LOAD_IMM32/64)
  - Example programs in `examples/syscalls/` show real file I/O, stdout/stderr, etc.
  - Use `strace` to verify real kernel calls (e.g., `strace -e trace=write,open,close ./bin/demi-engine-debug -A examples/syscalls/01_hello_world.asm`)

## Interrupts

- **All major x86-style interrupt opcodes implemented:**
  - `CLI` (disable interrupts)
  - `STI` (enable interrupts)
  - `INT <vector>` (trigger interrupt)
  - `IRET` (return from interrupt)
- **Interrupt Vector Table (IVT):**
  - IVT is stored in VM memory, each entry is a 32-bit handler address
  - IVT setup now works thanks to STORE writing 32-bit values
  - INT triggers either a syscall (0x80) or a handler from IVT
  - IRET restores CPU state from stack
- **Syscall/Interrupt Integration:**
  - INT 0x80 triggers syscall handler (Linux conventions)
  - Other INT vectors use IVT lookup
  - All interrupt logic integrated into CPU execution loop

## Current Limitations & Next Steps

- **Immediate values:**
  - Most instructions only support 8-bit immediates (0-255)
  - Need to implement 32-bit and 64-bit versions for full syscall compatibility (e.g., LOAD_IMM32, STORE32, MOV32, etc.)
- **Syscall enum:**
  - All syscalls mapped to an enum for efficient dispatch
  - Easy to extend for more syscalls
- **Examples:**
  - All syscall/interrupt demos moved to `examples/syscalls/` and `examples/interrupts/`
  - No longer in `tests/` since they are not unit tests
- **File I/O:**
  - Real file creation/writing now works, but permission/mode issues may occur due to 8-bit immediate limits
  - Use chmod as workaround if needed
- **Documentation:**
  - See `docs/usage/REAL_SYSCALLS.md` and `docs/codebase/SYSCALLS_IMPLEMENTATION.md` for full details

## How to Resume Work

- Implement 32-bit/64-bit immediate instructions for all relevant opcodes
- Expand syscall handler to support more Linux syscalls
- Add more interrupt handler examples and IVT tests
- Use the enum-based syscall dispatch for new features
- Reference this file for context when continuing development on another machine

---

**Last updated:** 2025-11-26
