---
name: Bug report
about: Report a reproducible bug in demi (build, runtime, or tooling). Please include
  steps to reproduce and environment details.
title: "[BUG]"
labels: bug, triage
assignees: ''

---

**Before filing:** search existing issues and discussions; if this is a security-related issue, do NOT post PoC or exploit code in a public issue — see SECURITY.md and follow the private reporting instructions.

## Summary
A short, descriptive summary of the bug (what happened, where, and when).

## Environment
- demi version / commit SHA:
- Platform / OS and architecture (e.g., Ubuntu 24.04 x86_64, macOS arm64):
- Compiler / Toolchain versions (e.g., gcc/clang version, Python version if used):
- How you installed demi (from source, release artifact, package manager, etc.):
- Any relevant build flags, environment variables or CI environment:

## Components involved
(Select or describe) e.g., frontend parser, virtual machine, code generator, runtime, build scripts, packaging, CI, third-party dependency (name & version).

## Steps to reproduce
Provide a minimal, copy-pastable set of steps (terminal commands, code snippets, scripts, or a small repo) that reliably reproduces the issue:

1.
2.
3.

If possible, include a small reproducer link (GitHub repo, gist, or pastebin).

## Expected behavior
Describe what you expected to happen.

## Actual behavior
Describe what actually happened, including error messages, stack traces, or incorrect outputs. Paste relevant logs (sanitized) here or attach/upload them.

## Temporary workarounds or mitigation (if any)
If you found a workaround, please describe it.

## Impact assessment (optional)
How severe is this for you or downstream users? e.g., blocking development, corrupting artifacts, security-sensitive, CI failing.

## Additional context
Any other information that might help triage (e.g., related issues, recent changes, CI logs, test case references).

<!--
Maintainers: include links to CI run IDs or attach crash dumps when available to speed triage.
-->
