# DemiEngine Test Commands - Quick Reference

## 🚀 Quick Commands

| Command                 | Tests Run                           | Use Case                     |
| ----------------------- | ----------------------------------- | ---------------------------- |
| `make test`             | Full suite                          | Pre-commit / CI              |
| `./bin/demi-engine -ut` | Unit tests (145)                    | Fast dev feedback            |
| `./bin/demi-engine -at` | In-assembly tests (374; 3 skipped)  | VM/opcode + integration flow |

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

### In-Assembly Tests Only

```bash
./bin/demi-engine --assembly-test
./bin/demi-engine -at

# Quiet mode (results-focused)
./bin/demi-engine --assembly-test-quiet
./bin/demi-engine -atq
```

### Filter Test Output

`--test-filter` affects *output*, not which tests are executed.

```bash
./bin/demi-engine -ut --test-filter fails
./bin/demi-engine -ut --test-filter success   # alias: pass

./bin/demi-engine -at --test-filter fails
./bin/demi-engine -at --test-filter success   # alias: pass
```

## 📊 Current Test Counts

- Unit tests: 145/145 passing
- In-assembly tests: 371 passed, 3 skipped, 0 failed (374 total)
- Total: 516 passed, 3 skipped, 0 failed (519 total)
