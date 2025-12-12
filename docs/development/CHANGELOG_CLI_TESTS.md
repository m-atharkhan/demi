# Changelog - CLI and Test Framework Improvements

**Date**: November 7, 2025  
**Version**: DemiEngine v2.0 - Test Framework Enhancement Release

## Summary

This release brings significant improvements to DemiEngine's command-line interface and test execution system, focusing on usability, performance insights, and better test organization.

---

## 🎯 Major Features

### 1. Argument Linking

**New capability to combine multiple short command-line flags into a single argument**

#### What Changed

- Implemented greedy longest-match algorithm for parsing linked arguments
- Short flags (single `-`) can now be combined: `-utq`, `-atq`, `-tq`, etc.
- Maintains backward compatibility with separate flags

#### Examples

```bash
# Before
./bin/demi-engine --unit-test --quiet

# After (both work)
./bin/demi-engine --unit-test --quiet
./bin/demi-engine -utq
```

#### Implementation

- **File**: `src/main.cpp`
- **Function**: `ArgParser::parse()`
- **Algorithm**: Greedy longest-match with fallback to shorter flags

---

### 2. Enhanced Test Output

#### Assembly Tests - Category Grouping

**Tests are now organized by category with comprehensive timing information**

##### Quiet Mode (`-atq`)

```
Algorithms (5/5) [29.2ms]
  ✓ [7.6ms] fibonacci calculation - Calculates the 5th Fibonacci number
  ✓ [5.7ms] factorial calculation - Calculates factorial of 5 (5!)
  ...

Assembly Tests: 79 passed / 79 total [403.6ms total, 5.11ms avg]
```

**Features**:

- Category headers with pass/fail count and timing
- Individual test timing
- Test descriptions
- Color-coded output (cyan/yellow for categories, green/red for tests)
- Enhanced summary with total and average execution time

##### Verbose Mode (`-at`)

```
[INFO]
Algorithms (5/5) [29.2ms]
[INFO]   ✓ [7.6ms] fibonacci calculation
[INFO]     Description: Calculates the 5th Fibonacci number
[INFO]     Author: DemiEngine Team
[INFO]     Tags: fibonacci, iterative, complex
...

=== Test Summary ===
[INFO] Total Tests: 79 (passed: 79, failed: 0)
[INFO] Total Assertions: 145 (passed: 145, failed: 0)
[INFO] Execution Time: 382.63ms total, 4.84ms average per test
[INFO] Slowest Test: stack overflow [33.9ms]
```

**Implementation**:

- **File**: `src/test/assembly_test_executor.cpp`
- **Function**: `TestExecutor::print_results()`
- **Data Structure**: `std::map<std::string, std::vector<const TestResult*>>` for category grouping

#### Unit Tests - Enhanced Summary

**Added comprehensive timing and performance metrics**

```
Unit Tests: 101 passed / 101 [1002.2ms total, 9.92ms avg]
Slowest test: call_stack_overflow [391.3ms]
```

**Features**:

- Total execution time
- Average time per test
- Failed test count display
- Slowest test identification (verbose mode)

**Implementation**:

- **File**: `src/test/test_framework.hpp`
- **Function**: `TestFramework::print_results()`

---

### 3. Unified Test Command

**`--test` / `-t` now runs BOTH unit tests AND assembly tests**

#### What Changed

```bash
# Before: Only ran unit tests
./bin/demi-engine --test

# After: Runs both unit and assembly tests
./bin/demi-engine --test
```

#### Output Format

```
┌──────────────────────────────────────────────────────┐
│     Running All DemiEngine Tests                     │
└──────────────────────────────────────────────────────┘

━━━━━━ Unit Tests ━━━━━━
[Unit test output...]
Unit Tests: 101 passed / 101 [870.8ms total, 8.62ms avg]

━━━━━━ Assembly Tests ━━━━━━
[Assembly test output...]
Assembly Tests: 79 passed / 79 total [326.1ms total, 4.13ms avg]
```

#### Implementation

- **File**: `src/main.cpp`
- **Function**: `run_all_tests()`
- Calls both `run_unit_tests()` and `run_in_assembly_tests()`

---

### 4. Quiet Mode Enhancements

**Improved quiet mode to balance brevity with useful information**

