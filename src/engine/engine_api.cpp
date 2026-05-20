#include "engine.hpp"
#include "cpu.hpp"
#include "vfs.hpp"
#include "sandbox_policy.hpp"

#include <iostream>
#include <stdexcept>

namespace demi {

class Engine::Impl {
public:
    Config config_;
    CPU cpu_;
    sandbox::VirtualFileSystem vfs_;
    sandbox::SyscallDispatcher syscall_dispatcher_;
    
    uint64_t current_ticks_ = 0;
    MemoryHook memory_write_hook_;
    uint64_t hook_start_addr_ = 0;
    uint64_t hook_end_addr_ = 0;
    
    std::vector<uint8_t> current_program_; // Cache program to step() through
    bool program_loaded_ = false;

    Impl(const Config& config) 
        : config_(config),
          vfs_(config.io_root_path, config.strict_io),
          syscall_dispatcher_(config.enable_sandbox) {
        
        cpu_.set_sandbox_environment(&syscall_dispatcher_, &vfs_);
        cpu_.resize_memory(config_.memory_size);
    }

    bool load_executable(const std::vector<uint8_t>& binary, uint64_t base_address) {
        // Just writing the binary to memory
        if (base_address + binary.size() > cpu_.get_memory_size()) {
            return false;
        }

        auto& mem = cpu_.get_memory();
        std::copy(binary.begin(), binary.end(), mem.begin() + base_address);
        cpu_.set_pc(base_address);
        current_program_ = binary; // Or construct full layout
        program_loaded_ = true;
        current_ticks_ = 0;
        return true;
    }

    bool map_memory(uint64_t virtual_address, size_t size, uint32_t /*permissions*/) {
        // Currently, CPU just uses a flat vector for memory.
        // We'd add custom mapping handling here if memory was paged.
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
        
        // Trigger hook if overlapping
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
        
        // Advance CPU by 1 instruction
        bool advanced = cpu_.step(current_program_);
        current_ticks_++;
        return advanced;
    }

    Engine::CompleteReason run() {
        if (!program_loaded_) return CompleteReason::EXCEPTION;

        // Run until halted, or interrupted by watchdog
        while (true) {
            bool running = cpu_.step(current_program_);
            current_ticks_++;

            // Watchdog Timeout Check
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
// Engine Facade wrapper (Pimpl)
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

} // namespace demi