#include <demi/engine.hpp>
#include "cpu.hpp"
#include "sandbox_policy.hpp"
#include "vfs.hpp"
#include "cpu_registers.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace demi {

namespace {

using DemiEngine_Registers::Register;
using DemiEngine_Registers::TOTAL_REGISTERS;

enum class EngineErrorCode : int {
    OK = 0,
    NULL_ENGINE = 1,
    INVALID_BINARY = 2,
    MEMORY_OOB = 3,
    SANDBOX_VIOLATION = 4,
    SYSCALL_FAILURE = 5,
    MISSING_PROGRAM = 6,
    INVALID_CONFIG = 7,
    INVALID_REGISTER = 8,
    UNKNOWN = 99
};

Register to_register(uint32_t index) {
    return static_cast<Register>(index);
}

} // namespace

class Engine::Impl {
public:
    Config config_;
    CPU cpu_;
    sandbox::SyscallDispatcher syscall_dispatcher_;
    sandbox::VirtualFileSystem vfs_;

    std::vector<uint8_t> current_program_;
    uint64_t program_base_ = 0;
    uint64_t program_end_ = 0;
    bool program_loaded_ = false;
    uint64_t current_ticks_ = 0;

    uint64_t hook_start_addr_ = 0;
    uint64_t hook_end_addr_ = 0;
    MemoryHook memory_write_hook_;

    SyscallHook syscall_hook_;
    StdoutHook stdout_hook_;
    StdinHook stdin_hook_;

    bool output_capture_enabled_ = false;
    std::vector<uint8_t> output_buffer_;

    mutable std::string last_error_;
    mutable int last_error_code_ = static_cast<int>(EngineErrorCode::OK);

    Impl(const Config& config)
        : config_(config),
          cpu_(config.memory_size),
          syscall_dispatcher_(config.enable_sandbox),
          vfs_(config.io_root_path, config.strict_io) {
        cpu_.set_sandbox_environment(&syscall_dispatcher_, &vfs_);
        wire_cpu_hooks();
    }

    void set_error(EngineErrorCode code, const std::string& message) const {
        last_error_code_ = static_cast<int>(code);
        last_error_ = message;
    }

    void clear_error() const {
        last_error_code_ = static_cast<int>(EngineErrorCode::OK);
        last_error_.clear();
    }

    void wire_cpu_hooks() {
        cpu_.set_hooks(
            [this](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3,
                   uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
                if (syscall_hook_) {
                    return syscall_hook_(syscall_id, arg1, arg2, arg3, arg4, arg5, result);
                }
                return false;
            },
            [this](int fd, const std::vector<uint8_t>& data) {
                if (output_capture_enabled_) {
                    output_buffer_.insert(output_buffer_.end(), data.begin(), data.end());
                }
                if (stdout_hook_) {
                    stdout_hook_(fd, data);
                }
            },
            [this](size_t max_count, std::vector<uint8_t>& data) {
                if (stdin_hook_) {
                    stdin_hook_(max_count, data);
                }
            });
    }

    bool load_executable(const std::vector<uint8_t>& binary, uint64_t base_address) {
        clear_error();
        if (binary.empty()) {
            set_error(EngineErrorCode::INVALID_BINARY, "executable binary is empty");
            return false;
        }
        if (base_address + binary.size() > cpu_.get_memory_size()) {
            set_error(EngineErrorCode::MEMORY_OOB, "executable does not fit in VM memory");
            return false;
        }

        auto& mem = cpu_.get_memory();
        std::fill(mem.begin() + base_address, mem.begin() + base_address + binary.size(), 0);
        std::copy(binary.begin(), binary.end(), mem.begin() + base_address);

        current_program_.assign(static_cast<size_t>(base_address), 0);
        current_program_.insert(current_program_.end(), binary.begin(), binary.end());

        program_base_ = base_address;
        program_end_ = base_address + binary.size();
        cpu_.set_pc(static_cast<uint32_t>(base_address));
        program_loaded_ = true;
        current_ticks_ = 0;
        return true;
    }

