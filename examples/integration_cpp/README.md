# Demi C++ Integration API Example

This example demonstrates how to host the sandboxed Demi Virtual Machine within a parent C++ application using the clean `demi::Engine` C++ embedding API.

## Project Structure

- `main.cpp`: The parent application that instantiates the Demi VM, registers STDOUT and Syscall interception hooks, loads assembly bytecode, and runs/monitors execution.
- `hello.asm`: A simple 64-bit assembly script which outputs a greeting using `sys_write` and exits using `sys_exit`.
- `Makefile`: Automates building the project, generating static libraries, compiling the assembly logic, and executing the demo.

## Prerequisites

Ensure you have a modern C++17 compiler (`g++`) and GNU Make installed inside your WSL system.

## Building and Running the Example

Everything is automated within the local `Makefile`. You can run the entire workflow with a single command inside WSL:

```bash
make run
```

This command will:
1. Automatically verify and build the core static library (`libdemi-debug.a`) from the project root.
2. Assemble `hello.asm` to the raw binary bytecode representation `hello.hex` using `demi-engine-debug`.
3. Compile the local C++ host driver (`integration_demo`).
4. Execute `integration_demo` with the compiled bytecode vector.

To clean up build artifacts:

```bash
make clean
```

## How It Works

### 1. Security & Watchdog Sandbox
Parent applications configure execution security bounds strictly:
```cpp
demi::Config config;
config.enable_sandbox = true;         // Intercepts dangerous operations
config.strict_io = false;             // Redirect raw standard buffers using hooks
config.max_execution_ticks = 100000;  // watchdogs infinite loops from hogging Host threads
config.memory_size = 1024 * 64;       // Sandboxed 64KB heap
```

### 2. Standard I/O Interception
Stdout redirection allows intercepting print statements (`sys_write` to terminal) for custom logging systems or UI components:
```cpp
vm.set_stdout_hook([](int fd, const std::vector<uint8_t>& data) {
    std::string text(data.begin(), data.end());
    std::cout << "[VM STDOUT (fd=" << fd << ")] " << text;
});
```

### 3. Custom System Calls Hooking
Parent hosts can capture, alter, and emulate system queries or supply customized hardware device drivers:
```cpp
vm.set_syscall_hook([](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
    // Bypasses default handling when returning true
    return false; 
});
```
