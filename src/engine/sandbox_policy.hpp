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

// SyscallDispatcher: validates guest syscalls against the sandbox policy.
// When sandbox is enabled, file I/O is routed through the VirtualFileSystem
// (HANDLED_INTERNALLY) and process/network operations are DENIED.
// When sandbox is disabled, all syscalls are ALLOWED (passthrough to host).
class SyscallDispatcher {
    bool enable_sandbox_;

public:
    explicit SyscallDispatcher(bool enable_sandbox) : enable_sandbox_(enable_sandbox) {}

    bool sandbox_enabled() const { return enable_sandbox_; }

    SyscallResult validate_syscall(uint64_t syscall_id) const {
        if (!enable_sandbox_) return SyscallResult::ALLOWED;

        DemiEngine::Syscall sc = static_cast<DemiEngine::Syscall>(static_cast<uint32_t>(syscall_id));
        switch (sc) {
            // Always allowed
            case DemiEngine::Syscall::SYS_EXIT:
                return SyscallResult::ALLOWED;

            // File I/O — handled by VirtualFileSystem (VFS jail)
            case DemiEngine::Syscall::SYS_READ:
            case DemiEngine::Syscall::SYS_WRITE:
            case DemiEngine::Syscall::SYS_OPEN:
            case DemiEngine::Syscall::SYS_CLOSE:
                return SyscallResult::HANDLED_INTERNALLY;

            // Process execution — denied in sandbox
            case DemiEngine::Syscall::SYS_FORK:
            case DemiEngine::Syscall::SYS_EXECVE:
            // Network — denied in sandbox
            case DemiEngine::Syscall::SYS_SOCKET:
                return SyscallResult::DENIED;

            default:
                return SyscallResult::DENIED;
        }
    }
};

} // namespace sandbox
} // namespace demi
