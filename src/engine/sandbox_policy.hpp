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

public:
    explicit SyscallDispatcher(bool enable_sandbox) : enable_sandbox_(enable_sandbox) {}

    SyscallResult validate_syscall(uint64_t syscall_id) const {
        if (!enable_sandbox_) return SyscallResult::ALLOWED;

        // Use the guest VM ABI Syscall enum for matching
        DemiEngine::Syscall sc = static_cast<DemiEngine::Syscall>(static_cast<uint32_t>(syscall_id));
        switch (sc) {
            // Safe operations
            case DemiEngine::Syscall::SYS_EXIT:
            case DemiEngine::Syscall::SYS_GETTIMEOFDAY:
                return SyscallResult::ALLOWED;

            // I/O operations (Handled by our Jailed VFS instead of host)
            case DemiEngine::Syscall::SYS_READ:
            case DemiEngine::Syscall::SYS_WRITE:
            case DemiEngine::Syscall::SYS_OPEN:
            case DemiEngine::Syscall::SYS_CLOSE:
                return SyscallResult::HANDLED_INTERNALLY;

            // Dangerous operations
            case DemiEngine::Syscall::SYS_FORK:
            case DemiEngine::Syscall::SYS_EXECVE:
            case DemiEngine::Syscall::SYS_SOCKET:
                return SyscallResult::DENIED;

            default:
                // Default deny in sandbox mode
                return SyscallResult::DENIED;
        }
    }
};

} // namespace sandbox
} // namespace demi