    bool map_memory(uint64_t virtual_address, size_t size, uint32_t /*permissions*/) {
        clear_error();
        try {
            size_t current_size = cpu_.get_memory_size();
            if (virtual_address > current_size || size > SIZE_MAX - virtual_address) {
                set_error(EngineErrorCode::MEMORY_OOB, "memory mapping address/size overflow");
                return false;
            }
            if (static_cast<size_t>(virtual_address) + size > current_size) {
                cpu_.resize_memory(static_cast<size_t>(virtual_address) + size);
            }
            return true;
        } catch (const std::exception& e) {
            set_error(EngineErrorCode::MEMORY_OOB, e.what());
            return false;
        }
    }

    bool write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data) {
        clear_error();
        size_t mem_size = cpu_.get_memory_size();
        if (virtual_address > mem_size || data.size() > mem_size - virtual_address) {
            set_error(EngineErrorCode::MEMORY_OOB, "write would exceed VM memory bounds");
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

    bool read_memory(uint64_t virtual_address, std::vector<uint8_t>& data, size_t size) {
        size_t mem_size = cpu_.get_memory_size();
        if (virtual_address > mem_size || size > mem_size - virtual_address) {
            return false;
        }
        const auto& mem = cpu_.get_memory();
        data.assign(mem.begin() + static_cast<size_t>(virtual_address),
                    mem.begin() + static_cast<size_t>(virtual_address) + size);
        return true;
    }

    Engine::CompleteReason run_internal() {
        clear_error();
        if (!program_loaded_) {
            set_error(EngineErrorCode::MISSING_PROGRAM, "no program loaded");
            return Engine::CompleteReason::EXCEPTION;
        }

        while (cpu_.get_pc() < program_end_) {
            bool running = true;
            if (!cpu_.handle_pending_interrupts(current_program_, running)) {
                if (!cpu_.step(current_program_)) {
                    break;
                }
            }
            current_ticks_++;

            if (cpu_.has_security_fault()) {
                set_error(EngineErrorCode::SANDBOX_VIOLATION, "sandbox denied syscall or I/O access");
                return Engine::CompleteReason::SECURITY_VIOLATION;
            }

            if (!running) {
                break;
            }

            if (config_.max_execution_ticks > 0 && current_ticks_ >= config_.max_execution_ticks) {
                return Engine::CompleteReason::WATCHDOG_TIMEOUT;
            }
        }

        return Engine::CompleteReason::FINISHED;
    }

    bool tick() {
        clear_error();
        if (!program_loaded_) {
            set_error(EngineErrorCode::MISSING_PROGRAM, "no program loaded");
            return false;
        }
        if (cpu_.get_pc() >= program_end_) {
            return false;
        }

        bool running = true;
        if (!cpu_.handle_pending_interrupts(current_program_, running)) {
            if (!cpu_.step(current_program_)) {
                return false;
            }
        }
        current_ticks_++;

        if (cpu_.has_security_fault()) {
            set_error(EngineErrorCode::SANDBOX_VIOLATION, "sandbox denied syscall or I/O access");
        }

        return running && cpu_.get_pc() < program_end_;
    }

    int step() {
        clear_error();
        if (!program_loaded_) {
            set_error(EngineErrorCode::MISSING_PROGRAM, "no program loaded");
            return static_cast<int>(Engine::CompleteReason::EXCEPTION);
        }
        if (cpu_.get_pc() >= program_end_) {
            return static_cast<int>(Engine::CompleteReason::FINISHED);
        }

        bool running = true;
        if (!cpu_.handle_pending_interrupts(current_program_, running)) {
            if (!cpu_.step(current_program_)) {
                return static_cast<int>(Engine::CompleteReason::FINISHED);
            }
        }
        current_ticks_++;

        if (cpu_.has_security_fault()) {
            set_error(EngineErrorCode::SANDBOX_VIOLATION, "sandbox denied syscall or I/O access");
            return static_cast<int>(Engine::CompleteReason::SECURITY_VIOLATION);
        }

        if (!running || cpu_.get_pc() >= program_end_) {
            return static_cast<int>(Engine::CompleteReason::FINISHED);
        }

        if (config_.max_execution_ticks > 0 && current_ticks_ >= config_.max_execution_ticks) {
            return static_cast<int>(Engine::CompleteReason::WATCHDOG_TIMEOUT);
        }

        return -1; // still running after one instruction
    }

    int run_for_ticks(uint64_t max_ticks) {
        clear_error();
        if (!program_loaded_) {
            set_error(EngineErrorCode::MISSING_PROGRAM, "no program loaded");
            return static_cast<int>(Engine::CompleteReason::EXCEPTION);
        }

        uint64_t ticks_run = 0;
        while (cpu_.get_pc() < program_end_ && ticks_run < max_ticks) {
            bool running = true;
            if (!cpu_.handle_pending_interrupts(current_program_, running)) {
                if (!cpu_.step(current_program_)) {
                    return static_cast<int>(Engine::CompleteReason::FINISHED);
                }
            }
            current_ticks_++;
            ticks_run++;

            if (cpu_.has_security_fault()) {
                set_error(EngineErrorCode::SANDBOX_VIOLATION, "sandbox denied syscall or I/O access");
                return static_cast<int>(Engine::CompleteReason::SECURITY_VIOLATION);
            }

            if (!running) {
                return static_cast<int>(Engine::CompleteReason::FINISHED);
            }

            if (config_.max_execution_ticks > 0 && current_ticks_ >= config_.max_execution_ticks) {
                return static_cast<int>(Engine::CompleteReason::WATCHDOG_TIMEOUT);
            }
        }

        if (cpu_.get_pc() >= program_end_) {
            return static_cast<int>(Engine::CompleteReason::FINISHED);
        }
        return -1; // tick budget exhausted, VM still running
    }

    bool reset() {
        clear_error();
        try {
            cpu_.reset();
            cpu_.set_sandbox_environment(&syscall_dispatcher_, &vfs_);
            wire_cpu_hooks();
            current_ticks_ = 0;
            cpu_.clear_security_fault();
            if (program_loaded_) {
                return load_executable(
                    std::vector<uint8_t>(current_program_.begin() + program_base_,
                                         current_program_.end()),
                    program_base_);
            }
            return true;
        } catch (const std::exception& e) {
            set_error(EngineErrorCode::UNKNOWN, e.what());
            return false;
        }
    }

    bool get_register_u64(uint32_t reg_index, uint64_t& out) const {
        if (reg_index >= TOTAL_REGISTERS) {
            return false;
        }
        out = cpu_.get_register_64(to_register(reg_index));
        return true;
    }

    bool set_register_u64(uint32_t reg_index, uint64_t value) {
        if (reg_index >= TOTAL_REGISTERS) {
            return false;
        }
        cpu_.set_register_64(to_register(reg_index), value);
        return true;
    }

    void set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook) {
        hook_start_addr_ = start_addr;
        hook_end_addr_ = end_addr;
        memory_write_hook_ = std::move(hook);
    }

    void set_syscall_hook(SyscallHook hook) {
        syscall_hook_ = std::move(hook);
        wire_cpu_hooks();
    }

    void set_stdout_hook(StdoutHook hook) {
        stdout_hook_ = std::move(hook);
        wire_cpu_hooks();
    }

    void set_stdin_hook(StdinHook hook) {
        stdin_hook_ = std::move(hook);
        wire_cpu_hooks();
    }
};