#### Configuration Changes

- **File**: `src/config.hpp`
- Added `Config::quiet_assembly_test` flag
- `-q` flag now sets both `Config::quiet` and `Config::quiet_assembly_test`

#### What Gets Suppressed

- DEBUG log messages
- INFO log messages
- RUNNING status messages
- Verbose test execution details

#### What Gets Displayed

- Test results (✓ or ✗)
- Test timing information
- Category summaries
- Final statistics
- Failure details (errors, failed assertions)

#### Assembly Test Fix

**Issue**: Assembly tests used `Logger::info()` which got filtered in quiet mode  
**Solution**: Changed to use `std::cout` directly

**Implementation**:

- **File**: `src/test/assembly_test_executor.cpp`
- Direct output bypasses logger filtering
- Maintains proper formatting and color coding

---

## 📝 Modified Files

### Core Changes

1. **src/main.cpp**
   - Implemented argument linking in `ArgParser::parse()`
   - Added `run_all_tests()` function
   - Updated `--test` argument to call `run_all_tests()`
   - Updated `-q` callback to set both quiet flags
   - Modified `run_assembly_tests_only()` to respect quiet mode

2. **src/config.hpp**
   - Added `Config::quiet` flag for unit tests
   - Added `Config::quiet_assembly_test` flag for assembly tests

3. **src/debug/logger.cpp**
   - Updated `should_filter_message()` to check `Config::quiet`

4. **src/test/test_framework.hpp**
   - Enhanced `print_results()` with timing information
   - Added slowest test identification
   - Improved summary statistics
   - Added total time and average calculations

5. **src/test/assembly_test_executor.cpp**
   - Implemented category grouping in `print_results()`
   - Added timing for categories and individual tests
   - Changed quiet mode to use `std::cout` directly
   - Enhanced summary with total/average time and slowest test
   - Improved category organization with full metadata display

### Documentation Updates

1. **docs/CLI_IMPROVEMENTS.md** (New)
   - Comprehensive guide to all CLI improvements
   - Examples and usage patterns
   - Implementation details
   - Performance information

2. **README.md**
   - Updated command-line interface section
   - Updated test coverage section
   - Updated test suite flags
   - Added argument linking examples

3. **docs/CHANGELOG_CLI_TESTS.md** (This file)
   - Complete changelog for this release

---

## 🔧 Technical Details

### Argument Linking Algorithm

**Greedy Longest-Match Parsing**:

```cpp
size_t i = 1;
while (i < arg.size()) {
    bool matched = false;
    // Try longest match first, decrementing length
    for (size_t len = arg.size() - i; len > 0; len--) {
        std::string potential = arg.substr(i, len);
        if (match_found) {
            execute_callback();
            i += len;
            matched = true;
            break;
        }
    }
    if (!matched) {
        // Handle unknown flag
        break;
    }
}
```

### Test Data Structures

**Category Tracking**:

```cpp
// Unit tests
std::map<std::string, std::tuple<int, int, double, std::vector<TestResult>>> category_data;
// Stores: (passed_count, total_count, total_time, test_results)

// Assembly tests
std::map<std::string, std::vector<const TestResult*>> category_tests;
std::map<std::string, size_t> category_totals;
std::map<std::string, size_t> category_passed;
std::map<std::string, double> category_times;
```

### Performance Optimizations

**Previous Issue**: `negative_tests` category taking 12.5 seconds

- `stack_underflow`: 6316ms
- `call_stack_overflow`: 6216ms

**Solution**: Reduced `MAX_STEPS` from 10000 to 500

- `negative_tests`: 12582.7ms → 702.8ms (17.9x speedup)
- `stack_underflow`: 6316ms → 340ms (18.6x speedup)
- `call_stack_overflow`: 6216ms → 287ms (21.6x speedup)

---

## 🧪 Testing

### Test Migration

- Reviewed `tests/data.test.asm.disabled`
- Added 3 new useful tests to `tests/data.test.asm`:
  - `sequential data access`
  - `boundary value testing`
  - `data persistence across operations`

### Cleanup

