#pragma once
#include "../assembler/ast.hpp"
#include "../assembler/demi_assembler.hpp"
#include "../engine/cpu.hpp"
#include "../debug/logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace Testing {

using namespace Assembler;
using Logging::Logger;

/**
 * Result of a single test assertion
 */
struct AssertionResult {
    bool passed;
    std::string assertion_type;
    std::string expected;
    std::string actual;
    std::string message;
    size_t line;
    size_t column;
    
    AssertionResult(bool p, const std::string& type, const std::string& exp, 
                   const std::string& act, size_t ln, size_t col)
        : passed(p), assertion_type(type), expected(exp), actual(act), line(ln), column(col) {
        if (!passed) {
            message = fmt::format("Expected {}, but got {}", expected, actual);
        }
    }
};

/**
 * Result of a single test case execution
 */
struct BenchmarkStats {
    std::vector<double> iteration_times;
    double min_time_ms = 0.0;
    double max_time_ms = 0.0;
    double avg_time_ms = 0.0;
    double median_time_ms = 0.0;
    double stddev_ms = 0.0;
    size_t iterations = 0;
    size_t warmup_iterations = 0;
    std::string measure_type = "time";
    
    void calculate_statistics() {
        if (iteration_times.empty()) return;
        
        iterations = iteration_times.size();
        
        // Calculate min, max, sum
        min_time_ms = *std::min_element(iteration_times.begin(), iteration_times.end());
        max_time_ms = *std::max_element(iteration_times.begin(), iteration_times.end());
        
        double sum = std::accumulate(iteration_times.begin(), iteration_times.end(), 0.0);
        avg_time_ms = sum / iterations;
        
        // Calculate median
        std::vector<double> sorted_times = iteration_times;
        std::sort(sorted_times.begin(), sorted_times.end());
        size_t mid = sorted_times.size() / 2;
        if (sorted_times.size() % 2 == 0) {
            median_time_ms = (sorted_times[mid-1] + sorted_times[mid]) / 2.0;
        } else {
            median_time_ms = sorted_times[mid];
        }
        
        // Calculate standard deviation
        double variance = 0.0;
        for (double time : iteration_times) {
            variance += (time - avg_time_ms) * (time - avg_time_ms);
        }
        variance /= iterations;
        stddev_ms = std::sqrt(variance);
    }
};

struct TestResult {
    std::string test_name;
    std::string description;
    std::string author;
    std::string category;
    std::vector<std::string> tags;
    bool passed;
    bool skipped;
    bool is_benchmark;
    std::vector<AssertionResult> assertions;
    std::string error_message;
    double execution_time_ms;
    BenchmarkStats benchmark_stats;
    
    TestResult(const std::string& name) 
        : test_name(name), passed(true), skipped(false), is_benchmark(false), execution_time_ms(0.0) {}
    
    void set_metadata(const std::string& desc, const std::string& auth, 
                     const std::string& cat, const std::vector<std::string>& test_tags) {
        description = desc;
        author = auth;
        category = cat;
        tags = test_tags;
    }
    
    void add_assertion(const AssertionResult& result) {
        assertions.push_back(result);
        if (!result.passed) {
            passed = false;
        }
    }
    
    void set_error(const std::string& error) {
        passed = false;
        error_message = error;
    }
    
    void set_skipped(bool skip) {
        skipped = skip;
        passed = skip;  // Skipped tests count as passed
    }
    
    void set_execution_time(double time_ms) {
        execution_time_ms = time_ms;
    }
    
    void set_benchmark(bool benchmark) {
        is_benchmark = benchmark;
    }
    
    size_t get_passed_count() const {
        return std::count_if(assertions.begin(), assertions.end(),
                           [](const AssertionResult& r) { return r.passed; });
    }
    
    size_t get_failed_count() const {
        return assertions.size() - get_passed_count();
    }
};

/**
 * Captures output from CPU device operations
 */
class OutputCapture {
public:
    void capture(uint8_t value) {
        buffer.push_back(value);
    }
    
    void capture_string(const std::string& str) {
        for (char c : str) {
            buffer.push_back(static_cast<uint8_t>(c));
        }
    }
    
    std::string get_output() const {
        return std::string(buffer.begin(), buffer.end());
    }
    
    void clear() {
        buffer.clear();
    }
    
private:
    std::vector<uint8_t> buffer;
};

/**
 * Executes in-assembly tests and evaluates assertions
 */
class TestExecutor {
public:
    TestExecutor() = default;
    
    /**
     * Execute all tests from an assembly source file
     * @param filename Path to assembly file containing tests
     * @return Vector of test results
     */
    std::vector<TestResult> execute_tests_from_file(const std::string& filename);
    
    /**
     * Execute a single test case
     * @param test_case The test case AST node
     * @param program_statements The main program statements (for context)
     * @return Test result
     */
    TestResult execute_test(const Assembler::TestCase& test_case, 
                           const std::vector<std::unique_ptr<Assembler::Statement>>& program_statements);
    
    /**
     * Print test results in a formatted way
     */
    void print_results(const std::vector<TestResult>& results);
    
private:
    DemiAssembler assembler;
    OutputCapture output_capture;
    std::string current_source;     // Store current source for extraction
    std::string current_file_path;  // Store current file path for include resolution
    
    /**
     * Evaluate a single assertion against CPU state
     */
    AssertionResult evaluate_assertion(const Assembler::TestAssertion& assertion, 
                                      const CPU& cpu,
                                      bool expect_error_occurred);
    
    /**
     * Assemble test code into bytecode
     */
    std::vector<uint8_t> assemble_test_code(const std::vector<std::unique_ptr<Assembler::Statement>>& statements);
    
    /**
     * Extract expected value from expression
     */
    int64_t get_expected_value(const Assembler::Expression& expr);
    
    /**
     * Get register number from expression
     */
    int get_register_number(const Assembler::Expression& expr);
    
    /**
     * Get ST register number from expression (for FPU testing)
     */
    int get_st_register_number(const Assembler::Expression& expr);
    
    /**
     * Get expected floating point value from expression
     */
    double get_expected_float_value(const Assembler::Expression& expr);
    
    /**
     * Get memory address from expression
     */
    uint32_t get_memory_address(const Assembler::Expression& expr);
};

} // namespace Testing
