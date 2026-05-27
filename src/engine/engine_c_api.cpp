#include <demi/engine_c_api.h>
#include <demi/engine.hpp>

#include <cstring>
#include <new>
#include <string>
#include <vector>

static constexpr uint32_t DEMI_ENGINE_MAGIC = 0xDE11E001;

struct EngineContext {
    uint32_t magic = DEMI_ENGINE_MAGIC;
    demi::Engine* engine = nullptr;
    std::string last_error;

    demi_memory_hook_cb memory_hook = nullptr;
    void* memory_user_data = nullptr;

    demi_syscall_hook_cb syscall_hook = nullptr;
    void* syscall_user_data = nullptr;

    demi_stdout_hook_cb stdout_hook = nullptr;
    void* stdout_user_data = nullptr;

    demi_stdin_hook_cb stdin_hook = nullptr;
    void* stdin_user_data = nullptr;

    explicit EngineContext(demi::Engine* e) : engine(e) {}
    ~EngineContext() { delete engine; }
};

namespace {

EngineContext* validate_engine(demi_engine_t engine) {
    if (!engine) {
        return nullptr;
    }
    auto* ctx = static_cast<EngineContext*>(engine);
    if (ctx->magic != DEMI_ENGINE_MAGIC || ctx->engine == nullptr) {
        return nullptr;
    }
    return ctx;
}

demi::Config config_from_c(const demi_config_t* config, std::string& error) {
    demi::Config out;
    if (config == nullptr) {
        error = "config pointer is null";
        return out;
    }
    if (config->memory_size == 0) {
        error = "memory_size must be greater than zero";
        return out;
    }
    out.enable_sandbox = config->enable_sandbox;
    out.strict_io = config->strict_io;
    out.max_execution_ticks = config->max_execution_ticks;
    out.memory_size = config->memory_size;
    if (config->io_root_path != nullptr) {
        out.io_root_path = config->io_root_path;
    }
    return out;
}

int to_c_result(demi::Engine::CompleteReason reason) {
    switch (reason) {
        case demi::Engine::CompleteReason::FINISHED:
            return DEMI_FINISHED;
        case demi::Engine::CompleteReason::WATCHDOG_TIMEOUT:
            return DEMI_WATCHDOG_TIMEOUT;
        case demi::Engine::CompleteReason::SECURITY_VIOLATION:
            return DEMI_SECURITY_VIOLATION;
        default:
            return DEMI_EXCEPTION;
    }
}

} // namespace

extern "C" {

uint32_t demi_engine_api_version(void) {
    return DEMI_API_VERSION;
}

demi_config_t demi_engine_default_config(void) {
    demi_config_t config{};
    config.enable_sandbox = false;
    config.strict_io = false;
    config.io_root_path = "/tmp/demi_vfs";
    config.max_execution_ticks = 0;
    config.memory_size = 1024 * 1024 * 16;
    return config;
}

demi_config_t demi_engine_sandboxed_config(const char* jail_root_path) {
    demi_config_t config = demi_engine_default_config();
    config.enable_sandbox = true;
    config.strict_io = true;
    config.max_execution_ticks = 100'000'000;
    if (jail_root_path != nullptr) {
        config.io_root_path = jail_root_path;
    }
    return config;
}

demi_engine_t demi_engine_create(const demi_config_t* config) {
    try {
        std::string error;
        if (config == nullptr || config->memory_size == 0) {
            return nullptr;
        }
        demi::Config cpp_config = config_from_c(config, error);
        return new EngineContext(new demi::Engine(cpp_config));
    } catch (const std::exception& e) {
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

demi_engine_t demi_engine_create_default(void) {
    demi_config_t cfg = demi_engine_default_config();
    return demi_engine_create(&cfg);
}

demi_engine_t demi_engine_create_sandboxed(const char* jail_root_path, uint64_t max_execution_ticks) {
    demi_config_t config = demi_engine_sandboxed_config(jail_root_path);
    config.max_execution_ticks = max_execution_ticks;
    return demi_engine_create(&config);
}

void demi_engine_destroy(demi_engine_t engine) {
    if (!engine) {
        return;
    }
    auto* ctx = static_cast<EngineContext*>(engine);
    ctx->magic = 0;
    delete ctx;
}

const char* demi_engine_last_error(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return "invalid or null engine handle";
    }
    if (!ctx->last_error.empty()) {
        return ctx->last_error.c_str();
    }
    return ctx->engine->last_error().c_str();
}

int demi_engine_last_error_code(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return DEMI_ERR_NULL_ENGINE;
    }
    if (!ctx->last_error.empty()) {
        return DEMI_ERR_INVALID_CONFIG;
    }
    return ctx->engine->last_error_code();
}

void demi_engine_clear_error(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->last_error.clear();
    ctx->engine->clear_error();
}

bool demi_engine_load_executable(demi_engine_t engine, const uint8_t* binary, size_t size, uint64_t base_address) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    if (!binary) {
        ctx->last_error = "binary pointer is null";
        return false;
    }
    try {
        std::vector<uint8_t> program(binary, binary + size);
        return ctx->engine->load_executable(program, base_address);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_map_memory(demi_engine_t engine, uint64_t virtual_address, size_t size, uint32_t permissions) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->map_memory(virtual_address, size, permissions);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_write_memory(demi_engine_t engine, uint64_t virtual_address, const uint8_t* data, size_t size) {
    auto* ctx = validate_engine(engine);
    if (!ctx || !data) {
        return false;
    }
    try {
        std::vector<uint8_t> bytes(data, data + size);
        return ctx->engine->write_memory(virtual_address, bytes);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_read_memory(demi_engine_t engine, uint64_t virtual_address, uint8_t* data, size_t size) {
    auto* ctx = validate_engine(engine);
    if (!ctx || !data) {
        return false;
    }
    try {
        std::vector<uint8_t> bytes;
        if (!ctx->engine->read_memory(virtual_address, bytes, size)) {
            return false;
        }
        std::memcpy(data, bytes.data(), bytes.size());
        return true;
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

uint8_t demi_engine_peek_memory(demi_engine_t engine, uint64_t addr) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return 0;
    }
    return ctx->engine->peek_memory(addr);
}

bool demi_engine_tick(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->tick();
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

int demi_engine_step(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return DEMI_EXCEPTION;
    }
    try {
        return ctx->engine->step();
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return DEMI_EXCEPTION;
    } catch (...) {
        ctx->last_error = "unknown error";
        return DEMI_EXCEPTION;
    }
}

int demi_engine_run(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return DEMI_EXCEPTION;
    }
    try {
        return to_c_result(ctx->engine->run());
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return DEMI_EXCEPTION;
    } catch (...) {
        ctx->last_error = "unknown error";
        return DEMI_EXCEPTION;
    }
}

int demi_engine_run_for_ticks(demi_engine_t engine, uint64_t max_ticks) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return DEMI_EXCEPTION;
    }
    try {
        return ctx->engine->run_for_ticks(max_ticks);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return DEMI_EXCEPTION;
    } catch (...) {
        ctx->last_error = "unknown error";
        return DEMI_EXCEPTION;
    }
}

bool demi_engine_reset(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->reset();
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

uint64_t demi_engine_get_tick_count(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return 0;
    }
    return ctx->engine->get_tick_count();
}

uint32_t demi_engine_get_pc(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return 0;
    }
    return ctx->engine->get_pc();
}

