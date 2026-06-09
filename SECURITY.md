# Security Policy for Demi

Demi is a virtualized compiler engine that executes code and produces machine
artifacts.
Security issues can have wide-reaching impact:
remote code execution, sandbox escapes, supply-chain risks, and corrupted build
artifacts.
This document explains how to report vulnerabilities and how we respond.

## Supported Versions

Only the latest commit on the `main` branch receives security updates.
Release branches and tags are frozen.
Report issues against any version — fixes land in `main`.

| Version | Supported          |
| ------- | ------------------ |
| main    | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

**Do not open a public issue.**
Use GitHub's private vulnerability reporting instead.

1. Go to **Security** tab → **Advisories** → **New draft security advisory**.
2. Or visit: https://github.com/bobrossrtx/demi/security/advisories/new
3. If you cannot use the advisory flow, open a private issue marked as
   "security" — do not include proof-of-concept code or sensitive details.
4. If none of the above work, contact the maintainer via the GitHub profile
   linked on the repository.

Include in your report:

- **Description**: a clear summary of the vulnerability.
- **Affected components**: list each one explicitly — engine frontend,
  virtual machine, runtime, build scripts, CI/CD pipeline, packaging,
  third-party dependencies. Do not use "etc." or leave items implied.
- **Reproduction**: step-by-step, minimal, and copy-pastable. Include the
  exact conditions or constraints needed (operating system, build flags,
  sandbox mode, input format).
- **Impact assessment**: list each impact explicitly — remote code execution,
  privilege escalation, data exfiltration, supply-chain compromise,
  denial of service. Do not use "etc."
- **Suggested mitigations**: if you have any.

If you must send attachments, share them through the private GitHub advisory
or a secure transfer link.
Do not attach files to public issues or emails unless the maintainer
explicitly requests it — ask for confirmation first.

Response time: within 72 hours.
Patch time: confirmed vulnerabilities are patched within 7 days.
Credit: reporters are acknowledged in the advisory and release notes unless
they request anonymity.

## Scope

Demi is a virtualized compiler engine.
Security-sensitive areas include, in order of priority:

1. **Sandbox escape** — breaking out of the VFS (Virtual File System) jail
   when `--sandbox` mode is active.
2. **Capability bypass** — circumventing `--allow-read`, `--allow-write`,
   `--allow-exec`, or `--allow-ioctl` flags.
3. **Memory safety** — buffer overflows, use-after-free, out-of-bounds
   access in the virtual machine runtime.
4. **Bytecode injection** — crafted `.hex` or `.asm` files that cause
   unintended behavior.
5. **Resource exhaustion** — denial of service via excessive memory
   allocation (if the request exceeds 100 MB without explicit user opt-in),
   CPU usage (if a single tick spins for more than 5,000,000 cycles without
   yielding), or disk usage (if the VFS container grows beyond 100 MB).
6. **C API (libdemi)** — unsafe usage patterns in the C embedding API that
   could affect host applications.

Sandbox escapes and capability bypasses are the highest priority.

## Disclosure Policy

- Reporters are credited in the advisory and release notes (opt-in).
- A **CVE (Common Vulnerabilities and Exposures)** identifier will be
  requested for critical vulnerabilities.
- Fixes are released on `main` immediately.
- Release tags follow within 24 hours.
- We coordinate a public disclosure timeline with the reporter.
  If a **CVE** is assigned, it is included in the advisory and commit
  messages.

Because Demi is a build-time tool, security issues can affect downstream
users.
When relevant, we will:

- Publish a **GitHub Security Advisory** with a clear summary and
  mitigation steps.
- Notify downstream consumers that embed Demi (if the vulnerability affects
  the C API or build output and the impact score is CVSS 7.0 or higher).
- Include detection guidance in the advisory when applicable.

## Development

When proposing major design changes that affect security — such as the
sandboxing model, **JIT (Just-In-Time)** compilation, or code loading —
open a design discussion that includes a security analysis and threat model.

## Acknowledgments

Contributors who responsibly disclose vulnerabilities are acknowledged
unless they request otherwise.
If a reporter asks to remain anonymous, their request is honored in all
public communications.