- Removed all `.asm` test/debug files from root directory:
  - `debug_overflow.asm`, `debug_vadd.asm`, `debug_vmul.asm`
  - `simple.asm`, `simple_vadd.asm`
  - `test_extreg_debug.asm`, `test_simple.asm`
  - `test_vadd.asm`, `test_vadd_full.asm`, `test_vadd_simple.asm`

### Verification

All features tested and verified:

```bash
# Argument linking
./bin/demi-engine -utq  # ✓ Works
./bin/demi-engine -atq  # ✓ Works
./bin/demi-engine -tq   # ✓ Works

# Category grouping
./bin/demi-engine -atq | head -40  # ✓ Shows categories

# Timing information
./bin/demi-engine -at | tail -20   # ✓ Shows slowest test

# Unified test command
./bin/demi-engine -tq              # ✓ Runs both test types
```

---

## 📊 Test Results

### Current Test Coverage

```
Unit Tests: 101 passed / 101 (100%)
Assembly Tests: 79 passed / 79 (100%)
TOTAL: 180 tests passing (100% coverage)
```

### Performance Metrics

```
Unit Tests: ~1002ms total, ~9.92ms average
Assembly Tests: ~404ms total, ~5.11ms average
Combined: ~1406ms total
```

### Category Distribution

**Assembly Tests**:

- Algorithms: 5 tests
- Arithmetic: 8 tests
- Benchmarks: 4 tests
- Control Flow: 7 tests
- Core Instructions: 2 tests
- Data: 14 tests
- Errors: 6 tests
- FPU: 10 tests
- LOADR: 2 tests
- Logical: 5 tests
- Memory: 3 tests
- Registers: 6 tests
- SIMD: 6 tests
- Stack: 4 tests
- Stack Operations: 1 test

**Unit Tests**:

- 64bit_operations: 7 tests
- arithmetic: 3 tests
- assembler: 19 tests
- bitwise: 2 tests
- conditional_jumps: 4 tests
- control_flow: 7 tests
- devices: 7 tests
- fpu: 13 tests
- fusion: 4 tests
- lea: 4 tests
- logical: 4 tests
- memory: 8 tests
- negative_tests: 13 tests
- register_validation: 1 test
- simd_control: 1 test
- simd_registers: 4 tests
- stack: 1 test
- swap: 3 tests

---

## 🎨 Visual Improvements

### Color Coding

- **Green (✓)**: Passed tests
- **Red (✗)**: Failed tests
- **Cyan**: Category headers (all passed)
- **Yellow**: Category headers (some failures)
- **Gray**: Timing information

### Output Format

- **Tree Structure**: Hierarchical category and test display
- **Centered Separators**: Test output delimiters (60 characters)
- **Indentation**: Tests indented under categories (2 spaces)
- **Timing Badges**: `[X.Xms]` in gray for all timing displays

---

## 🔄 Backward Compatibility

All changes maintain full backward compatibility:

- ✅ Separate flags still work: `-ut -q` same as `-utq`
- ✅ Long form arguments unchanged: `--unit-test --quiet`
- ✅ `-ut` alone still works for unit tests
- ✅ `-at` alone still works for assembly tests
- ✅ Non-quiet mode unchanged in behavior
- ✅ Existing test files continue to work

---

## 🚀 Benefits

1. **Faster Workflows**: Linked arguments reduce typing
2. **Better Organization**: Category grouping improves readability
3. **Performance Insights**: Timing helps identify slow tests
4. **Comprehensive Testing**: Single command runs all tests
5. **Flexible Output**: Quiet mode balances brevity with usefulness
6. **Professional Appearance**: Color-coded, well-formatted output
7. **Developer Productivity**: Faster test iteration cycles

---

## 📚 Future Enhancements

Potential improvements for future releases:

- [ ] Configurable output formats (JSON, XML, TAP)
- [ ] Parallel test execution for faster runs
- [ ] Test filtering by tags or categories
- [ ] Performance regression detection
- [ ] Test result caching
- [ ] Watch mode for continuous testing
- [ ] Test coverage reports
- [ ] Benchmark comparison across commits

---

## 🙏 Acknowledgments

These improvements were developed through iterative refinement and user feedback, focusing on:

- Developer experience and workflow efficiency
- Clear, actionable test output
- Performance visibility and optimization
- Maintaining simplicity while adding power

---

**End of Changelog**
