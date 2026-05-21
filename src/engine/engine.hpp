#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace demi {

struct Config {
    bool enable_sandbox = true;
    bool strict_io = true;
    std::string io_root_path = "/tmp/demi_vfs";
    uint64_t max_execution_ticks = 0;
    size_t memory_size = 1024 * 1024 * 16;
};

class Engine {
public:
    explicit Engine(const Config& config = Config{});
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool load_executable(const std::vector<uint8_t>& binary, uint64_t base_address = 0x1000);
    bool map_memory(uint64_t virtual_address, size_t size, uint32_t permissions);
    bool write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data);

    bool tick();

    enum class CompleteReason {
        FINISHED,
        WATCHDOG_TIMEOUT,
        SECURITY_VIOLATION,
        EXCEPTION
    };

    CompleteReason run();

    uint32_t get_pc() const;
    uint8_t peek_memory(uint64_t addr) const;

    using MemoryHook = std::function<void(uint64_t addr, uint8_t value)>;
    void set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook);

    using SyscallHook = std::function<bool(uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result)>;
    void set_syscall_hook(SyscallHook hook);

    using StdoutHook = std::function<void(int fd, const std::vector<uint8_t>& data)>;
    void set_stdout_hook(StdoutHook hook);

    using StdinHook = std::function<void(size_t max_count, std::vector<uint8_t>& data)>;
    void set_stdin_hook(StdinHook hook);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace demi