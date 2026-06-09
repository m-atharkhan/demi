# Social Media Post Templates

Copy-paste-ready templates for promoting Demi. Replace `{version}` and customize as needed.

---

## Hacker News — "Show HN"

**Title:** Show HN: Demi — a customizable virtual machine and programming language in C++17

**Text:**
Demi is a virtualized compiler engine written in C++17. It's the backend for a programming language where every aspect — syntax, semantics, keywords, behavior — is customizable per project via a .toml config file.

What exists today:
- A 134-register VM with 94+ opcodes (SIMD, FPU, arithmetic, control flow, I/O)
- Full assembly toolchain: lexer → parser → assembler → bytecode
- Comprehensive test suite: 500+ tests, 0 failures
- Standalone ELF compilation (-o flag)
- Sandbox mode with VFS jail and capability flags
- ImGui debugger with memory inspection and step-through
- Embedding C API (libdemi.so / libdemi.a)
- Code quality integrations: Codacy, SonarCloud, CodeRabbit

What's next: native x86-64 codegen, high-level language frontend, JIT compilation.

I'm looking for contributors interested in compilers, VMs, and language design. Good first issues are tagged.

Repo: https://github.com/bobrossrtx/demi
Try it: `git clone && make && ./bin/demi-engine-debug -A examples/basic/hello.asm`

---

## Reddit — r/programming

**Title:** I built a virtual machine and assembler in C++17 — looking for contributors

**Text:**
Over the past year I've been building Demi, a virtualized compiler engine. The goal is to create a programming language where every aspect is customizable per project — the "Vim of programming languages."

Current state:
- VM: 134 registers, 94+ opcodes, SIMD, FPU
- Assembler: full pipeline, supports labels, directives, sections
- Debugger: ImGui visual debugger with step-through
- Sandbox: VFS jail with capability-based access control
- Tests: 500+ tests, 0 failures
- Tools: SonarCloud, CodeRabbit, Codacy integrated

Stack: C++17, g++, Makefiles, fmt

I'm looking for contributors — compiler hackers, VM enthusiasts, language design nerds. There are good first issues tagged and a detailed CONTRIBUTING.md.

https://github.com/bobrossrtx/demi

---

## Reddit — r/cpp

**Title:** Demi — a C++17 VM with 94+ opcodes, sandbox, and native ELF compilation

**Text:**
I wanted to share Demi, a virtualized compiler engine I've been building in C++17.

Technical highlights:
- 134-register architecture with SIMD (VADD, VMUL, VDOT, VMAX, etc.) and FPU (FADD, FSIN, FSQRT, etc.)
- Threaded instruction dispatch with predictive fallback
- Instruction fusion: combines adjacent ops at runtime
- Sandbox mode: VFS jail using .vfs containers + per-op capability flags
- Native ELF compilation: `demi-engine -o output in.asm`
- Embedding API: link libdemi and run Demi programs from your own app
- Build: zero-dependency Makefile, fmt bundled in extern/

Full source: https://github.com/bobrossrtx/demi
I'd love code review from experienced C++ devs.

---

## Twitter/X — Thread

**Tweet 1/5:**
I built a virtual machine and programming language backend in C++17.
It's called Demi — and the goal is to be "the Vim of programming languages": every aspect customizable per project.
https://github.com/bobrossrtx/demi

**Tweet 2/5:**
What exists:
• 134 registers, 94+ opcodes (SIMD, FPU, I/O)
• Full assembler (lexer→parser→bytecode)
• ImGui visual debugger
• Sandbox with VFS jail
• Native ELF compilation
• Embedding C API
500+ tests, 0 failures.

**Tweet 3/5:**
The architecture:
┌─────────────────────────────┐
│ .dem source (planned)       │
│   → Lexer → Parser → AST   │
├─────────────────────────────┤
│ Assembler (.asm → bytecode) │
│   → CPU (134 regs, 94+ ops) │
│   → Devices (console, file) │
│   → Sandbox (VFS jail)      │
└─────────────────────────────┘

**Tweet 4/5:**
Code quality is a priority:
• SonarCloud static analysis
• CodeRabbit AI reviews on every PR
• Codacy Cppcheck integration
• CI: build + test on every push
• 500+ tests, zero failures

**Tweet 5/5:**
Looking for contributors interested in:
• Compilers & codegen
• Virtual machines
• Language design
• Systems programming

Good first issues tagged. PRs welcome!

https://github.com/bobrossrtx/demi

---

## Dev.to Article — Outline

**Title:** Building a Virtual Machine from Scratch in C++17

**Sections:**
1. Why build a VM in 2026?
2. Architecture overview (registers, opcodes, memory)
3. The instruction dispatch loop (threaded vs predictive)
4. SIMD and FPU: vector ops in a soft VM
5. The sandbox: VFS jail with capability flags
6. Testing: 500+ tests with in-assembly assertions
7. What's next: native codegen, JIT, language frontend
8. How to contribute

**Code snippet hook:**
```assembly
; hello.asm — runs on Demi VM
.section .data
    msg: .string "Hello from Demi!\n"

.section .text
.global _start
_start:
    LOAD_IMM R0, 1
    LOAD_IMM R1, msg
    LOAD_IMM R2, 18
    OUT R0, R1
    HALT
```

---

## Lobsters

**Title:** Demi — a customizable VM and compiler backend in C++17

**Text:**
Demi is a virtualized compiler engine for a programming language where syntax, semantics, and behavior are customizable per project. The VM has 134 registers, 94+ opcodes (SIMD, FPU), instruction fusion, sandbox mode, and native ELF compilation. 500+ tests, zero failures. Looking for contributors interested in compilers, VMs, and language design.

https://github.com/bobrossrtx/demi
Tags: compilers, performance, c++
