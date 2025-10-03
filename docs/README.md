# DemiEngine Documentation

Welcome to the DemiEngine documentation! DemiEngine is a virtual machine and assembly toolchain that will serve as the foundation for the Demi programming language - featuring 134 registers, 63 implemented opcodes (151 planned), and a complete assembly development environment.

> 📋 **[Complete Documentation Index](DOCUMENTATION_INDEX.md)** - Full navigation guide with categorized document listings

## 📚 Documentation Structure

### 🚀 [Quick Reference](./QUICK_REFERENCE.md)
Fast lookup for commands, instruction set, implementation status, and common patterns. **Start here** for quick answers!

### 🛠️ [Development & Planning](./development/README.md)
Development roadmaps, future feature plans, and implementation specifications.

- **[Future Opcodes](./development/FUTURE_OPCODES.md)** - Complete plan for implementing 88 additional opcodes (SIMD, FPU, AVX, MMX)
- **[Demi Language Plan](./development/DEMI_LANGUAGE_PLAN.md)** - High-level language frontend specification (Stage 6)
- **[Native Compiler Plan](./development/NATIVE_COMPILER_PLAN.md)** - x86-64 code generation design

### 🧪 [Testing Documentation](./testing/README.md)
Complete testing framework documentation, test writing guides, and status reports.

- **[Test Framework Design](./testing/TEST_FRAMEWORK_DESIGN.md)** - Architecture and design
- **[Test Quick Reference](./testing/TEST_QUICK_REFERENCE.md)** - Running and writing tests
- **Current Status**: 188/188 tests passing (100% coverage)

### 💻 [Codebase Documentation](./codebase/README.md)
Technical documentation for developers working on DemiEngine itself. API references, architecture overviews, and implementation details.

- **[Architecture & Components](./codebase/README.md)** - Core system design
- **[API Reference](./codebase/API_REFERENCE.md)** - Complete class and function documentation
- **[Module Documentation](./codebase/modules/)** - Detailed module documentation (CPU, Assembler, Devices, etc.)

### 📖 [Usage Documentation](./usage/README.md)
User guides for programming the virtual computer. Writing assembly programs, understanding the instruction set, and working with devices.

### 🔧 [Troubleshooting Guide](./TROUBLESHOOTING.md)
Solutions for common problems, debugging tips, and best practices.

## ⚡ Quick Start

```bash
# Build DemiEngine
make

# Run assembly program
./bin/demi-engine -A examples/hello_world.asm

# Run hex program
./bin/demi-engine -H tests/hex/helloworld.hex

# Run all tests (188 tests)
./bin/demi-engine --test

# Run specific test category
./bin/demi-engine --unit-test           # 78 unit tests
./bin/demi-engine --assembly-test       # 68 assembly tests
./bin/demi-engine --integration-test    # 42 integration tests

# Show help
./bin/demi-engine --help
```

## 📊 Project Status

**Current Phase**: Assembly Language Expansion (Stage 1)

### Implemented Features ✅
- **134 Registers**: x86-64 style register architecture
- **63 Opcodes**: Core instruction set (arithmetic, logic, memory, control flow, I/O, stack)
- **Assembly Toolchain**: Lexer → Parser → Assembler → Bytecode
- **Device I/O**: Extensible device framework
- **Testing**: 188/188 tests passing (100% coverage)
- **Native Executables**: x86-64 ELF generation with embedded VM

### Planned Features 🔄
- **88 Additional Opcodes**: SIMD (26), FPU (23), AVX (20), MMX (11), Extended 64-bit (18)
- **Native Code Generation**: Direct x86-64 compilation (Stage 2)
- **Demi Language Frontend**: High-level language with customization (Stage 6)

**Total Opcodes**: 151 defined, 63 implemented (42% - expanding to 100% in Q1-Q2 2026)

## 📚 Additional Resources

- **Project Roadmap**: `/roadmap.md` - Complete development plan and future stages
- **Contributing**: `/CONTRIBUTING.md` - Development guidelines and contribution process
- **Examples**: `/examples/` - Example assembly programs
- **Tests**: `/tests/` - Comprehensive test suite

## 🆘 Getting Help

- **Quick answers**: [Quick Reference](./QUICK_REFERENCE.md)
- **Programming guide**: [Usage Documentation](./usage/README.md)
- **Technical details**: [Codebase Documentation](./codebase/README.md)
- **Common issues**: [Troubleshooting Guide](./TROUBLESHOOTING.md)
- **Test help**: [Testing Documentation](./testing/README.md)
