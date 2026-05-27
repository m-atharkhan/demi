#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEMI_API_VERSION 1

typedef void* demi_engine_t;

typedef struct demi_config {
    bool enable_sandbox;
    bool strict_io;
    const char* io_root_path;
    uint64_t max_execution_ticks;
    size_t memory_size;
} demi_config_t;

typedef enum demi_result {
    DEMI_FINISHED = 0,
    DEMI_WATCHDOG_TIMEOUT = 1,
    DEMI_SECURITY_VIOLATION = 2,
    DEMI_EXCEPTION = 3
} demi_result_t;

enum demi_error_code {
    DEMI_ERR_OK = 0,
    DEMI_ERR_NULL_ENGINE = 1,
    DEMI_ERR_INVALID_BINARY = 2,
    DEMI_ERR_MEMORY_OOB = 3,
    DEMI_ERR_SANDBOX_VIOLATION = 4,
    DEMI_ERR_SYSCALL_FAILURE = 5,
    DEMI_ERR_MISSING_PROGRAM = 6,
    DEMI_ERR_INVALID_CONFIG = 7,
    DEMI_ERR_INVALID_REGISTER = 8,
    DEMI_ERR_UNKNOWN = 99
};

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef DEMI_BUILD_SHARED
        #define DEMI_API __declspec(dllexport)
    #else
        #define DEMI_API
    #endif
#else
    #define DEMI_API __attribute__((visibility("default")))
#endif

/* API versioning */
DEMI_API uint32_t demi_engine_api_version(void);

/* Config-based engine creation */
DEMI_API demi_config_t demi_engine_default_config(void);
DEMI_API demi_config_t demi_engine_sandboxed_config(const char* jail_root_path);
DEMI_API demi_engine_t demi_engine_create(const demi_config_t* config);

/* Legacy constructors (deprecated but retained) */
DEMI_API demi_engine_t demi_engine_create_default(void);
DEMI_API demi_engine_t demi_engine_create_sandboxed(const char* jail_root_path, uint64_t max_execution_ticks);

DEMI_API void demi_engine_destroy(demi_engine_t engine);

/* Error reporting */
DEMI_API const char* demi_engine_last_error(demi_engine_t engine);
DEMI_API int demi_engine_last_error_code(demi_engine_t engine);
DEMI_API void demi_engine_clear_error(demi_engine_t engine);

/* Execution & memory */
DEMI_API bool demi_engine_load_executable(demi_engine_t engine, const uint8_t* binary, size_t size, uint64_t base_address);
DEMI_API bool demi_engine_map_memory(demi_engine_t engine, uint64_t virtual_address, size_t size, uint32_t permissions);
DEMI_API bool demi_engine_write_memory(demi_engine_t engine, uint64_t virtual_address, const uint8_t* data, size_t size);
DEMI_API bool demi_engine_read_memory(demi_engine_t engine, uint64_t virtual_address, uint8_t* data, size_t size);
DEMI_API uint8_t demi_engine_peek_memory(demi_engine_t engine, uint64_t addr);

DEMI_API bool demi_engine_tick(demi_engine_t engine);
DEMI_API int demi_engine_step(demi_engine_t engine);
DEMI_API int demi_engine_run(demi_engine_t engine);
DEMI_API int demi_engine_run_for_ticks(demi_engine_t engine, uint64_t max_ticks);
DEMI_API bool demi_engine_reset(demi_engine_t engine);
DEMI_API uint64_t demi_engine_get_tick_count(demi_engine_t engine);

/* CPU introspection */
DEMI_API uint32_t demi_engine_get_pc(demi_engine_t engine);
DEMI_API bool demi_engine_set_pc(demi_engine_t engine, uint32_t pc);
DEMI_API bool demi_engine_get_register_u64(demi_engine_t engine, uint32_t reg_index, uint64_t* out);
DEMI_API bool demi_engine_set_register_u64(demi_engine_t engine, uint32_t reg_index, uint64_t value);
DEMI_API bool demi_engine_get_flags(demi_engine_t engine, uint32_t* out);
DEMI_API bool demi_engine_set_flags(demi_engine_t engine, uint32_t value);

/* Output capture */
DEMI_API void demi_engine_enable_output_capture(demi_engine_t engine, bool enabled);
DEMI_API const uint8_t* demi_engine_get_output_buffer(demi_engine_t engine, size_t* out_size);
DEMI_API void demi_engine_clear_output_buffer(demi_engine_t engine);

/* Hooks */
typedef void (*demi_memory_hook_cb)(uint64_t addr, uint8_t value, void* user_data);
typedef bool (*demi_syscall_hook_cb)(uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t* result, void* user_data);
typedef void (*demi_stdout_hook_cb)(int fd, const uint8_t* data, size_t size, void* user_data);
typedef void (*demi_stdin_hook_cb)(size_t max_count, uint8_t* data, size_t* out_size, void* user_data);

DEMI_API void demi_engine_set_memory_hook(demi_engine_t engine, uint64_t start_addr, uint64_t end_addr, demi_memory_hook_cb hook, void* user_data);
DEMI_API void demi_engine_set_syscall_hook(demi_engine_t engine, demi_syscall_hook_cb hook, void* user_data);
DEMI_API void demi_engine_set_stdout_hook(demi_engine_t engine, demi_stdout_hook_cb hook, void* user_data);
DEMI_API void demi_engine_set_stdin_hook(demi_engine_t engine, demi_stdin_hook_cb hook, void* user_data);

DEMI_API void demi_engine_clear_memory_hook(demi_engine_t engine);
DEMI_API void demi_engine_clear_syscall_hook(demi_engine_t engine);
DEMI_API void demi_engine_clear_stdout_hook(demi_engine_t engine);
DEMI_API void demi_engine_clear_stdin_hook(demi_engine_t engine);

#ifdef __cplusplus
}
#endif

