# Command Line Interface Improvements

This document describes the recent enhancements to DemiEngine's command-line interface and test execution system.

## Table of Contents

1. [Argument Linking](#argument-linking)
2. [Enhanced Test Output](#enhanced-test-output)
3. [Unified Test Command](#unified-test-command)
4. [Quiet Mode Enhancements](#quiet-mode-enhancements)

---

## Argument Linking

### Overview

DemiEngine now supports **argument linking**, allowing multiple short flags to be combined into a single argument for convenience.

### How It Works

The argument parser uses a **greedy longest-match algorithm** to parse linked arguments:

- Attempts to match the longest possible flag first
- Falls back to shorter flags if no match is found
- Processes flags left-to-right

### Examples

```bash
# Run unit tests in quiet mode
./bin/demi-engine -utq
# Equivalent to: ./bin/demi-engine -ut -q

# Run assembly tests in quiet mode
./bin/demi-engine -atq
# Equivalent to: ./bin/demi-engine -at -q

# Run all tests in quiet mode
./bin/demi-engine -tq
# Equivalent to: ./bin/demi-engine -t -q

# Enable extended registers in debug mode
./bin/demi-engine -erd
# Equivalent to: ./bin/demi-engine -er -d
```

### Supported Linked Combinations

- `-utq` - Unit tests + Quiet mode
- `-atq` - Assembly tests + Quiet mode
- `-tq` - All tests + Quiet mode
- `-erd` - Extended registers + Debug mode
- `-utd` - Unit tests + Debug mode
- `-atd` - Assembly tests + Debug mode

### Implementation Details

**File**: `src/main.cpp` - `ArgParser::parse()`

The parser processes linked arguments by:

1. Identifying arguments starting with `-` (not `--`)
2. Attempting greedy matching starting from position 1
3. Calling callbacks for matched flags in order
4. Continuing with remaining characters after each match

```cpp
// Greedy longest-match for linked arguments
size_t i = 1;
while (i < arg.size()) {
    bool matched = false;
    // Try longest match first
    for (size_t len = arg.size() - i; len > 0; len--) {
        std::string potential = arg.substr(i, len);
        // Check if this matches a known flag...
    }
}
```

---

## Enhanced Test Output

### Assembly Tests - Category Grouping

Assembly tests now organize output by category, making it easier to understand test results.

#### Quiet Mode (`-atq`)

```
Algorithms (5/5) [29.2ms]
  ✓ [7.6ms] fibonacci calculation - Calculates the 5th Fibonacci number
  ✓ [5.7ms] factorial calculation - Calculates factorial of 5 (5!)
  ✓ [4.3ms] maximum of three numbers - Finds the maximum value
  ...

Arithmetic (8/8) [19.6ms]
  ✓ [2.5ms] addition - Tests basic addition
  ✓ [2.5ms] subtraction - Tests basic subtraction
  ...

Assembly Tests: 79 passed / 79 total [403.6ms total, 5.11ms avg]
```

**Features**:

- Tests grouped under category headers
- Category headers show: `Name (passed/total) [time]`
- Each test shows: `✓ [time] name - description`
- Color-coded: Cyan for all passed, Yellow for failures
- Timing for individual tests and categories
- Enhanced summary with total time and average

#### Verbose Mode (`-at`)

```
[INFO]
Algorithms (5/5) [29.2ms]
[INFO]   ✓ [7.6ms] fibonacci calculation
[INFO]     Description: Calculates the 5th Fibonacci number using iterative approach
[INFO]     Author: DemiEngine Team
[INFO]     Tags: fibonacci, iterative, complex
...

=== Test Summary ===
[INFO] Total Tests: 79 (passed: 79, failed: 0)
[INFO] Total Assertions: 145 (passed: 145, failed: 0)
[INFO] Execution Time: 382.63ms total, 4.84ms average per test
[INFO] Slowest Test: stack overflow [33.9ms]
[INFO] ✓ All tests passed!
```

**Features**:

- Full test metadata displayed
- Category grouping with detailed test information
- Author and tag information
- Slowest test identification
- Comprehensive summary statistics

### Unit Tests - Enhanced Summary

Unit tests now provide more detailed timing and performance information.

#### Example Output

```
Unit Tests: 101 passed / 101 [1002.2ms total, 9.92ms avg]
Slowest test: call_stack_overflow [391.3ms]
```

**Features**:

- Total execution time
- Average time per test
- Failed test count (when applicable)
- Slowest test identification (verbose mode)

---

## Unified Test Command

### Overview

The `--test` / `-t` command now runs **both** unit tests and assembly tests in a single execution.

### Usage

```bash
# Run all tests (unit tests + assembly tests)
./bin/demi-engine --test

# Run all tests in quiet mode
./bin/demi-engine -tq

# Run all tests with debug output
./bin/demi-engine -td
```

### Output Format

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

### Separate Test Commands

Individual test types can still be run separately:

```bash
# Unit tests only
./bin/demi-engine -ut
./bin/demi-engine -utq  # Unit tests, quiet mode

# Assembly tests only
./bin/demi-engine -at
./bin/demi-engine -atq  # Assembly tests, quiet mode
```

---

## Quiet Mode Enhancements

### Overview

Quiet mode has been enhanced to provide useful information while suppressing verbose logging.

### Configuration

**File**: `src/config.hpp`

Two separate quiet flags:

- `Config::quiet` - For unit tests
- `Config::quiet_assembly_test` - For assembly tests

The `-q` flag sets **both** flags automatically:

```cpp
parser.add_bool_arg("quiet", "--quiet", "-q", "...",
    [this](bool value) {
        Config::quiet = value;
        Config::quiet_assembly_test = value;
    });
```

### Quiet Mode Behavior

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

### Assembly Test Quiet Mode

**Implementation**: `src/test/assembly_test_executor.cpp`

Uses `std::cout` directly instead of `Logger::info()` to bypass log filtering:

```cpp
if (Config::quiet_assembly_test) {
    std::cout << fmt::format("\033[32m✓\033[0m \033[90m[{:.1f}ms]\033[0m {}",
                            result.execution_time_ms, result.test_name);
    if (!result.description.empty()) {
        std::cout << " - " << result.description;
    }
    std::cout << std::endl;
}
```

### Unit Test Quiet Mode

**Implementation**: `src/test/test_framework.hpp`

Categories are displayed with test lists underneath:

```cpp
if (Config::quiet) {
    // Show category breakdown with timing and tests underneath
    for (const auto& [cat, data] : category_data) {
        std::cout << fmt::format("{}  {:<20} {:>3}/{:<3} [{:>7.1f}ms]\033[0m\n",
                               cat_color, cat, passed_count, total_count, total_time);

        // Print individual tests under category
        for (const auto& result : cat_results) {
            // ... test output ...
        }
    }
}
```

---

## Performance Information

### Timing Display

All test modes now display timing information:

- **Individual tests**: `[X.Xms]` next to each test
- **Categories**: `[XXX.Xms]` in category headers
- **Summaries**: Total time and average per test

### Color Coding

- **Green (✓)**: Passed tests
- **Red (✗)**: Failed tests
- **Cyan**: Category headers (all passed)
- **Yellow**: Category headers (some failures)
- **Gray**: Timing information

### Performance Metrics Example

```
Assembly Tests: 79 passed / 79 total [403.6ms total, 5.11ms avg]
```

This shows:

- Total tests: 79
- Pass rate: 100%
- Total execution time: 403.6ms
- Average per test: 5.11ms

---

## Implementation Files

### Modified Files

1. **src/main.cpp**
   - Argument linking implementation
   - `run_all_tests()` function
   - Quiet flag callbacks

2. **src/config.hpp**
   - `Config::quiet` flag
   - `Config::quiet_assembly_test` flag

3. **src/debug/logger.cpp**
   - Quiet mode filtering in `should_filter_message()`

4. **src/test/test_framework.hpp**
   - Enhanced `print_results()` with timing
   - Category tracking and tree structure
   - Slowest test identification

5. **src/test/assembly_test_executor.cpp**
   - Category grouping in `print_results()`
   - Direct `std::cout` output in quiet mode
   - Enhanced summary statistics

---

## Testing the Improvements

### Verify Argument Linking

```bash
# Test unit tests + quiet
./bin/demi-engine -utq

# Test assembly tests + quiet
./bin/demi-engine -atq

# Test all tests + quiet
./bin/demi-engine -tq
```

### Verify Enhanced Output

```bash
# See category grouping (quiet)
./bin/demi-engine -atq | head -30

# See full details (verbose)
./bin/demi-engine -at | tail -30

# See all tests
./bin/demi-engine -t
```

### Verify Performance Metrics

```bash
# Check timing displays
./bin/demi-engine -atq | grep "ms"

# Check slowest test
./bin/demi-engine -at | grep "Slowest"
```

---

## Benefits

1. **Faster Workflow**: Linked arguments reduce typing
2. **Better Organization**: Category grouping improves readability
3. **Performance Insights**: Timing helps identify slow tests
4. **Comprehensive Testing**: Single command runs all tests
5. **Flexible Output**: Quiet mode balances brevity with usefulness

---

## Future Enhancements

Potential improvements for future versions:

- [ ] Configurable output formats (JSON, XML, TAP)
- [ ] Parallel test execution
- [ ] Test filtering by tags
- [ ] Performance regression detection
- [ ] Test result caching
- [ ] Watch mode for continuous testing
