# DemiEngine Documentation Index

## 🧭 Complete Navigation Guide

This documentation index provides a comprehensive guide to all DemiEngine documentation. Use the table and directory structure below to quickly locate any document, whether current, planned, or archived.

- **Quick Navigation Table:** Jump to any major document by category.
- **Directory Structure:** Visual map of all documentation files and folders.
- **Task-Based Guides:** Find what you need based on your current goal.
- **Archive:** Access historical, deprecated, or verification documents in `archive/`:
  ```
  archive/
  ├── README.md
  ├── RENAME_CHECKLIST.md
  ├── TEST_FLAGS.md.old
  ├── DOCUMENTATION_VERIFICATION_FINDINGS.md
  ├── DOCUMENTATION_CORRECTIONS_SUMMARY.md
  └── DOCUMENTATION_UPDATE_SUMMARY.md
  ```

Refer to the sections below for detailed navigation and descriptions of each document.

---

## 📚 Quick Navigation

| Category            | Document                                                       | Description                            |
| ------------------- | -------------------------------------------------------------- | -------------------------------------- |
| **Getting Started** | [Main README](../README.md)                                    | Project overview and quick start       |
|                     | [Quick Reference](reference/QUICK_REFERENCE.md)                | Fast command and instruction lookup    |
|                     | [Usage Guide](usage/README.md)                                 | Programming guide and tutorials        |
| **Development**     | [Roadmap](../roadmap.md)                                       | Complete development roadmap           |
|                     | [Future Opcodes](development/FUTURE_OPCODES.md)                | 88 planned opcodes implementation plan |
|                     | [Demi Language Plan](development/DEMI_LANGUAGE_PLAN.md)        | High-level language specification      |
|                     | [Native Compiler Plan](development/NATIVE_COMPILER_PLAN.md)    | x86-64 code generation plan            |
|                     | [Debug Directives](DEBUG_DIRECTIVES_IMPLEMENTATION.md)         | 14 debug directives implementation     |
| **Testing**         | [Test Framework Design](testing/TEST_FRAMEWORK_DESIGN.md)      | Complete testing architecture          |
|                     | [Test Quick Reference](testing/TEST_QUICK_REFERENCE.md)        | Running and writing tests              |
|                     | [Test Flags](testing/TEST_FLAGS.md)                            | CPU flags testing guide                |
|                     | [Test Status](testing/TEST_FRAMEWORK_STATUS.md)                | Current test status (516 passed, 3 skipped, 0 failed; 519 total) |
| **Codebase**        | [Architecture](codebase/README.md)                             | Core system design                     |
|                     | [API Reference](codebase/API_REFERENCE.md)                     | Complete API documentation             |
|                     | [Error Handling](development/ERROR_HANDLING_IMPLEMENTATION.md) | Error & debug system implementation    |
|                     | [CPU Module](codebase/modules/cpu.md)                          | CPU implementation details             |
|                     | [Assembler Module](codebase/modules/assembler.md)              | Assembler architecture                 |
|                     | [Device Manager](codebase/modules/device_manager.md)           | Device I/O system                      |
| **Support**         | [Troubleshooting](usage/TROUBLESHOOTING.md)                    | Common issues and solutions            |
|                     | [Contributing](../CONTRIBUTING.md)                             | Development guidelines                 |

---

## 📂 Directory Structure

```
docs/
├── README.md                    # Main documentation index (YOU ARE HERE)
├── DEBUG_DIRECTIVES_IMPLEMENTATION.md # Debug directives (14 total)
│
├── reference/                   # Reference documentation
│   ├── QUICK_REFERENCE.md       # Fast lookup reference
│   ├── FPU_REFERENCE.md         # FPU instruction reference
│   ├── SIMD_REFERENCE.md        # SIMD instruction reference
│   └── SIMD_COMPARISON.md       # SIMD comparison guide
│
├── development/                 # Development plans and specifications
│   ├── README.md
│   ├── FUTURE_OPCODES.md       # Opcode implementation plan (88 opcodes)
│   ├── DEMI_LANGUAGE_PLAN.md   # Demi language specification
│   ├── NATIVE_COMPILER_PLAN.md # Native codegen design
│   ├── REGISTER_IMPLEMENTATION_NOTES.md # Register architecture notes
│   ├── ERROR_HANDLING_IMPLEMENTATION.md # Error handling implementation
│   └── DEVELOPMENT_PLAN_UPDATE.md
│
├── testing/                     # Testing documentation
│   ├── README.md
│   ├── TEST_FRAMEWORK_DESIGN.md      # Framework architecture
│   ├── TEST_QUICK_REFERENCE.md       # Quick testing guide
│   ├── TEST_FLAGS.md                 # Flags testing
│   ├── TEST_FRAMEWORK_STATUS.md      # Current status
│   ├── TEST_ENHANCEMENT_SUMMARY.md   # Enhancement history
│   ├── NEGATIVE_TEST_IMPLEMENTATION.md
│   ├── TEST_EXECUTION_COMPLETE.md    # Historical: Test completion
│   ├── TEST_SUITE_SUMMARY.md         # Historical: Suite summary
│   ├── TEST_STATUS_SUMMARY.md        # Historical: Status summary
│   ├── TEST_FLAG_IMPLEMENTATION.md   # Historical: Flag implementation
│   └── VALIDATION_TEST_RESULTS.md    # Historical: Validation results
│
├── codebase/                    # Technical/API documentation
│   ├── README.md
│   ├── API_REFERENCE.md
│   └── modules/
│       ├── cpu.md
│       ├── assembler.md
│       ├── device_manager.md
│       ├── debug.md
│       ├── main.md
│       └── testing.md
│
├── usage/                       # User programming guides
│   ├── README.md
│   ├── FEATURES.md              # Features guide
│   └── TROUBLESHOOTING.md       # Problem solving guide
│
└── archive/                     # Historical/deprecated docs
    ├── README.md
    ├── RENAME_CHECKLIST.md
    └── TEST_FLAGS.md.old
```

