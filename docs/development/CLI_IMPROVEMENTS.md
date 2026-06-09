# CLI Improvements and Sandbox Flags

## Sandbox Flags

The following sandbox and capability flags control guest program access:

```bash
--sandbox         Enable sandbox mode (VFS jail, restricts I/O and exec)
--allow-read      Allow real filesystem reads, stat, and fstat
--allow-write     Allow real filesystem writes and lseek
--allow-exec      Allow fork+execve
--allow-ioctl     Allow raw ioctl syscalls
```

These flags are processed in Phase 1 (boolean flags) to ensure they take
effect before test flags that may exit early. Sandbox setup is wired in
both the main execution path and the assembly test executor.

When `--sandbox` is active, tests in the "Sandbox" category are auto-skipped
unless the flag is set, preventing false failures in the default test suite.

### Capability Matrix

| Flag | Effect in sandbox |
|------|------------------|
| (none) | VFS jail — only SYS_EXIT and stdout always work |
| `--allow-read` | Real FS reads, stat, fstat bypass VFS |
| `--allow-write` | Real FS writes, lseek bypass VFS |
| `--allow-exec` | fork+execve permitted |
| `--allow-ioctl` | Raw ioctl permitted |

### Syscalls Blocked in Sandbox

| Syscall | Reason |
|---------|--------|
| SYS_SOCKET (359) | Network access |
| SYS_BIND (361) | Network access |
| SYS_CONNECT (362) | Network access |
| Unknown/unmapped | Default deny |

### Syscalls Added (2026-06-09)

| Syscall | Number | Gated behind |
|---------|--------|-------------|
| SYS_LSEEK | 8 | --allow-read or --allow-write |
| SYS_STAT | 106 | --allow-read |
| SYS_FSTAT | 108 | --allow-read |

## Future Enhancements

- [ ] Configurable output formats (JSON, XML, TAP)
- [ ] Parallel test execution
- [ ] Test filtering by tags
- [ ] Performance regression detection
- [ ] Test result caching
- [ ] Watch mode for continuous testing
