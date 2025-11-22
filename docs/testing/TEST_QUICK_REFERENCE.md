# DemiEngine Test Commands - Quick Reference

## 🚀 Quick Commands

| Command | Tests Run | Time | Use Case |
|---------|-----------|------|----------|
| `make test` | All (219) | ~5-7s | **Pre-commit check** |
| `./bin/demi-engine -ut` | Unit (101) | ~1-2s | **Rapid iteration** |
| `./bin/demi-engine -at` | Assembly (118) | ~2s | **Test file validation** |

## 📋 Full Commands

### All Tests
```bash
./bin/demi-engine --test
./bin/demi-engine -t
make test
```

### Unit Tests Only
```bash
./bin/demi-engine --unit-test
./bin/demi-engine -ut
```
**What it tests:** Assembler, parser, CPU validation, error handling, negative cases

### Assembly Tests Only
```bash
./bin/demi-engine --assembly-test
./bin/demi-engine -at
```
**What it tests:** Complete programs (.hex files), CPU instructions, device I/O

### Assembly Tests Only
```bash
./bin/demi-engine --assembly-test
./bin/demi-engine -at
```
**What it tests:** #test/#assert directive parsing, test structure validation

## 💡 Development Workflows

### Quick Feedback Loop
```bash
# Make change
vim src/assembler/assembler.cpp

# Quick test (1-2 seconds)
./bin/demi-engine -ut
```

### Testing CPU Changes
```bash
# Modify CPU
vim src/engine/opcodes/opcodes_consolidated.cpp

# Test execution
./bin/demi-engine -it
```

### Before Git Commit
```bash
# Full test suite
make test

# Or
./bin/demi-engine -t
```

## 📊 Test Statistics

- **Total Tests**: 182
- **Unit Tests**: 79
- **Assembly Tests**: 61 (validation)
- **Integration Tests**: 42
- **Pass Rate**: 100%

## 🎯 When to Use Each

| Scenario | Command | Why |
|----------|---------|-----|
| Active coding | `-ut` | Fastest feedback |
| CPU instruction work | `-it` | Tests real execution |
| Creating test files | `-at` | Validates syntax |
| Pre-commit | `-t` | Full coverage |
| CI/CD | All flags | Parallel execution |

## ⚡ Performance Tips

1. **Use `-ut` during development** - Fastest turnaround
2. **Run `-it` after CPU changes** - Catches execution bugs
3. **Use `-at` for test syntax** - Quick validation
4. **Save `-t` for pre-commit** - Comprehensive but slower

## 📖 More Information

- **Detailed Guide**: [docs/TEST_FLAGS.md](TEST_FLAGS.md)
- **Test Suite**: [tests/asm/README.md](../tests/asm/README.md)
- **CI Integration**: [docs/TEST_FLAGS.md#cicd-integration](TEST_FLAGS.md#cicd-integration)

---

**Tip**: Combine with `-d` for debug output: `./bin/demi-engine -ut -d`
