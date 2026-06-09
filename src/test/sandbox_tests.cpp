#include "test_framework.hpp"
// ── Sandbox Policy Tests ──

#include "../engine/sandbox_policy.hpp"

TEST_CASE(sandbox_disabled_allows_all, "sandbox") {
    demi::sandbox::SyscallDispatcher disp(false);
    using R = demi::sandbox::SyscallResult;
    if (disp.validate_syscall(1)  != R::ALLOWED) throw AssertionFailure("EXIT not allowed");
    if (disp.validate_syscall(3)  != R::ALLOWED) throw AssertionFailure("READ not allowed");
    if (disp.validate_syscall(4)  != R::ALLOWED) throw AssertionFailure("WRITE not allowed");
    if (disp.validate_syscall(5)  != R::ALLOWED) throw AssertionFailure("OPEN not allowed");
    if (disp.validate_syscall(2)  != R::ALLOWED) throw AssertionFailure("FORK not allowed");
    if (disp.validate_syscall(11) != R::ALLOWED) throw AssertionFailure("EXECVE not allowed");
}

TEST_CASE(sandbox_enabled_allows_exit, "sandbox") {
    demi::sandbox::SyscallDispatcher disp(true);
    if (disp.validate_syscall(1) != demi::sandbox::SyscallResult::ALLOWED)
        throw AssertionFailure("EXIT must be allowed in sandbox");
}

TEST_CASE(sandbox_enabled_vfs_io, "sandbox") {
    demi::sandbox::SyscallDispatcher disp(true);
    using R = demi::sandbox::SyscallResult;
    if (disp.validate_syscall(3) != R::HANDLED_INTERNALLY) throw AssertionFailure("READ");
    if (disp.validate_syscall(4) != R::HANDLED_INTERNALLY) throw AssertionFailure("WRITE");
    if (disp.validate_syscall(5) != R::HANDLED_INTERNALLY) throw AssertionFailure("OPEN");
    if (disp.validate_syscall(6) != R::HANDLED_INTERNALLY) throw AssertionFailure("CLOSE");
}

TEST_CASE(sandbox_enabled_denies_dangerous, "sandbox") {
    demi::sandbox::SyscallDispatcher disp(true);
    using R = demi::sandbox::SyscallResult;
    if (disp.validate_syscall(2)   != R::DENIED) throw AssertionFailure("FORK");
    if (disp.validate_syscall(11)  != R::DENIED) throw AssertionFailure("EXECVE");
    if (disp.validate_syscall(359) != R::DENIED) throw AssertionFailure("SOCKET");
}

TEST_CASE(sandbox_denies_unknown, "sandbox") {
    demi::sandbox::SyscallDispatcher disp(true);
    if (disp.validate_syscall(999) != demi::sandbox::SyscallResult::DENIED)
        throw AssertionFailure("unknown syscall must be denied");
}

TEST_CASE(sandbox_toggle, "sandbox") {
    demi::sandbox::SyscallDispatcher a(false), b(true);
    if (a.sandbox_enabled()) throw AssertionFailure("expected disabled");
    if (!b.sandbox_enabled()) throw AssertionFailure("expected enabled");
}