Engine::Engine(const Config& config) : pimpl_(std::make_unique<Impl>(config)) {}
Engine::~Engine() = default;

bool Engine::load_executable(const std::vector<uint8_t>& binary, uint64_t base_address) {
    try {
        return pimpl_->load_executable(binary, base_address);
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::INVALID_BINARY, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::map_memory(uint64_t virtual_address, size_t size, uint32_t permissions) {
    try {
        return pimpl_->map_memory(virtual_address, size, permissions);
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::MEMORY_OOB, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::write_memory(uint64_t virtual_address, const std::vector<uint8_t>& data) {
    try {
        return pimpl_->write_memory(virtual_address, data);
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::MEMORY_OOB, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::read_memory(uint64_t virtual_address, std::vector<uint8_t>& data, size_t size) {
    try {
        if (!pimpl_->read_memory(virtual_address, data, size)) {
            pimpl_->set_error(EngineErrorCode::MEMORY_OOB, "read would exceed VM memory bounds");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::MEMORY_OOB, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::tick() {
    try {
        return pimpl_->tick();
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

int Engine::step() {
    try {
        return pimpl_->step();
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return static_cast<int>(CompleteReason::EXCEPTION);
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return static_cast<int>(CompleteReason::EXCEPTION);
    }
}

int Engine::run_for_ticks(uint64_t max_ticks) {
    try {
        return pimpl_->run_for_ticks(max_ticks);
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return static_cast<int>(CompleteReason::EXCEPTION);
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return static_cast<int>(CompleteReason::EXCEPTION);
    }
}

bool Engine::reset() {
    try {
        return pimpl_->reset();
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

Engine::CompleteReason Engine::run() {
    try {
        return pimpl_->run_internal();
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return CompleteReason::EXCEPTION;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return CompleteReason::EXCEPTION;
    }
}

uint32_t Engine::get_pc() const {
    if (!pimpl_) return 0;
    return pimpl_->cpu_.get_pc();
}

bool Engine::set_pc(uint32_t pc) {
    if (!pimpl_) return false;
    try {
        pimpl_->cpu_.set_pc(pc);
        return true;
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

uint8_t Engine::peek_memory(uint64_t addr) const {
    if (!pimpl_) return 0;
    const auto& mem = pimpl_->cpu_.get_memory();
    if (addr < mem.size()) return mem[addr];
    return 0;
}

uint64_t Engine::get_tick_count() const {
    if (!pimpl_) return 0;
    return pimpl_->current_ticks_;
}

bool Engine::get_register_u64(uint32_t reg_index, uint64_t& out) const {
    if (!pimpl_) return false;
    try {
        if (!pimpl_->get_register_u64(reg_index, out)) {
            pimpl_->set_error(EngineErrorCode::INVALID_REGISTER, "invalid register index");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::set_register_u64(uint32_t reg_index, uint64_t value) {
    if (!pimpl_) return false;
    try {
        if (!pimpl_->set_register_u64(reg_index, value)) {
            pimpl_->set_error(EngineErrorCode::INVALID_REGISTER, "invalid register index");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

bool Engine::get_flags(uint32_t& out) const {
    if (!pimpl_) return false;
    out = pimpl_->cpu_.get_flags();
    return true;
}

bool Engine::set_flags(uint32_t value) {
    if (!pimpl_) return false;
    try {
        pimpl_->cpu_.set_flags(value);
        return true;
    } catch (const std::exception& e) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, e.what());
        return false;
    } catch (...) {
        pimpl_->set_error(EngineErrorCode::UNKNOWN, "unknown error");
        return false;
    }
}

const std::string& Engine::last_error() const {
    static const std::string empty;
    if (!pimpl_) return empty;
    return pimpl_->last_error_;
}

int Engine::last_error_code() const {
    if (!pimpl_) return static_cast<int>(EngineErrorCode::NULL_ENGINE);
    return pimpl_->last_error_code_;
}

void Engine::clear_error() {
    if (pimpl_) pimpl_->clear_error();
}

void Engine::enable_output_capture(bool enabled) {
    if (pimpl_) pimpl_->output_capture_enabled_ = enabled;
}

const std::vector<uint8_t>& Engine::get_output_buffer() const {
    static const std::vector<uint8_t> empty;
    if (!pimpl_) return empty;
    return pimpl_->output_buffer_;
}

void Engine::clear_output_buffer() {
    if (pimpl_) pimpl_->output_buffer_.clear();
}

void Engine::set_memory_write_hook(uint64_t start_addr, uint64_t end_addr, MemoryHook hook) {
    pimpl_->set_memory_write_hook(start_addr, end_addr, std::move(hook));
}

void Engine::clear_memory_hook() {
    pimpl_->set_memory_write_hook(0, 0, nullptr);
}

void Engine::set_syscall_hook(SyscallHook hook) {
    pimpl_->set_syscall_hook(std::move(hook));
}

void Engine::clear_syscall_hook() {
    pimpl_->set_syscall_hook(nullptr);
}

void Engine::set_stdout_hook(StdoutHook hook) {
    pimpl_->set_stdout_hook(std::move(hook));
}

void Engine::clear_stdout_hook() {
    pimpl_->set_stdout_hook(nullptr);
}

void Engine::set_stdin_hook(StdinHook hook) {
    pimpl_->set_stdin_hook(std::move(hook));
}

void Engine::clear_stdin_hook() {
    pimpl_->set_stdin_hook(nullptr);
}

} // namespace demi
