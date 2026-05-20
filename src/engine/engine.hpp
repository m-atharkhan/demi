#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace demi {

// Security and execution configuration
struct Config {
    bool enable_sandbox = true;      // Intercepts and filters system calls
    bool strict_io = true;           // Jails file I/O to io_root_path
    std::string io_root_path = "/tmp/demi_vfs";
    
    uint64_t max_execution_ticks = 0; // 0 = unlimited. Protects against infinite loops
    size_t memory_size = 1024 * 1024 * 16; // 16MB default
};

class Engine {
public:
    explicit Engine(const Config& config = Config{});
    ~Engine();

    // Prevent copying to manage internal resources safely
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Memory Mapping & Initialization
    bool load_executable(const std::vector<uint8_t>& binary, uint64_t base_address = 0x1000);
    bool map_memory(uint64_t virtual_address, size_t size, uint32_t permissions);
    bool write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data);
    
    // Execution Control
    bool tick(); // Executes a single instruction
    
    enum class CompleteReason { 
        FINISHED, 
        WATCHDOG_TIMEOUT, 
        SECURITY_VIOLATION, 
        EXCEPTION 
    };
    
    CompleteReason run(); // Executes until halt, timeout, or violation

    // Hooking capability (e.g., for custom VRAM mapping in Minecraft mods)
    using MemoryHook = std::function<void(uint64_t addr, uint8_t value)>;
    void set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook);

    // Syscall hook (return true to bypass default handling)
    using SyscallHook = std::function<bool(uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result)>;
    void set_syscall_hook(SyscallHook hook);

    // Stdio hooks
    using StdoutHook = std::function<void(int fd, const std::vector<uint8_t>& data)>; // fd will be 1 or 2
    void set_stdout_hook(StdoutHook hook);

    using StdinHook = std::function<void(size_t max_count, std::vector<uint8_t>& data)>;
    void set_stdin_hook(StdinHook hook);

private:
    // Pimpl Idiom: Hides internal CPU/Memory classes from the public ABI
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace demi
