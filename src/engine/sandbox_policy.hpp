#pragma once
#include <cstdint>

#ifdef _WIN32
    // Windows doesn't have sys/syscall.h, define dummy values for the sandbox matcher
    #define SYS_read        3
    #define SYS_write       4
    #define SYS_open        5
    #define SYS_close       6
    #define SYS_gettimeofday 78
    #define SYS_exit        1
    #define SYS_fork        2
    #define SYS_execve      11
    #define SYS_ioctl       54
    #define SYS_socket      359
#else
    #include <sys/syscall.h>
#endif

// Fallbacks if not defined by the system
#ifndef SYS_read
#define SYS_read        3
#endif
#ifndef SYS_write
#define SYS_write       4
#endif
#ifndef SYS_open
#define SYS_open        5
#endif
#ifndef SYS_close
#define SYS_close       6
#endif
#ifndef SYS_gettimeofday
#define SYS_gettimeofday 78
#endif
#ifndef SYS_exit
#define SYS_exit        1
#endif
#ifndef SYS_fork
#define SYS_fork        2
#endif
#ifndef SYS_execve
#define SYS_execve      11
#endif
#ifndef SYS_socket
#define SYS_socket      359
#endif

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

        switch (syscall_id) {
            // Safe operations
            case SYS_exit:
            case SYS_gettimeofday:
            // Add other safe timing/math syscalls
                return SyscallResult::ALLOWED;
                
            // I/O operations (Handled by our Jailed VFS instead of host)
            case SYS_read:
            case SYS_write:
            case SYS_open:
            case SYS_close:
                return SyscallResult::HANDLED_INTERNALLY;

            // Dangerous operations
            case SYS_fork:
            case SYS_execve:
            case SYS_socket:
            // Add other networking/process syscalls
                return SyscallResult::DENIED;
                
            default:
                // Default deny in sandbox mode
                return SyscallResult::DENIED; 
        }
    }
};

} // namespace sandbox
} // namespace demi