#pragma once

#include <cstdint>

namespace DemiEngine {

// Linux i386 syscall numbers (INT 0x80)
// Reference: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
enum class Syscall : uint32_t {
    // Process Control
    SYS_EXIT        = 1,
    SYS_FORK        = 2,
    SYS_WAITPID     = 7,
    SYS_EXECVE      = 11,
    SYS_KILL        = 37,
    SYS_GETPID      = 20,
    SYS_GETPPID     = 64,
    
    // File Operations
    SYS_READ        = 3,
    SYS_WRITE       = 4,
    SYS_OPEN        = 5,
    SYS_CLOSE       = 6,
    SYS_CREAT       = 8,
    SYS_LINK        = 9,
    SYS_UNLINK      = 10,
    SYS_CHDIR       = 12,
    SYS_LSEEK       = 19,
    SYS_ACCESS      = 33,
    SYS_RENAME      = 38,
    SYS_MKDIR       = 39,
    SYS_RMDIR       = 40,
    SYS_DUP         = 41,
    SYS_PIPE        = 42,
    SYS_IOCTL       = 54,
    SYS_FCNTL       = 55,
    SYS_DUP2        = 63,
    SYS_FSYNC       = 118,
    
    // Memory Management
    SYS_BRK         = 45,
    SYS_MMAP        = 90,
    SYS_MUNMAP      = 91,
    SYS_MPROTECT    = 125,
    SYS_MMAP2       = 192,
    
    // Time
    SYS_TIME        = 13,
    SYS_GETTIMEOFDAY = 78,
    SYS_NANOSLEEP   = 162,
    
    // Signals
    SYS_SIGNAL      = 48,
    SYS_SIGACTION   = 67,
    SYS_SIGRETURN   = 119,
    
    // Network (sockets)
    SYS_SOCKET      = 359,  // Actually socketcall on i386
    SYS_BIND        = 361,
    SYS_CONNECT     = 362,
    SYS_LISTEN      = 363,
    SYS_ACCEPT      = 364,
    SYS_SEND        = 369,
    SYS_RECV        = 371,
    
    // Information
    SYS_UNAME       = 122,
    SYS_STAT        = 106,
    SYS_FSTAT       = 108,
    SYS_LSTAT       = 107,
    
    // Other
    SYS_GETUID      = 24,
    SYS_GETEUID     = 49,
    SYS_GETGID      = 47,
    SYS_GETEGID     = 50,
    
    // Placeholder for unsupported
    SYS_UNSUPPORTED = 0xFFFFFFFF
};

// Helper to convert syscall number to enum
inline Syscall to_syscall(uint32_t num) {
    switch (num) {
        case 1: return Syscall::SYS_EXIT;
        case 3: return Syscall::SYS_READ;
        case 4: return Syscall::SYS_WRITE;
        case 5: return Syscall::SYS_OPEN;
        case 6: return Syscall::SYS_CLOSE;
        case 45: return Syscall::SYS_BRK;
        case 54: return Syscall::SYS_IOCTL;
        case 90: return Syscall::SYS_MMAP;
        case 192: return Syscall::SYS_MMAP2;
        default: return Syscall::SYS_UNSUPPORTED;
    }
}

// Helper to get syscall name for logging
inline const char* syscall_name(Syscall sc) {
    switch (sc) {
        case Syscall::SYS_EXIT: return "sys_exit";
        case Syscall::SYS_READ: return "sys_read";
        case Syscall::SYS_WRITE: return "sys_write";
        case Syscall::SYS_OPEN: return "sys_open";
        case Syscall::SYS_CLOSE: return "sys_close";
        case Syscall::SYS_BRK: return "sys_brk";
        case Syscall::SYS_IOCTL: return "sys_ioctl";
        case Syscall::SYS_MMAP: return "sys_mmap";
        case Syscall::SYS_MMAP2: return "sys_mmap2";
        default: return "sys_unknown";
    }
}

} // namespace DemiEngine
