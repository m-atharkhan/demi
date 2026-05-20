# Embedding DemiEngine (`libdemi`)

DemiEngine is designed to be a standalone, general-purpose Virtual Machine, but you can also safely host it inside parent applications to execute untrusted user code. A public Embedded API (`demi/engine.hpp`) is available for this purpose.

## Supported Languages and Bindings
Currently, the core engine SDK exposes a Clean C++ Header (`engine.hpp`) and, crucially, a **Pure C API Wrapper** (`engine_c_api.h`). 

You do **NOT** need to write JNI code directly inside the Demi Engine! By compiling `demi` into a shared library (`libdemi.so` / `libdemi.dll`), you can immediately bind it externally using FFI wrappers:

- **Java / JVM via JNA (Java Native Access)**: The recommended way to embed `demi-engine` into a **Minecraft Mod** or Java EE backend. You parse our C bindings straight from Java without any extra C++ middleware.
- **Python via CTypes**: Direct import of `libdemi.so`.
- **C# via P/Invoke**: Directly use DllImport on our exported C wrapper.
- **C/C++**: Native API for game engines or low-level integrations.

## Security Posture

When hosting DemiEngine, it is highly recommended to instantiate the configuration with both Sandbox and Strict I/O enabled to prevent Remote Code Execution (RCE) and Denial of Service (DoS):

```cpp
#include <demi/engine.hpp>

int main() {
    demi::Config config;
    config.enable_sandbox = true;
    config.strict_io = true;
    config.io_root_path = "/safe/vfs/directory"; // Jails file I/O
    config.max_execution_ticks = 100'000'000;    // Cap execution to prevent infinite loop DoS

    demi::Engine vm(config);
    // ... load binary and run ...
}
```

### Sandbox Protections
When `--sandbox` (or `enable_sandbox`) is enabled:
- **File I/O**: Intercepted and routed through a Virtual File System (VFS).
- **Directory Traversal**: Jailed strictly to `io_root_path`. Expressions like `../../etc/passwd` are safely resolved within the jail path.
- **Process and Network Syscalls**: Syscalls like `fork`, `execve`, or `socket` immediately return errors or halt the VM with a security fault.

### Watchdog Protection
When allowing untrusted assembly code, infinite loops (e.g. `loop: JMP loop`) can tie up the host thread. `max_execution_ticks` creates a quota system; if the VM executes more instructions than allowed, it throws a `WATCHDOG_TIMEOUT` and shuts down cleanly.

## Virtual Memory & VRAM Hooking

If you are embedding DummyEngine in a Game (e.g., Minecraft mod), you map memory regions manually and intercept writes to implement "VRAM" or Memory-Mapped I/O:

```cpp
vm.set_memory_write_hook(0xA000, 0xB000, [](uint64_t addr, uint8_t value) {
    // This callback happens sequentially whenever the VM writes to VRAM
    update_game_texture(addr - 0xA000, value);
});
```

## System Call & Standard I/O Interception

You can hook Linux-style syscalls (`INT 0x80`) and standard I/O (stdout, stdin) triggered by the VM. This is extremely powerful for redirecting print statements (`sys_write` to FD 1 or 2) directly into your parent application's logging or console windows, rather than the host's actual stdout.

### Standard I/O Hooks
```cpp
// Redirect stdout/stderr from the VM
vm.set_stdout_hook([](int fd, const std::vector<uint8_t>& data) {
    std::string text(data.begin(), data.end());
    my_game_engine_log(text);
});

// Provide stdin iteratively to the VM
vm.set_stdin_hook([](size_t max_count, std::vector<uint8_t>& data) {
    std::string input = get_user_console_input();
    data.assign(input.begin(), input.end());
});
```

### Full Syscall Hooking
If you need complete control over how an operating system is emulated within your sandbox, you can override system calls directly and optionally bypass the VM's built-in handlers:

```cpp
vm.set_syscall_hook([](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
    if (syscall_id == 60) { // sys_exit
        my_custom_exit_logger();
        result = 0;
        return true; // Bypass default DemiEngine processing
    }
    return false; // Let DemiEngine handle it (Sandbox/VFS logic)
});
```

Using the API ensures the complexities of internal subsystems (`cpu.hpp`, 134-registers, memory mappings) are cleanly decoupled from the embedding host.