bool demi_engine_set_pc(demi_engine_t engine, uint32_t pc) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->set_pc(pc);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_get_register_u64(demi_engine_t engine, uint32_t reg_index, uint64_t* out) {
    auto* ctx = validate_engine(engine);
    if (!ctx || !out) {
        return false;
    }
    try {
        return ctx->engine->get_register_u64(reg_index, *out);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_set_register_u64(demi_engine_t engine, uint32_t reg_index, uint64_t value) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->set_register_u64(reg_index, value);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

bool demi_engine_get_flags(demi_engine_t engine, uint32_t* out) {
    auto* ctx = validate_engine(engine);
    if (!ctx || !out) {
        return false;
    }
    return ctx->engine->get_flags(*out);
}

bool demi_engine_set_flags(demi_engine_t engine, uint32_t value) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return false;
    }
    try {
        return ctx->engine->set_flags(value);
    } catch (const std::exception& e) {
        ctx->last_error = e.what();
        return false;
    } catch (...) {
        ctx->last_error = "unknown error";
        return false;
    }
}

void demi_engine_enable_output_capture(demi_engine_t engine, bool enabled) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->engine->enable_output_capture(enabled);
}

const uint8_t* demi_engine_get_output_buffer(demi_engine_t engine, size_t* out_size) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        if (out_size) {
            *out_size = 0;
        }
        return nullptr;
    }
    const auto& buffer = ctx->engine->get_output_buffer();
    if (out_size) {
        *out_size = buffer.size();
    }
    return buffer.empty() ? nullptr : buffer.data();
}

void demi_engine_clear_output_buffer(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->engine->clear_output_buffer();
}

void demi_engine_set_memory_hook(demi_engine_t engine, uint64_t start_addr, uint64_t end_addr, demi_memory_hook_cb hook, void* user_data) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->memory_hook = hook;
    ctx->memory_user_data = user_data;
    ctx->engine->set_memory_write_hook(start_addr, end_addr, [ctx](uint64_t addr, uint8_t value) {
        if (ctx->memory_hook) {
            ctx->memory_hook(addr, value, ctx->memory_user_data);
        }
    });
}

void demi_engine_set_syscall_hook(demi_engine_t engine, demi_syscall_hook_cb hook, void* user_data) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
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
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->stdout_hook = hook;
    ctx->stdout_user_data = user_data;
    ctx->engine->set_stdout_hook([ctx](int fd, const std::vector<uint8_t>& data) {
        if (ctx->stdout_hook) {
            ctx->stdout_hook(fd, data.data(), data.size(), ctx->stdout_user_data);
        }
    });
}

void demi_engine_set_stdin_hook(demi_engine_t engine, demi_stdin_hook_cb hook, void* user_data) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->stdin_hook = hook;
    ctx->stdin_user_data = user_data;
    ctx->engine->set_stdin_hook([ctx](size_t max_count, std::vector<uint8_t>& data) {
        if (ctx->stdin_hook) {
            size_t out_size = 0;
            data.resize(max_count);
            ctx->stdin_hook(max_count, data.data(), &out_size, ctx->stdin_user_data);
            if (out_size > max_count) {
                out_size = max_count;
            }
            data.resize(out_size);
        }
    });
}

void demi_engine_clear_memory_hook(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->memory_hook = nullptr;
    ctx->memory_user_data = nullptr;
    ctx->engine->clear_memory_hook();
}

void demi_engine_clear_syscall_hook(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->syscall_hook = nullptr;
    ctx->syscall_user_data = nullptr;
    ctx->engine->clear_syscall_hook();
}

void demi_engine_clear_stdout_hook(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->stdout_hook = nullptr;
    ctx->stdout_user_data = nullptr;
    ctx->engine->clear_stdout_hook();
}

void demi_engine_clear_stdin_hook(demi_engine_t engine) {
    auto* ctx = validate_engine(engine);
    if (!ctx) {
        return;
    }
    ctx->stdin_hook = nullptr;
    ctx->stdin_user_data = nullptr;
    ctx->engine->clear_stdin_hook();
}

} // extern "C"
