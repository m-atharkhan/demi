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
    std::string virtual_disk_path;   // if set, file I/O routes through this .vfs container
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
    bool read_memory(uint64_t virtual_address, std::vector<uint8_t>& data, size_t size);

    bool tick();
    int step();
    int run_for_ticks(uint64_t max_ticks);
    bool reset();

    enum class CompleteReason {
        FINISHED,
        WATCHDOG_TIMEOUT,
        SECURITY_VIOLATION,
        EXCEPTION
    };

    CompleteReason run();

    uint32_t get_pc() const;
    bool set_pc(uint32_t pc);
    uint8_t peek_memory(uint64_t addr) const;
    uint64_t get_tick_count() const;

    bool get_register_u64(uint32_t reg_index, uint64_t& out) const;
    bool set_register_u64(uint32_t reg_index, uint64_t value);
    bool get_flags(uint32_t& out) const;
    bool set_flags(uint32_t value);

    const std::string& last_error() const;
    int last_error_code() const;
    void clear_error();

    void enable_output_capture(bool enabled);
    const std::vector<uint8_t>& get_output_buffer() const;
    void clear_output_buffer();

    using MemoryHook = std::function<void(uint64_t addr, uint8_t value)>;
    void set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook);
    void clear_memory_hook();

    using SyscallHook = std::function<bool(uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result)>;
    void set_syscall_hook(SyscallHook hook);
    void clear_syscall_hook();

    using StdoutHook = std::function<void(int fd, const std::vector<uint8_t>& data)>;
    void set_stdout_hook(StdoutHook hook);
    void clear_stdout_hook();

    using StdinHook = std::function<void(size_t max_count, std::vector<uint8_t>& data)>;
    void set_stdin_hook(StdinHook hook);
    void clear_stdin_hook();

    // VirtualDisk access (host-side interaction with the .vfs container)
    std::vector<uint8_t> vdisk_read_file(const std::string& filename) const;
    bool vdisk_write_file(const std::string& filename, const std::vector<uint8_t>& data);
    bool vdisk_delete_file(const std::string& filename);
    bool vdisk_file_exists(const std::string& filename) const;
    int vdisk_file_size(const std::string& filename) const;
    std::vector<std::string> vdisk_list_files() const;
    bool vdisk_save();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace demi

