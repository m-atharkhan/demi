#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointer representing the Engine instance
typedef void* demi_engine_t;

// CompleteReason enum mappings
#define DEMI_FINISHED 0
#define DEMI_WATCHDOG_TIMEOUT 1
#define DEMI_SECURITY_VIOLATION 2
#define DEMI_EXCEPTION 3

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef DEMI_BUILD_SHARED
        #define DEMI_API __declspec(dllexport)
    #else
        #define DEMI_API
    #endif
#else
    #define DEMI_API __attribute__((visibility("default")))
#endif

// -----------------------------------------------------
// Instantiation
// -----------------------------------------------------

// Creates an engine with default (unsafe) host-level access
DEMI_API demi_engine_t demi_engine_create_default();

// Creates an engine with Strict Sandbox, VFS jail, and Watchdog limits active
DEMI_API demi_engine_t demi_engine_create_sandboxed(const char* jail_root_path, uint64_t max_execution_ticks);

// Destroys the engine instances and frees memory
DEMI_API void demi_engine_destroy(demi_engine_t engine);

// -----------------------------------------------------
// Execution & Memory
// -----------------------------------------------------

// Load bytecode into the CPU memory.
DEMI_API bool demi_engine_load_executable(demi_engine_t engine, const uint8_t* binary, size_t size, uint64_t base_address);

// Executes a single instruction
DEMI_API bool demi_engine_tick(demi_engine_t engine);

// Runs the CPU linearly until it hits a halt instruction, watchdog timeout, or security violation
DEMI_API int demi_engine_run(demi_engine_t engine);

// -----------------------------------------------------
// External Hooks
// -----------------------------------------------------

// Function pointer signature for memory interception (e.g., VRAM hooking)
typedef void (*demi_memory_hook_cb)(uint64_t addr, uint8_t value, void* user_data);

// Attach a memory write observer
DEMI_API void demi_engine_set_memory_hook(demi_engine_t engine, uint64_t start_addr, uint64_t end_addr, demi_memory_hook_cb hook, void* user_data);

// Syscall hook (return true to bypass default handling)
typedef bool (*demi_syscall_hook_cb)(uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t* result, void* user_data);
DEMI_API void demi_engine_set_syscall_hook(demi_engine_t engine, demi_syscall_hook_cb hook, void* user_data);

// Stdio hooks
typedef void (*demi_stdout_hook_cb)(int fd, const uint8_t* data, size_t size, void* user_data);
DEMI_API void demi_engine_set_stdout_hook(demi_engine_t engine, demi_stdout_hook_cb hook, void* user_data);

typedef void (*demi_stdin_hook_cb)(size_t max_count, uint8_t* data, size_t* out_size, void* user_data);
DEMI_API void demi_engine_set_stdin_hook(demi_engine_t engine, demi_stdin_hook_cb hook, void* user_data);

#ifdef __cplusplus
}
#endif
