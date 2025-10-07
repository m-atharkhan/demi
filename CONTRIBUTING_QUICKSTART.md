# Contributing to DemiEngine - Quick Start Guide

## 🎯 Perfect for New Contributors

DemiEngine is actively seeking contributors! We're in Stage 2 of development (Assembly Language Expansion) and have many **beginner-friendly tasks** available.

### 🌟 Why Contribute to DemiEngine?

- **Learn Systems Programming**: Work on CPU emulation, assembly language design, and virtual machines
- **100% Test Coverage**: Your contributions are protected by comprehensive tests
- **Clear Architecture**: Well-documented codebase with examples
- **Mentorship Available**: Core team provides guidance for new contributors
- **Impact**: Help build the foundation for a revolutionary programming language

### 🚀 Easy Ways to Get Started

#### 1. **Add New FPU Operations** (Beginner)
- Implement missing floating-point instructions
- Follow existing patterns in `src/engine/opcodes/`
- Each opcode is ~20-30 lines with tests included

#### 2. **Expand Assembly Examples** (Beginner)
- Create example programs in `examples/`
- Document usage patterns
- Write educational content

#### 3. **Improve Documentation** (Beginner)
- Add code comments
- Write tutorials
- Improve API documentation

#### 4. **Add SIMD Instructions** (Intermediate)
- Implement vector operations
- Work with 128-bit registers
- Performance-critical code

#### 5. **Test Framework Enhancements** (Intermediate)
- Improve test reporting
- Add performance benchmarks
- Extend assertion capabilities

### 🛠️ Setup in 5 Minutes

```bash
git clone https://github.com/bobrossrtx/demi.git
cd demi
make prereqs  # Installs dependencies
make build    # Builds everything
make test     # Runs test suite (should be 100% passing)
```

### 💡 Your First Contribution

1. Look for issues labeled `good-first-issue`
2. Comment "I'd like to work on this"
3. Fork the repo
4. Make your changes
5. Submit a PR

**We respond to new contributors within 24 hours!**

### 🤝 Join Our Community

- 💬 **Discord**: [Join our Discord server](#) (coming soon)
- 📧 **Email**: contributor@demiengine.org
- 🐦 **Twitter**: @DemiEngine

---

*Ready to help build the future of programming languages?*