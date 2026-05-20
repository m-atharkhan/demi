#include "engine_c_api.h"
#include "engine.hpp"

// Wrapper to hold C-compatible hook context
struct EngineContext {
    demi::Engine* engine;
    demi_memory_hook_cb memory_hook;
    void* memory_user_data;
    
    demi_syscall_hook_cb syscall_hook;
    void* syscall_user_data;

    demi_stdout_hook_cb stdout_hook;
    void* stdout_user_data;

    demi_stdin_hook_cb stdin_hook;
    void* stdin_user_data;

    EngineContext(demi::Engine* e) : 
        engine(e), 
        memory_hook(nullptr), memory_user_data(nullptr),
        syscall_hook(nullptr), syscall_user_data(nullptr),
        stdout_hook(nullptr), stdout_user_data(nullptr),
        stdin_hook(nullptr), stdin_user_data(nullptr) {}

    ~EngineContext() { delete engine; }
};

extern "C" {

demi_engine_t demi_engine_create_default() {
    demi::Config config;
    config.enable_sandbox = false;
    config.strict_io = false;
    config.max_execution_ticks = 0;
    
    return new EngineContext(new demi::Engine(config));
}

demi_engine_t demi_engine_create_sandboxed(const char* jail_root_path, uint64_t max_execution_ticks) {
    demi::Config config;
    config.enable_sandbox = true;
    config.strict_io = true;
    if (jail_root_path != nullptr) {
        config.io_root_path = jail_root_path;
    }
    config.max_execution_ticks = max_execution_ticks;

    return new EngineContext(new demi::Engine(config));
}

void demi_engine_destroy(demi_engine_t engine) {
    if (engine) {
        delete static_cast<EngineContext*>(engine);
    }
}

bool demi_engine_load_executable(demi_engine_t engine, const uint8_t* binary, size_t size, uint64_t base_address) {
    if (!engine || !binary) return false;
    
    std::vector<uint8_t> program(binary, binary + size);
    return static_cast<EngineContext*>(engine)->engine->load_executable(program, base_address);
}

bool demi_engine_tick(demi_engine_t engine) {
    if (!engine) return false;
    return static_cast<EngineContext*>(engine)->engine->tick();
}

int demi_engine_run(demi_engine_t engine) {
    if (!engine) return DEMI_EXCEPTION;
    auto reason = static_cast<EngineContext*>(engine)->engine->run();
    
    switch (reason) {
        case demi::Engine::CompleteReason::FINISHED: return DEMI_FINISHED;
        case demi::Engine::CompleteReason::WATCHDOG_TIMEOUT: return DEMI_WATCHDOG_TIMEOUT;
        case demi::Engine::CompleteReason::SECURITY_VIOLATION: return DEMI_SECURITY_VIOLATION;
        default: return DEMI_EXCEPTION;
    }
}

void demi_engine_set_memory_hook(demi_engine_t engine, uint64_t start_addr, uint64_t end_addr, demi_memory_hook_cb hook, void* user_data) {
    if (!engine) return;
    
    auto* ctx = static_cast<EngineContext*>(engine);
    ctx->memory_hook = hook;
    ctx->memory_user_data = user_data;

    // Use a C++ lambda to capture the context and forward the call to the C callback
    ctx->engine->set_memory_write_hook(start_addr, end_addr, [ctx](uint64_t addr, uint8_t value) {
        if (ctx->memory_hook) {
            ctx->memory_hook(addr, value, ctx->memory_user_data);
        }
    });
}

void demi_engine_set_syscall_hook(demi_engine_t engine, demi_syscall_hook_cb hook, void* user_data) {
    if (!engine) return;
    
    auto* ctx = static_cast<EngineContext*>(engine);
    ctx->syscall_hook = hook;
    ctx->syscall_user_data = user_data;

    ctx->engine->set_syscall_hook([ctx](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
        if (ctx->syscall_hook) {
            return ctx->syscall_hook(syscall_id, arg1, arg2, arg3, arg4, arg5, &result, ctx->syscall_user_data);
        }
        return false;
    });
}

void demi_engine_set_stdout_hook(demi_engine_t engine, demi_stdout_hook_cb hook, void* user_data) {
    if (!engine) return;
    
    auto* ctx = static_cast<EngineContext*>(engine);
    ctx->stdout_hook = hook;
    ctx->stdout_user_data = user_data;

    ctx->engine->set_stdout_hook([ctx](int fd, const std::vector<uint8_t>& data) {
        if (ctx->stdout_hook) {
            ctx->stdout_hook(fd, data.data(), data.size(), ctx->stdout_user_data);
        }
    });
}

void demi_engine_set_stdin_hook(demi_engine_t engine, demi_stdin_hook_cb hook, void* user_data) {
    if (!engine) return;
    
    auto* ctx = static_cast<EngineContext*>(engine);
    ctx->stdin_hook = hook;
    ctx->stdin_user_data = user_data;

    ctx->engine->set_stdin_hook([ctx](size_t max_count, std::vector<uint8_t>& data) {
        if (ctx->stdin_hook) {
            size_t out_size = 0;
            data.resize(max_count);
            ctx->stdin_hook(max_count, data.data(), &out_size, ctx->stdin_user_data);
            data.resize(out_size);
        }
    });
}

} // extern "C"