---

## 🎯 Documentation by Task

### I want to...

#### Learn DemiEngine

1. Start with [Main README](../README.md)
2. Read [Quick Reference](reference/QUICK_REFERENCE.md)
3. Follow [Usage Guide](usage/README.md)
4. Try [Example Programs](../examples/README.md)

#### Write Assembly Programs

1. Read [Usage Guide](usage/README.md)
2. Check [Quick Reference](reference/QUICK_REFERENCE.md) for instructions
3. Browse [Example Programs](../examples/README.md)
4. See [Test Programs](../tests/asm/README.md) for more examples

#### Understand the Codebase

1. Start with [Architecture Overview](codebase/README.md)
2. Review [API Reference](codebase/API_REFERENCE.md)
3. Read specific [Module Documentation](codebase/modules/)
4. Check [Contributing Guide](../CONTRIBUTING.md)

#### Write Tests

1. Read [Test Framework Design](testing/TEST_FRAMEWORK_DESIGN.md)
2. Follow [Test Quick Reference](testing/TEST_QUICK_REFERENCE.md)
3. Study [Test Flags Guide](testing/TEST_FLAGS.md)
4. Look at existing tests in `/tests/`

#### Understand Future Plans

1. Read [Project Roadmap](../roadmap.md)
2. Review [Future Opcodes Plan](development/FUTURE_OPCODES.md)
3. Check [Demi Language Plan](development/DEMI_LANGUAGE_PLAN.md)
4. See [Native Compiler Plan](development/NATIVE_COMPILER_PLAN.md)

#### Troubleshoot Issues

1. Check [Troubleshooting Guide](usage/TROUBLESHOOTING.md)
2. Review [Test Status](testing/TEST_FRAMEWORK_STATUS.md)
3. Search [Issue Tracker](https://github.com/bobrossrtx/demi/issues)
4. Ask in discussions

#### Contribute Code

1. Read [Contributing Guide](../CONTRIBUTING.md)
2. Review [Architecture Overview](codebase/README.md)
3. Check [Test Framework](testing/TEST_FRAMEWORK_DESIGN.md)
4. Study [API Reference](codebase/API_REFERENCE.md)

---

## 📊 Current Project Status

**Phase**: Stage 1 - Assembly Language Expansion  
**Focus**: Implementing 88 additional opcodes

### Implementation Status

- ✅ **Core Backend**: Complete (134 registers, 63 opcodes, 188 tests)
- 🔄 **Opcode Expansion**: Planning (88 opcodes: SIMD, FPU, AVX, MMX)
- 🔜 **Native Codegen**: Next phase (Stage 2)
- 🔜 **Demi Language**: Future phase (Stage 6)

### Key Metrics

- **Opcodes**: 63 implemented / 151 defined (42%)
- **Registers**: 134 (x86-64 style architecture)
- **Tests**: 516 passed, 3 skipped, 0 failed (519 total)
  - Unit: 145 passed, 0 failed (145 total)
  - In-assembly: 371 passed, 3 skipped, 0 failed (374 total)

---

## 🔗 External Resources

- **Repository**: https://github.com/bobrossrtx/demi
- **Issues**: https://github.com/bobrossrtx/demi/issues
- **Discussions**: https://github.com/bobrossrtx/demi/discussions
- **License**: MIT (see [LICENSE](../LICENSE))

---

## 📝 Document Status

| Document                      | Status     | Last Updated |
| ----------------------------- | ---------- | ------------ |
| README.md                     | ✅ Current | 2025-10-03   |
| reference/QUICK_REFERENCE.md  | ✅ Current | 2025-10-03   |
| development/FUTURE_OPCODES.md | ✅ Current | 2025-10-03   |
| testing/\*                    | ✅ Current | 2025-10-03   |
| codebase/\*                   | ✅ Current | 2025-10-03   |
| usage/\*                      | ✅ Current | 2025-10-03   |

---

**Last Updated**: October 3, 2025  
**Documentation Version**: 2.0 (Reorganized structure)
