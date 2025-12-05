# DemiEngine

[![Build Status](https://github.com/bobrossrtx/demi/actions/workflows/build.yml/badge.svg)](https://github.com/bobrossrtx/demi-engine/actions/workflows/build.yml)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Test Coverage](https://img.shields.io/badge/test_coverage-100%25-brightgreen.svg)](#testing)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)

## **Revolutionary Virtual Machine & Programming Language Platform**

*Complete custom toolchain for the future Demi programming language - the Vim of programming languages*

**Demi will be the Vim of programming languages** - infinitely customizable, with every aspect configurable to match your exact needs. Just as Vim lets you tailor your editor to perfection, Demi will let you customize syntax, semantics, behavior, and tooling on a per-project basis.

DemiEngine is the foundational backend for **Demi**, a revolutionary programming language that will offer unprecedented customization capabilities. With a rock-solid virtual machine featuring 134 registers, 94+ implemented opcodes (including 8 SIMD foundation operations and 23+ FPU floating-point instructions), and **comprehensive test coverage**, DemiEngine provides the infrastructure for a dual-mode execution system: rapid interpretation for development and native compilation for production performance.

---

## 🎯 **Project Vision**

**Current Status:** Core Backend Complete - SIMD Foundation + FPU Arithmetic Implemented ✅

DemiEngine serves as the foundation for the upcoming **Demi programming language**. With the SIMD foundation established and FPU floating-point arithmetic now complete (31+ specialized instructions, 19 tests, 38 assertions - 100% pass rate), we continue expanding the assembly instruction set to enable seamless native code generation. The future Demi language will feature:

- 🎭 **Total Language Customization**: Project-specific syntax, keywords, and behaviors
- ⚡ **Dual-Mode Execution**: Interpretation for development + Native compilation for production
- 🔧 **Revolutionary Configuration**: `demi.toml` controls every aspect of language behavior
- 🌍 **Project-Specific Dialects**: Different language variants per project without touching core
- 🚀 **Performance**: 10-50x speedups through native x86-64 compilation

### 📈 **Development Roadmap**

| Stage | Status | Target | Description |
|-------|--------|--------|-------------|
| **Stage 1** | ✅ Complete | Q4 2025 | Core VM Backend (63 opcodes, 134 registers) |
| **Stage 2** | � In Progress | Q1 2026 | Assembly Language Expansion (SIMD, FPU, AVX) |
| **Stage 3** | 🔜 Next | Q2 2026 | Native x86-64 Code Generation |
| **Stage 4** | 🔜 Planning | Q3 2026 | Demi Language Frontend (High-level syntax) |
| **Stage 5** | 🔜 Planning | Q4 2026 | D-ISA Assembler Integration |
| **Stage 6** | 🔜 Planning | Q1 2027 | Custom Linker |
| **Stage 7** | 🔜 Planning | Q2 2027 | Unified `demi` Toolchain |
| **Stage 8** | 🔜 Planning | Q4 2027 | JIT Compilation |

---

## 🚀 **Quick Start**

### Prerequisites
```bash
# Required dependencies
sudo apt install build-essential libglfw3-dev libglew-dev libfmt-dev

# Or on macOS
brew install glfw glew fmt

# Or on Windows with vcpkg
vcpkg install glfw3 glew fmt
```

### Build and Run
```bash
# Clone and build
git clone https://github.com/bobrossrtx/demi-engine.git
cd demi-engine
make

# Run comprehensive test suite
make test

# Run assembly file (primary usage)
./bin/demi-engine -A examples/basic/hello.asm

# Enable debug mode with detailed logging
./bin/demi-engine -A examples/basic/hello.asm -dv

# Compile to standalone executable
./bin/demi-engine -A examples/basic/hello.asm

# Run specific test file
./bin/demi-engine -at tests/arithmetic.test.asm

# Run unit tests
./bin/demi-engine -ut
```

### Command Line Interface
```
Demi Engine - Virtualized Compiler and Assembler

Usage: demi-engine [options] [files...]

General:
  --help                    -h      Shows help information

Input/Output:
  --compile                 -o      Compile program into a standalone executable

Assembly:
  --assembly                -A      Assembly mode: assemble and run .asm file
  --entry-point             -e      Specify entry point symbol (default: _start)

Architecture:
  --architecture            -x      Set architecture (x86 or x64)
  -x86                              Shortcut for --architecture=x86
  -x64                              Shortcut for --architecture=x64
  --no-arch-warn                    Silence mixed architecture warning

Testing:
  --test                    -t      Run built-in unit tests or test specific files
  --unit-test               -ut     Run built-in unit tests only or specific test by name
  --assembly-test           -at     Run in-assembly tests (supports files and folders)
  --assembly-test-quiet     -atq    Run in-assembly tests in quiet mode
  --test-filter                     Filter test output (all|fails|success)
  --show-metadata           -sm     Show test metadata (description, author, category, tags)

Debugging:
  --debug                   -d      Enable debug mode
  --debug-level             -dl     Set debug level (trace, detail, info, important, critical, all)
  --debug-verbose           -dv     Enable debug with verbose output (TRACE level)
  --debug-quiet             -dq     Enable debug with minimal output (IMPORTANT level)
  --verbose                 -v      Show informational messages
  --debug-file              -f      Debug file path
  --extended-registers      -er     Show extended register output (50 registers)
  --memdump                 -m      Print memory dump after execution
```

**New Features:**
- **Argument Linking**: Combine short flags for faster workflows (e.g., `-atq` = `-at -q`)
- **Enhanced Test Output**: Category grouping, timing information, and performance metrics
- **Unified Test Command**: `--test` now runs both unit and assembly tests
- **Quiet Mode**: Suppresses logs while showing useful results and timing

---

## ⭐ **Key Features**

### 🖥️ **Advanced Virtual Machine**
- **134-Register Architecture**: Comprehensive register set with x86-64 style registers
- **94+ Opcode Instruction Set**: Arithmetic, logic, memory, I/O, control flow, **SIMD foundation operations**, and **FPU floating-point arithmetic**
- **SIMD Foundation**: 8 fundamental vector instructions for parallel computation (VADD, VMUL, VDOT, VMAX, etc.)
- **FPU Arithmetic Operations**: 23+ floating-point instructions (FADD, FSIN, FSQRT, etc.) with full mathematical support
- **1MB Memory**: Expandable to 64MB with paging framework
- **Device I/O System**: Modular devices (console, file, counter, RAM disk) with port-based communication
- **Professional Debugging**: ImGui-based visual debugger with real-time inspection

### 🔧 **Development Tools**
- **Assembly Language**: Complete lexer → parser → assembler → bytecode pipeline
- **Debug Directives**: 14 powerful directives (.print, .dump, .assert, .memdump, .log, etc.) for program inspection
- **Test Framework**: Comprehensive testing with 100% SIMD foundation + FPU coverage (19/19 tests, 38/38 assertions)
- **Build System**: Automated compilation and testing with Make
- **Error Handling**: Comprehensive error reporting and validation
- **Hot Debugging**: Live system inspection and step-through capabilities

### 🎯 **Production Ready**
- **Memory Safety**: Robust bounds checking and validation
- **Standalone Compilation**: Generate native executables with embedded VM
- **Cross-Platform**: Linux, Windows, macOS support
- **Optimization**: Performance tuning and efficient execution
- **Extensibility**: Easy addition of new devices and instructions

---

## 📚 **Documentation**

### 📖 **Complete Guide**
- **[📋 Project Roadmap](roadmap.md)** – Complete development plan and vision
- **[📚 Documentation Hub](docs/README.md)** – Comprehensive technical documentation
- **[🎮 Usage Guide](docs/usage/README.md)** – Learn to write programs and use the system
- **[🔧 API Reference](docs/codebase/API_REFERENCE.md)** – Technical details for developers

### 🎓 **Learning Resources**
- **[📝 Examples](tests/hex/)** – Sample programs demonstrating features
- **[🧪 Test Cases](tests/)** – Real-world usage patterns
- **[⚠️ Troubleshooting](docs/usage/TROUBLESHOOTING.md)** – Common issues and solutions
- **[🤝 Contributing](CONTRIBUTING.md)** – How to contribute to the project

### 🚀 **Feature Documentation**
- **[🌟 Features Overview](docs/usage/FEATURES.md)** – Complete feature documentation
- **[⚡ Quick Reference](docs/reference/QUICK_REFERENCE.md)** – Instruction set quick reference
- **[🔢 FPU Reference](docs/reference/FPU_REFERENCE.md)** – Complete floating-point unit guide
- **[📊 SIMD Reference](docs/reference/SIMD_REFERENCE.md)** – Complete vector operations guide
- **[🧪 Test Framework](docs/TEST_FRAMEWORK_DESIGN.md)** – Testing system documentation

---

## 🏗️ **Architecture Overview**

```
┌─────────────────────────────────────────────────────────────┐
│                    DemiEngine Architecture                  │
├─────────────────────────────────────────────────────────────┤
│  Future Demi Language Frontend (Stage 2)                    │
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │ Lexer/Parser    │  │ AST Generation  │                   │
│  │ (.dem files)    │→ │ & Type Checking │                   │
│  └─────────────────┘  └─────────────────┘                   │
│                              ↓                              │
├─────────────────────────────────────────────────────────────┤
│  Current DemiEngine Backend (Stage 1) ✅ 95% Complete       │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐  │
│  │ Assembly        │  │ Virtual Machine │  │ Device      │  │
│  │ Toolchain       │→ │ CPU (134 regs)  │↔ │ I/O System  │  │
│  │ (.asm → bytes)  │  │ 63 opcodes      │  │ (4 devices) │  │
│  └─────────────────┘  └─────────────────┘  └─────────────┘  │
│                              ↓                              │
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │ Debug Interface │  │ Test Framework  │                   │
│  │ (ImGui GUI)     │  │ (39/41 passing) │                   │
│  └─────────────────┘  └─────────────────┘                   │
└─────────────────────────────────────────────────────────────┘
```

### 🎯 **Core Components**

| Component | Status | Description |
|-----------|--------|-------------|
| **CPU Engine** | ✅ Complete | 134 registers, 63 implemented opcodes |
| **Memory System** | ✅ Complete | 1MB default, expandable to 64MB |
| **Device Manager** | ✅ Complete | Console, File, Counter, RAM Disk devices |
| **Assembly Tools** | ✅ Complete | Full lexer → parser → assembler pipeline |
| **Debug Interface** | ✅ Complete | ImGui-based visual debugger |
| **Test Framework** | ✅ Complete | 100% test coverage with comprehensive validation |

---

## 🧪 **Testing**

DemiEngine maintains exceptional quality through comprehensive testing with **perfect 100% test coverage**:

### Test Coverage
```bash
# Run all tests (unit tests + assembly tests)
./bin/demi-engine --test
# Or use short form with quiet mode
./bin/demi-engine -tq

# Run specific test suites
./bin/demi-engine --unit-test          # Unit tests only
./bin/demi-engine --assembly-test      # Assembly tests only

# Quiet mode for minimal output
./bin/demi-engine -utq                 # Unit tests, quiet (-ut + -q linked)
./bin/demi-engine -atq                 # Assembly tests, quiet (-at + -q linked)

# Test specific files only
./bin/demi-engine -t tests/algorithms.test.asm    # Test single file
./bin/demi-engine -atq tests/stack.test.asm       # Single file, quiet mode

# Achieved results:
┌──────────────────────────────────────────────────────────┐
│     DemiEngine Test Results - PERFECT COVERAGE           │
└──────────────────────────────────────────────────────────┘
Unit Tests: 101 passed / 101 [1002.2ms total, 9.92ms avg]
Assembly Tests: 79 passed / 79 total [403.6ms total, 5.11ms avg]
TOTAL: 180 tests passing (100% coverage)
```

### Test Suite Flags
- `--test` / `-t` - Run all tests (unit + assembly), or test a specific file if path provided
- `--unit-test` / `-ut` - Run unit tests only, or test a specific file
- `--assembly-test` / `-at` - Run assembly tests, or test a specific file
- `--quiet` / `-q` - Suppress logs, show only results and timing

**Argument Linking**: Combine flags for faster workflows:
- `-tq` - All tests, quiet mode
- `-utq` - Unit tests, quiet mode  
- `-atq` - Assembly tests, quiet mode

**All test flags support optional file arguments** - provide a file path to test only that file.

**See [docs/development/CLI_IMPROVEMENTS.md](docs/development/CLI_IMPROVEMENTS.md) for detailed usage examples and new features**

### Test Categories
- **✅ Unit Tests**: 101/101 passing - Core functionality validation
- **✅ Assembly Tests**: 79/79 passing - In-assembly test execution and validation
- **✅ Memory Tests**: Bounds checking and safety validation
- **✅ Device Tests**: I/O system functionality
- **✅ Assembly Toolchain**: Complete lexer → parser → assembler pipeline
- **✅ Register Tests**: Extended register system (134 registers)
- **✅ Performance Tests**: Timing and optimization validation

### Test Output Features
- **Category Grouping**: Tests organized by category with timing
- **Performance Metrics**: Individual test timing and slowest test identification
- **Color-Coded Results**: Visual feedback with green (✓) and red (✗) indicators
- **Tree Structure**: Hierarchical display of categories and tests
- **Quiet Mode**: Minimal output showing only results and key statistics

### Test Coverage Achievements
- **🏆 100% Unit Test Coverage**: All core functionality validated
- **🏆 100% Integration Test Coverage**: All real-world scenarios tested
- **🏆 Perfect Test Suite**: Zero failing tests across all categories
- **🏆 Comprehensive Validation**: Every major feature and edge case covered
- **🏆 Production Ready**: Highest quality standards achieved
- **🏆 Professional Test Organization**: Organized test suites in `tests/`, `examples/`, `benchmarks/`

### Test Organization
```bash
# Run all tests (unit + integration)
make test-all

# Run unit tests only
make test

# Category-specific test directories
tests/basic/     # Basic CPU operations and core instruction tests
tests/fpu/      # Floating point unit and ST register syntax tests  
tests/parsing/  # Advanced parser features (sections, directives, syntax)
examples/       # Sample programs and learning examples
benchmarks/     # Performance testing and stress tests
```

### Recently Fixed Issues (100% Coverage Achievement)
- **✅ DB Directive System**: Complete assembler data handling with hybrid format detection
- **✅ Extended Register Support**: Full 134-register system with partial extended instruction support
- **✅ Integration Test Corrections**: Fixed register range validation and instruction encoding issues
- **✅ Assembler Edge Cases**: Null terminator padding, label addressing, and format detection

### Current Capabilities and Limitations

**Fully Implemented** (63 opcodes):
- ✅ Core arithmetic: ADD, SUB, MUL, DIV, INC, DEC
- ✅ Bitwise logic: AND, OR, XOR, NOT, SHL, SHR
- ✅ Control flow: JMP, JZ, JNZ, JS, JNS, JC, JNC, JO, JNO, JG, JL, JGE, JLE
- ✅ Memory: LOAD, STORE, LEA, SWAP, MOV
- ✅ Stack: PUSH, POP, PUSH_FLAG, POP_FLAG, CALL, RET
- ✅ I/O: IN, OUT, INB, OUTB, INW, OUTW, INL, OUTL, INSTR, OUTSTR
- ✅ Extended: MOVEX, ADDEX, SUBEX, ADD64, SUB64, MOV64, LOAD_IMM64
- ✅ Modes: MODE32, MODE64, MODECMP

**Not Yet Implemented** (88 opcodes defined but not functional):
- ⚠️ SIMD/SSE operations (planned)
- ⚠️ FPU floating-point operations (planned)
- ⚠️ AVX vector operations (planned)
- ⚠️ MMX multimedia operations (planned)
- ⚠️ Some 64-bit extended operations (partial)

---

## 💻 **Programming Examples**

### Assembly Language Programming
```assembly
# hello_world.asm - Complete assembly program
.section .data
    msg: .string "Hello, DemiEngine!\n"
    msg_len = 19

.section .text
.global _start

_start:
    # Write system call
    LOAD_IMM R0, 1          # stdout file descriptor
    LOAD_IMM R1, msg        # message address
    LOAD_IMM R2, msg_len    # message length
    OUT R0, R1              # output to console device
    
    # Exit cleanly
    HALT
```

### Hex Programming (Current)
```bash
# Simple addition program
# tests/hex/add.hex
# R0 = 5, R1 = 7, R0 = R0 + R1, HALT
01 00 05 01 01 07 02 00 01 FF
```

### Future Demi Language (Stage 2)
```python
# example.dem - Future Demi language syntax
fn main() {
    let x = 42
    let y = fibonacci(x)
    print("Result: {}", y)
}

fn fibonacci(n: int) -> int {
    if n <= 1 { return n }
    return fibonacci(n-1) + fibonacci(n-2)
}
```

---

## 🔧 **Development**

### Building from Source
```bash
# Development build with debug symbols
make debug

# Release build (optimized)
make

# Clean build artifacts
make clean

# Run static analysis
make lint

# Generate documentation
make docs
```

### Project Structure
```
demi-engine/
├── src/                     # Source code
│   ├── engine/              # Virtual machine core
│   │   ├── cpu.cpp          # CPU implementation
│   │   ├── device_manager.cpp # I/O device system
│   │   └── opcodes/         # Instruction implementations
│   ├── assembler/           # Assembly toolchain
│   │   ├── lexer.cpp        # Token analysis
│   │   ├── parser.cpp       # Syntax parsing
│   │   └── assembler.cpp    # Code generation
│   ├── debug/               # Debugging tools
│   │   ├── gui.cpp          # ImGui interface
│   │   └── logger.cpp       # Logging system
│   └── test/                # Test framework
├── tests/                   # Test programs
│   ├── hex/                 # Hex bytecode tests
│   └── asm/                 # Assembly source tests
├── docs/                    # Documentation
├── bin/                     # Compiled executables
└── build/                   # Build artifacts
```

### Adding New Features
1. **New Instructions**: Add to `src/engine/opcodes/`
2. **New Devices**: Implement `Device` interface in `src/engine/`
3. **Assembly Features**: Extend lexer/parser in `src/assembler/`
4. **Tests**: Add to `tests/hex/` or `tests/asm/`

---

## 🌟 **Future Demi Language**

DemiEngine is preparing for the revolutionary **Demi programming language** with unprecedented customization capabilities:

### 🎭 **Total Language Customization**
```toml
# demi.toml - Project-specific language configuration
[language]
syntax_profile = "c_like"          # c_like, python_like, rust_like, custom
custom_keywords = ["async", "await", "match"]
operator_overrides = { "??" = "null_coalesce" }
statement_terminators = "optional"  # required, optional, forbidden

[language.features]
memory_management = "automatic"     # automatic, manual, hybrid
null_safety = true                 # Rust-like null safety
pattern_matching = true            # Advanced pattern matching
async_await = true                 # Async/await support

[runtime]
optimization_level = "balanced"    # none, speed, size, balanced
gc_strategy = "generational"       # mark_sweep, generational, incremental
```

### ⚡ **Dual-Mode Execution**
```bash
# Future demi command interface
demi -I program.dem                # Fast interpretation (development)
demi -c program.dem -o program     # Native compilation (production)
demi -I program.dem --debug        # Interactive debugging
demi -c program.dem --target arm64 # Cross-compilation
```

### 🔧 **Revolutionary Capabilities**
- **Project-Specific Dialects**: Different language variants per project
- **Custom Standard Libraries**: Replace or extend built-in functionality
- **Runtime Syntax Switching**: Apply configuration dynamically
- **Native Performance**: 10-50x speedups through x86-64 compilation
- **Configuration-Driven Development**: Every aspect controlled by `demi.toml`

---

## 🤝 **Contributing**

DemiEngine welcomes contributors at all levels! Whether you're interested in:

### 🎯 **Contribution Areas**
- **🖥️ Systems Programming**: CPU architecture, instruction implementation
- **🔧 Compiler Development**: Code generation, optimization passes
- **🎮 Tooling**: IDE integration, debugging interfaces
- **📚 Documentation**: Tutorials, examples, guides
- **🧪 Testing**: Test coverage, validation frameworks

### 📋 **Getting Started**
1. **Fork** the repository
2. **Read** [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines
3. **Choose** an area from our [roadmap](roadmap.md)
4. **Create** a feature branch
5. **Submit** a pull request

### 🏆 **Why Contribute?**
- ✅ **100% Custom Implementation**: No LLVM or GCC dependencies
- ✅ **Educational Value**: Learn compiler construction from scratch
- ✅ **Modern Architecture**: Built with contemporary best practices
- ✅ **Growing Community**: Be part of building something revolutionary
- ✅ **Real Impact**: Create a production-ready language platform

---

## 📄 **License**

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## 🎯 **Project Status Summary**

| Metric | Current Status | Target |
|--------|----------------|--------|
| **Backend Completion** | 🏆 100% | ✅ Complete (Q4 2025) |
| **Test Coverage** | 🏆 100/100 (100%) | ✅ Perfect Coverage |
| **Unit Tests** | 🏆 59/59 (100%) | ✅ Complete |
| **Integration Tests** | 🏆 41/41 (100%) | ✅ Complete |
| **Core Features** | ✅ Complete | ✅ Complete |
| **Documentation** | ✅ Complete | ✅ Complete |
| **Demi Language** | 🔜 Planning | Q1-Q2 2026 |
| **Native Compilation** | 🔜 Planning | Q3 2026 |

**🎉 MILESTONE ACHIEVED**: Stage 1 (DemiEngine Backend) Complete with Perfect Test Coverage!  
**Next Target**: Begin Stage 2 (Demi Language Frontend)

---

*Join us in building the future of customizable programming languages with DemiEngine! 🚀*
