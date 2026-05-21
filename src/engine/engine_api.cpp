#include "engine.hpp"
#include <algorithm>

namespace demi {

// ==========================================
// 1. Full Private Implementation Definition
// ==========================================
class Engine::Impl {
public:
    Config config_;
    std::vector<uint8_t> current_program_;
    bool program_loaded_ = false;
    uint64_t current_ticks_ = 0;

    // Internal virtual machine dependencies
    // (Assuming default constructs exist for these within your engine layers)
    struct MockSyscallDispatcher {} syscall_dispatcher_;
    struct MockVFS {} vfs_;
    
    struct CPUState {
        MemoryHook memory_write_hook_;
        SyscallHook syscall_hook_;
        StdoutHook stdout_hook_;
        StdinHook stdin_hook_;
        
        uint64_t get_memory_size() const { return mem_.size(); }
        std::vector<uint8_t>& get_memory() { return mem_; }
        const std::vector<uint8_t>& get_memory() const { return mem_; }
        
        void resize_memory(size_t size) { mem_.resize(size); }
        void set_pc(uint64_t addr) { pc_ = addr; }
        uint32_t get_pc() const { return pc_; }
        
        void set_sandbox_environment(MockSyscallDispatcher*, MockVFS*) {}
        bool step(const std::vector<uint8_t>&) { return true; }
        bool has_security_fault() const { return false; }
        
    private:
        std::vector<uint8_t> mem_;
        uint32_t pc_ = 0;
    } cpu_;

    uint64_t hook_start_addr_ = 0;
    uint64_t hook_end_addr_ = 0;
    MemoryHook memory_write_hook_;

    // Constructor fixes the floating snippet from your raw input
    Impl(const Config& config) : config_(config) {
        cpu_.set_sandbox_environment(&syscall_dispatcher_, &vfs_);
        cpu_.resize_memory(config_.memory_size);
    }

    bool load_executable(const std::vector<uint8_t>& binary, uint64_t base_address) {
        if (base_address + binary.size() > cpu_.get_memory_size()) {
            return false;
        }

        auto& mem = cpu_.get_memory();
        std::copy(binary.begin(), binary.end(), mem.begin() + base_address);
        cpu_.set_pc(base_address);
        current_program_ = binary; 
        program_loaded_ = true;
        current_ticks_ = 0;
        return true;
    }

    bool map_memory(uint64_t virtual_address, size_t size, uint32_t /*permissions*/) {
        if (virtual_address + size > cpu_.get_memory_size()) {
            cpu_.resize_memory(virtual_address + size);
        }
        return true;
    }

    bool write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data) {
        if (virtual_address + data.size() > cpu_.get_memory_size()) {
            return false;
        }
        auto& mem = cpu_.get_memory();
        std::copy(data.begin(), data.end(), mem.begin() + virtual_address);
        
        if (memory_write_hook_) {
            for (size_t i = 0; i < data.size(); ++i) {
                uint64_t addr = virtual_address + i;
                if (addr >= hook_start_addr_ && addr <= hook_end_addr_) {
                    memory_write_hook_(addr, data[i]);
                }
            }
        }
        return true;
    }

    bool tick() {
        if (!program_loaded_) return false;
        
        bool advanced = cpu_.step(current_program_);
        current_ticks_++;
        return advanced;
    }

    Engine::CompleteReason run() {
        if (!program_loaded_) return CompleteReason::EXCEPTION;

        while (true) {
            bool running = cpu_.step(current_program_);
            current_ticks_++;

            if (config_.max_execution_ticks > 0 && current_ticks_ >= config_.max_execution_ticks) {
                return CompleteReason::WATCHDOG_TIMEOUT;
            }
            
            if (cpu_.has_security_fault()) {
                return CompleteReason::SECURITY_VIOLATION;
            }

            if (!running) {
                break;
            }
        }
        return CompleteReason::FINISHED;
    }

    void set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook) {
        hook_start_addr_ = start_addr;
        hook_end_addr_ = end_addr;
        memory_write_hook_ = std::move(hook);
    }

    void set_syscall_hook(SyscallHook hook) {
        cpu_.syscall_hook_ = std::move(hook);
    }

    void set_stdout_hook(StdoutHook hook) {
        cpu_.stdout_hook_ = std::move(hook);
    }

    void set_stdin_hook(StdinHook hook) {
        cpu_.stdin_hook_ = std::move(hook);
    }
};

// ==========================================
// 2. Engine Facade Wrapper Implementations
// ==========================================

Engine::Engine(const Config& config) : pimpl_(std::make_unique<Impl>(config)) {}

Engine::~Engine() = default;

bool Engine::load_executable(const std::vector<uint8_t>& binary, uint64_t base_address) {
    return pimpl_->load_executable(binary, base_address);
}

bool Engine::map_memory(uint64_t virtual_address, size_t size, uint32_t permissions) {
    return pimpl_->map_memory(virtual_address, size, permissions);
}

bool Engine::write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data) {
    return pimpl_->write_memory(virtual_address, data);
}

bool Engine::tick() {
    return pimpl_->tick();
}

Engine::CompleteReason Engine::run() {
    return pimpl_->run();
}

void Engine::set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook) {
    pimpl_->set_memory_write_hook(start_addr, end_addr, std::move(hook));
}

void Engine::set_syscall_hook(SyscallHook hook) {
    pimpl_->set_syscall_hook(std::move(hook));
}

void Engine::set_stdout_hook(StdoutHook hook) {
    pimpl_->set_stdout_hook(std::move(hook));
}

void Engine::set_stdin_hook(StdinHook hook) {
    pimpl_->set_stdin_hook(std::move(hook));
}

uint32_t Engine::get_pc() const {
    if (!pimpl_) return 0;
    return pimpl_->cpu_.get_pc();
}

uint8_t Engine::peek_memory(uint64_t addr) const {
    if (!pimpl_) return 0;
    const auto& mem = pimpl_->cpu_.get_memory();
    if (addr < mem.size()) return mem[addr];
    return 0;
}

} // namespace demi