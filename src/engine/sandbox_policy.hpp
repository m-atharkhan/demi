#pragma once
#include <cstdint>

#include "syscalls.hpp"

namespace demi {
namespace sandbox {

enum class SyscallResult { 
    ALLOWED, 
    DENIED, 
    HANDLED_INTERNALLY 
};

class SyscallDispatcher {
    bool enable_sandbox_;
    bool allow_exec_ = false;  // --allow-exec: permit fork+execve in sandbox

public:
    explicit SyscallDispatcher(bool enable_sandbox) : enable_sandbox_(enable_sandbox) {}

    void set_allow_exec(bool v) { allow_exec_ = v; }
    bool sandbox_enabled() const { return enable_sandbox_; }

    SyscallResult validate_syscall(uint64_t syscall_id) const {
        if (!enable_sandbox_) return SyscallResult::ALLOWED;

        DemiEngine::Syscall sc = static_cast<DemiEngine::Syscall>(static_cast<uint32_t>(syscall_id));
        switch (sc) {
            case DemiEngine::Syscall::SYS_EXIT:
                return SyscallResult::ALLOWED;

            case DemiEngine::Syscall::SYS_READ:
            case DemiEngine::Syscall::SYS_WRITE:
            case DemiEngine::Syscall::SYS_OPEN:
            case DemiEngine::Syscall::SYS_CLOSE:
                return SyscallResult::HANDLED_INTERNALLY;

            case DemiEngine::Syscall::SYS_FORK:
            case DemiEngine::Syscall::SYS_EXECVE:
                return allow_exec_ ? SyscallResult::ALLOWED : SyscallResult::DENIED;

            case DemiEngine::Syscall::SYS_SOCKET:
                return SyscallResult::DENIED;

            default:
                return SyscallResult::DENIED;
        }
    }
};

} // namespace sandbox
} // namespace demi
