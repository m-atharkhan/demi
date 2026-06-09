#include <memory>
#include "assembly_test_executor.hpp"
#include "../assembler/lexer.hpp"
#include "../engine/sandbox_policy.hpp"
#include "../engine/vfs.hpp"
#include "../assembler/parser.hpp"
#include "../debug/error_handler.hpp"
#include "../debug/debug_handler.hpp"
#include "../debug/hexdumper.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem>
#include <cctype>

namespace Testing {

std::vector<TestResult> TestExecutor::execute_tests_from_file(const std::string& filename) {
    std::vector<TestResult> results;
    
    // Enable test mode for this test execution
    bool previous_test_mode = Config::test_mode;
    Architecture previous_architecture = Config::architecture;
    Config::test_mode = true;
    
    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << fmt::format("Failed to open test file: {}", filename) << std::endl;
        Config::test_mode = previous_test_mode; // Restore previous state
        Config::architecture = previous_architecture; // Restore previous architecture
        return results;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();
    
    // Store source for extraction
    current_source = source;
    current_file_path = filename;
    
    // Lex and parse - suppress error logging during test file parsing since
    // parser errors from unimplemented features (macros, etc.) are expected
    Logging::ErrorHandler::instance().set_quiet_mode(true);
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    // Re-enable error logging after parsing
    Logging::ErrorHandler::instance().set_quiet_mode(false);
    
    if (lexer.has_errors()) {
        // Parser errors from unimplemented features (macros, conditionals) are expected
        // Only report them if in very verbose mode (test_mode = false)
        if (!Config::test_mode) {
            std::cerr << "Lexer errors in test file:" << std::endl;
            for (const auto& error : lexer.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
        }
        Config::test_mode = previous_test_mode; // Restore previous state
        Config::architecture = previous_architecture; // Restore previous architecture
        return results;
    }
    
    if (parser.has_errors()) {
        // Parser errors from unimplemented features (macros, conditionals) are expected
        // Only report them if in very verbose mode (test_mode = false)
        if (!Config::test_mode) {
            std::cerr << "Parser errors in test file:" << std::endl;
            for (const auto& error : parser.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
        }
        // Don't return early - try to extract any test cases that were successfully parsed
        // The program pointer may still contain valid test cases despite parse errors
    }
    
    // Execute each test
    if (!Config::quiet_assembly_test) {
        std::cout << fmt::format("\033[1;36m=== Running tests from {} ===\033[0m", filename) << std::endl;
    }
    
    for (const auto& test : program->test_cases) {
        results.push_back(execute_test(*test, program->statements));
    }
    
    // Add spacing after test execution for better readability
    if (!Config::quiet_assembly_test && !results.empty()) {
        std::cout << std::endl;
    }
    
    Config::test_mode = previous_test_mode; // Restore previous state
    Config::architecture = previous_architecture; // Restore previous architecture
    return results;
}

TestResult TestExecutor::execute_test(const Assembler::TestCase& test_case, 
                                      [[maybe_unused]] const std::vector<std::unique_ptr<Assembler::Statement>>& program_statements) {
    // Set current test name for extraction
    current_test_name = test_case.name;
    
    TestResult result(test_case.name);
    result.set_metadata(test_case.description, test_case.author, test_case.category, test_case.tags);

    const bool defer_test_line_output = (Config::test_show_mode != TestShowMode::ALL);
    
    // Check if test is marked to skip
    if (test_case.get_skip()) {
        if (!defer_test_line_output && !Config::quiet_assembly_test) {
            std::cout << test_case.name << " (skipped)" << std::endl;
        }
        result.skipped = true;
        result.passed = false;  // Mark as not passed since it was skipped
        return result;
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // In ALL mode, print the test name immediately (progress-style output).
    // In filtered modes, defer printing until we know whether this test matches the filter.
    if (!defer_test_line_output) {
        std::cout << test_case.name << " ";
        std::cout.flush();
    }

    
    try {
        // First, assemble test code to process .memory directive
        std::vector<uint8_t> bytecode = assemble_test_code(test_case.body);
        
        DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Bytecode size: {}", bytecode.size());

        if (assembler.has_errors()) {
            std::stringstream error_msg;
            error_msg << "Assembly errors:\n";
            for (const auto& error : assembler.get_errors()) {
                error_msg << "  " << error << "\n";
            }
            result.set_error(error_msg.str());
            return result;
        }
        
        // Create CPU instance with memory size from .memory directive
        size_t test_memory_size = 512; // Default for test compatibility
        if (assembler.get_memory_size() > 0) {
            test_memory_size = assembler.get_memory_size();
        }
        CPU cpu(test_memory_size);  // Use configurable memory size

        // Wire sandbox if enabled via CLI
        std::unique_ptr<demi::sandbox::SyscallDispatcher> _test_sd;
        std::unique_ptr<demi::sandbox::VirtualFileSystem> _test_vfs;
        if (Config::sandbox_enabled) {
            _test_sd = std::make_unique<demi::sandbox::SyscallDispatcher>(true);
            _test_vfs = std::make_unique<demi::sandbox::VirtualFileSystem>(
                "/tmp/demi_vfs", true);
            cpu.set_sandbox_environment(_test_sd.get(), _test_vfs.get());
        }

        output_capture.clear();
        
        // Note: Test metadata will be printed after test execution based on filter mode
        
        // Execute the test code
        bool expect_error = false;
        bool error_occurred = false;

        // Check if we expect an error beforehand to suppress logging
        for (const auto& stmt : test_case.body) {
            if (stmt->type == Assembler::ASTNodeType::TEST_ASSERTION) {
                const auto* assertion = static_cast<const Assembler::TestAssertion*>(stmt.get());
                if (assertion->assertion_type == Assembler::TestAssertionType::EXPECT_ERROR) {
                    expect_error = true;
                    break;
                }
            }
        }

        if (expect_error) {
            Logging::DebugHandler::instance().set_suppress_output(true);
        }
        
        try {
            cpu.reset();
            
            // Apply test case configuration
            if (test_case.max_call_depth > 0) {
                cpu.set_max_call_depth_override(test_case.max_call_depth);
            }
            
            // Determine entry point
            uint32_t entry_address = assembler.get_entry_address();
            if (!test_case.entry_point.empty()) {
                // Try to parse as number
                try {
                    size_t pos;
                    entry_address = std::stoul(test_case.entry_point, &pos, 0);
                    if (pos != test_case.entry_point.length()) {
                        throw std::invalid_argument("Not a number");
                    }
                } catch (...) {
                    // Try to resolve as label
                    const auto& symbols = assembler.get_symbols();
                    auto it = symbols.find(test_case.entry_point);
                    if (it != symbols.end()) {
                        entry_address = it->second.address;
                    } else {
                        result.set_error("Entry point label '" + test_case.entry_point + "' not found");
                        return result;
                    }
                }
            }
            
            cpu.execute(bytecode, entry_address, test_case.max_steps);
            
            // IMPORTANT: Sync legacy registers to new register system
            // The opcodes write to legacy_registers but get_register() reads from registers array
            cpu.sync_from_legacy_registers();

            if (Config::memdump) {
                DEBUG_INFO(Logging::DebugCategory::MEM_ACCESS, "Post-execution: printing memory...{}", "");
                size_t mem_size = cpu.get_memory().size();
                if (mem_size > 0) {
                    // Print first 256 bytes or full memory if smaller
                    cpu.print_memory(0, std::min<size_t>(mem_size, 256));
                }
            }

            if (expect_error) {
                Logging::DebugHandler::instance().set_suppress_output(false);
            }
        } catch (const std::exception& e) {
            if (expect_error) {
                Logging::DebugHandler::instance().set_suppress_output(false);
            }
            
            // Print memory dump on error if requested
            if (Config::memdump) {
                DEBUG_INFO(Logging::DebugCategory::MEM_ACCESS, "Error occurred: printing memory...{}", "");
                size_t mem_size = cpu.get_memory().size();
                if (mem_size > 0) {
                    cpu.print_memory(0, std::min<size_t>(mem_size, 256));
                }
            }
            
            error_occurred = true;
            
            if (!expect_error) {
                result.set_error(fmt::format("Unexpected error during execution: {}", e.what()));
                return result;
            }
        }
        
        // Debug: Print CPU state after execution
        DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "After execution: EAX={}, EBX={}, ECX={}, EDX={}", 
            cpu.get_register(static_cast<Register>(0)),
            cpu.get_register(static_cast<Register>(1)),
            cpu.get_register(static_cast<Register>(2)),
            cpu.get_register(static_cast<Register>(3)));
        
        // Evaluate assertions
        for (const auto& stmt : test_case.body) {
            if (stmt->type == Assembler::ASTNodeType::TEST_ASSERTION) {
                const auto* assertion = static_cast<const Assembler::TestAssertion*>(stmt.get());
                auto assertion_result = evaluate_assertion(*assertion, cpu, error_occurred);
                result.add_assertion(assertion_result);
            }
        }
        
    } catch (const std::exception& e) {
        result.set_error(fmt::format("Test execution failed: {}", e.what()));
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.execution_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    // Print progress indicator (checkmark or cross).
    // In filtered modes, only print if the test matches the show filter.
    bool should_show = false;
    if (!result.skipped) {
        switch (Config::test_show_mode) {
            case TestShowMode::ALL:
                should_show = true;
                break;
            case TestShowMode::FAILS:
                should_show = !result.passed;
                break;
            case TestShowMode::SUCCESS:
                should_show = result.passed;
                break;
        }
    }

    if (should_show) {
        if (defer_test_line_output) {
            std::cout << test_case.name << " ";
        }

        if (result.passed) {
            std::cout << "\033[32m✓\033[0m" << std::endl;  // Green checkmark for passed
        } else {
            std::cout << "\033[31m✗\033[0m" << std::endl;  // Red X for failed
        }
        std::cout.flush();
    }
    
    return result;
}

AssertionResult TestExecutor::evaluate_assertion(const Assembler::TestAssertion& assertion, 
                                                const CPU& cpu,
                                                bool expect_error_occurred) {
    switch (assertion.assertion_type) {
        case Assembler::TestAssertionType::ASSERT_REG: {
            if (assertion.arguments.size() != 2) {
                return AssertionResult(false, "assert_reg", "2 arguments", 
                                     fmt::format("{} arguments", assertion.arguments.size()),
                                     assertion.line, assertion.column);
            }
            
            int reg_num = get_register_number(*assertion.arguments[0]);
            int64_t expected = get_expected_value(*assertion.arguments[1]);
            int64_t actual = cpu.get_register(static_cast<Register>(reg_num));
            
            // For legacy registers (EAX-EBP), mask to 8-bit if expected value is 8-bit
            if (reg_num < 8 && expected <= 255 && actual > 255) {
                actual = actual & 0xFF;
            }
            
            Logging::DebugHandler::instance().report(Logging::DebugCategory::TEST_EXECUTION, 
                fmt::format("Reading R{}: expected={}, actual={}", reg_num, expected, actual), 
                Logging::DebugLevel::DETAIL);
            
            bool passed = (actual == expected);
            return AssertionResult(passed, "assert_reg", 
                                 fmt::format("R{} = {}", reg_num, expected),
                                 fmt::format("R{} = {}", reg_num, actual),
                                 assertion.line, assertion.column);
        }
        
        case Assembler::TestAssertionType::ASSERT_MEM: {
            if (assertion.arguments.size() != 2) {
                return AssertionResult(false, "assert_mem", "2 arguments",
                                     fmt::format("{} arguments", assertion.arguments.size()),
                                     assertion.line, assertion.column);
            }
            
            uint32_t address = get_memory_address(*assertion.arguments[0]);
            int64_t expected = get_expected_value(*assertion.arguments[1]);
            
            // Read single byte from memory
            auto& memory = const_cast<CPU&>(cpu).get_memory();
            if (address >= memory.size()) {
                return AssertionResult(false, "assert_mem", 
                                     fmt::format("address {} within bounds", address),
                                     "address out of bounds",
                                     assertion.line, assertion.column);
            }
            
            // Read as single byte (legacy register behavior)
            int64_t actual = static_cast<int64_t>(memory[address]);
            
            bool passed = (actual == expected);
            return AssertionResult(passed, "assert_mem",
                                 fmt::format("memory[{}] = {}", address, expected),
                                 fmt::format("memory[{}] = {}", address, actual),
                                 assertion.line, assertion.column);
        }
        
        case Assembler::TestAssertionType::ASSERT_OUTPUT: {
            if (assertion.arguments.size() != 1) {
                return AssertionResult(false, "assert_output", "1 argument",
                                     fmt::format("{} arguments", assertion.arguments.size()),
                                     assertion.line, assertion.column);
            }
            
            // Get expected string
            std::string expected;
            if (assertion.arguments[0]->type == Assembler::ASTNodeType::STRING_LITERAL) {
                const auto* str_expr = static_cast<const Assembler::StringLiteralExpression*>(assertion.arguments[0].get());
                expected = str_expr->value;
            } else {
                return AssertionResult(false, "assert_output", "string literal",
                                     "non-string argument",
                                     assertion.line, assertion.column);
            }
            
            std::string actual = output_capture.get_output();
            bool passed = (actual == expected);
            
            return AssertionResult(passed, "assert_output",
                                 fmt::format("\"{}\"", expected),
                                 fmt::format("\"{}\"", actual),
                                 assertion.line, assertion.column);
        }
        
        case Assembler::TestAssertionType::ASSERT_FPU: {
            if (assertion.arguments.size() != 2) {
                return AssertionResult(false, "assert_fpu", "2 arguments",
                                     fmt::format("{} arguments", assertion.arguments.size()),
                                     assertion.line, assertion.column);
            }
            
            try {
                // Get ST register number (0-7)
                int st_num = get_st_register_number(*assertion.arguments[0]);
                
                // Get expected floating point value
                double expected = 0.0;
                if (assertion.arguments[1]->type == Assembler::ASTNodeType::FLOAT) {
                    const auto* float_expr = static_cast<const Assembler::FloatExpression*>(assertion.arguments[1].get());
                    expected = float_expr->value;
                } else if (assertion.arguments[1]->type == Assembler::ASTNodeType::IMMEDIATE) {
                    const auto* int_expr = static_cast<const Assembler::ImmediateExpression*>(assertion.arguments[1].get());
                    expected = static_cast<double>(int_expr->value);
                } else {
                    return AssertionResult(false, "assert_fpu", "numeric literal",
                                         "non-numeric argument",
                                         assertion.line, assertion.column);
                }
                
                // Get actual value from FPU stack
                double actual = cpu.fpu_peek(static_cast<uint8_t>(st_num));
                
                // Use epsilon comparison for floating point
                const double epsilon = 1e-10;
                bool passed = std::abs(actual - expected) < epsilon;
                
                DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, 
                          "FPU assertion: ST({}) expected={}, actual={}, diff={}, passed={}", 
                          st_num, expected, actual, std::abs(actual - expected), passed);
                
                return AssertionResult(passed, "assert_fpu",
                                     fmt::format("ST({}) = {}", st_num, expected),
                                     fmt::format("ST({}) = {}", st_num, actual),
                                     assertion.line, assertion.column);
            } catch (const std::exception& e) {
                return AssertionResult(false, "assert_fpu", "valid FPU assertion",
                                     fmt::format("error: {}", e.what()),
                                     assertion.line, assertion.column);
            }
        }
        
        case Assembler::TestAssertionType::EXPECT_ERROR: {
            bool passed = expect_error_occurred;
            return AssertionResult(passed, "expect_error",
                                 "error to occur",
                                 expect_error_occurred ? "error occurred" : "no error",
                                 assertion.line, assertion.column);
        }
    }
    
    return AssertionResult(false, "unknown", "", "", assertion.line, assertion.column);
}

std::vector<uint8_t> TestExecutor::assemble_test_code(const std::vector<std::unique_ptr<Assembler::Statement>>& statements) {
    // Debug: Log test name and statement count
    Logging::DebugHandler::instance().report(Logging::DebugCategory::TEST_EXECUTION, 
        fmt::format("Assembling test '{}' with {} statements", current_test_name, statements.size()), 
        Logging::DebugLevel::DETAIL);
    // Extract source lines for the test body
    std::stringstream source;
    std::vector<std::string> source_lines;
    
    // Split source into lines
    std::stringstream ss(current_source);
    std::string line;
    while (std::getline(ss, line)) {
        source_lines.push_back(line);
    }
    
    // First, include file-level preprocessing directives that appear before any .test block
    bool found_first_test = false;
    for (size_t i = 0; i < source_lines.size() && !found_first_test; ++i) {
        std::string code_line = source_lines[i];
        
        // Trim leading whitespace
        size_t start = code_line.find_first_not_of(" \t");
        if (start != std::string::npos) {
            code_line = code_line.substr(start);
            
            // Check if we've reached the first .test block
            if (code_line.find(".test") == 0) {
                found_first_test = true;
                break;
            }
            
            // Include file-level preprocessing directives
            if (code_line.find(".include") == 0 || 
                code_line.find(".define") == 0 ||
                code_line.find(".undef") == 0 ||
                code_line.find(".ifdef") == 0 ||
                code_line.find(".ifndef") == 0 ||
                code_line.find(".elif") == 0 ||
                code_line.find(".else") == 0 ||
                code_line.find(".endif") == 0 ||
                code_line.find(".memory") == 0) {
                source << source_lines[i] << "\n"; // Use original line with whitespace
            }
        }
    }
    
    // Then, include all preprocessing directives from the current test scope
    for (size_t i = 0; i < source_lines.size(); ++i) {
        std::string code_line = source_lines[i];
        
        // Trim leading whitespace
        size_t start = code_line.find_first_not_of(" \t");
        if (start != std::string::npos) {
            code_line = code_line.substr(start);
            
            // Include preprocessing directives from test context
            if (code_line.find(".include") == 0 || 
                code_line.find(".define") == 0 ||
                code_line.find(".undef") == 0 ||
                code_line.find(".ifdef") == 0 ||
                code_line.find(".ifndef") == 0 ||
                code_line.find(".elif") == 0 ||
                code_line.find(".else") == 0 ||
                code_line.find(".endif") == 0 ||
                code_line.find(".memory") == 0) {
                source << code_line << "\n";
            }
        }
    }
    
    // Choose extraction strategy based on whether we have meaningful statements
    // If we only have HALT and test assertions, use brace tracking for function macros
    
    // Count non-trivial statements (not HALT, not test assertions)
    size_t meaningful_statements = 0;
    for (const auto& stmt : statements) {
        if (stmt->type == Assembler::ASTNodeType::INSTRUCTION) {
            // Check if it's not just a HALT instruction
            // We need to look at the actual instruction to determine this
            // For now, count all instructions except we'll check content later
            meaningful_statements++;
        }
        // Don't count test assertions as meaningful statements
    }
    
    // If we only have HALT instruction (and maybe test assertions), use brace tracking
    bool use_brace_tracking = (meaningful_statements <= 1);

    // Check if the test uses braces in the source (new style .test or #test)
    // This ensures we correctly extract the body even if it contains directives like section/global
    std::string test_decl_dot = ".test \"" + current_test_name + "\"";
    std::string test_decl_hash = "#test \"" + current_test_name + "\"";
    for (const auto& line : source_lines) {
        if ((line.find(test_decl_dot) != std::string::npos || line.find(test_decl_hash) != std::string::npos) 
            && line.find("{") != std::string::npos) {
            use_brace_tracking = true;
            break;
        }
    }
    
    // Also use brace tracking if the test contains preprocessor directives that the parser doesn't understand
    // Check if source contains .ifdef, .ifndef, .else, .endif within this test's context
    for (const auto& line : source_lines) {
        std::string trimmed = line;
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != std::string::npos) {
            trimmed = trimmed.substr(start);
            if (trimmed.find(".ifdef") == 0 || trimmed.find(".ifndef") == 0 || 
                trimmed.find(".else") == 0 || trimmed.find(".endif") == 0) {
                use_brace_tracking = true;
                break;
            }
        }
    }
    
    // Debug: Log strategy decision
    DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Test '{}': meaningful_statements={}, use_brace_tracking={}, source_lines={}", 
                                              current_test_name, meaningful_statements, use_brace_tracking, source_lines.size());
    
    if (!use_brace_tracking) {
        // Strategy 1: Use statement line ranges for tests with recognized statements
        size_t min_line = SIZE_MAX;
        size_t max_line = 0;
        for (const auto& stmt : statements) {
            if (stmt->type != Assembler::ASTNodeType::TEST_ASSERTION) {
                min_line = std::min(min_line, stmt->line);
                max_line = std::max(max_line, stmt->line);
            }
        }
        
        // Extract only lines within the current test case range
        for (size_t line_num = min_line; line_num <= max_line && line_num <= source_lines.size(); ++line_num) {
            if (line_num > 0) {
                std::string code_line = source_lines[line_num - 1];
                
                // Trim leading whitespace
                size_t start = code_line.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    code_line = code_line.substr(start);
                    
                    // Skip test metadata directives but include actual assembly code
                    // Skip test metadata directives
                    if (code_line.find(".description") == 0 ||
                        code_line.find(".author") == 0 ||
                        code_line.find(".category") == 0 ||
                        code_line.find(".tag") == 0 ||
                        code_line.find(".benchmark") == 0 ||
                        code_line.find(".warmup") == 0 ||
                        code_line.find(".iterations") == 0 ||
                        code_line.find(".measure") == 0 ||
                        code_line.find(".maxsteps") == 0 ||
                        code_line.find(".assert_") == 0 ||
                        code_line.find(".expect_") == 0 ||
                        code_line.find(".entry_point") == 0 ||
                        code_line.find("#") == 0 ||
                        code_line.find("{") == 0 ||
                        code_line.find("}") == 0 ||
                        code_line.empty() ||
                        code_line[0] == ';') {
                        continue; // Skip metadata and comments
                    }
                    
                    source << code_line << "\n";
                }
            }
        }
    } else {
        // Strategy 2: Use brace tracking for tests with only function macro calls
        // (no recognized statements)
        
        // Find the current test case in the source (support both .test and #test syntax)
        std::string test_marker_dot = ".test \"" + current_test_name + "\"";
        std::string test_marker_hash = "#test \"" + current_test_name + "\"";
        int test_start = -1;
        for (size_t i = 0; i < source_lines.size(); i++) {
            if (source_lines[i].find(test_marker_dot) != std::string::npos ||
                source_lines[i].find(test_marker_hash) != std::string::npos) {
                DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Found test marker at line {}", i);
                test_start = i;
                break;
            }
        }
        
        if (test_start == -1) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::TEST_EXECUTION, 
                "Could not find test: " + current_test_name, 
                Logging::DebugLevel::CRITICAL);
            return {};
        }
        
        // Find opening brace
        int brace_line = -1;
        for (size_t i = test_start; i < source_lines.size(); i++) {
            if (source_lines[i].find("{") != std::string::npos) {
                DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Found opening brace at line {}", i);
                brace_line = i;
                break;
            }
        }
        
        if (brace_line == -1) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::TEST_EXECUTION, 
                "Could not find opening brace for test: " + current_test_name, 
                Logging::DebugLevel::CRITICAL);
            return {};
        }
        
        // Track brace depth and extract content
        int brace_depth = 0;
        for (size_t i = brace_line; i < source_lines.size(); i++) {
            const std::string& line = source_lines[i];
            
            // Count braces in this line
            for (char c : line) {
                if (c == '{') {
                    brace_depth++;
                    DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Brace open at line {}, depth={}", i, brace_depth);
                }
                else if (c == '}') {
                    brace_depth--;
                    DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Brace close at line {}, depth={}", i, brace_depth);
                }
            }
            
            // Include content inside braces but skip metadata
            if (brace_depth > 0) {
                std::string code_line = line;
                size_t start = code_line.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    code_line = code_line.substr(start);
                    
                    if (code_line.find(".description") == std::string::npos &&
                        code_line.find(".author") == std::string::npos &&
                        code_line.find(".category") == std::string::npos &&
                        code_line.find(".tag") == std::string::npos &&
                        code_line.find(".benchmark") == std::string::npos &&
                        code_line.find(".warmup") == std::string::npos &&
                        code_line.find(".iterations") == std::string::npos &&
                        code_line.find(".measure") == std::string::npos &&
                        code_line.find(".maxsteps") == std::string::npos &&
                        code_line.find(".entry_point") == std::string::npos &&
                        code_line.find(".maxcalldepth") == std::string::npos &&
                        code_line.find(".timeout") == std::string::npos &&
                        code_line.find(".skip") == std::string::npos &&
                        code_line.find(".assert_") == std::string::npos &&
                        code_line.find(".expect_") == std::string::npos &&
                        code_line.find("{") == std::string::npos &&
                        code_line.find("#") != 0 &&
                        !code_line.empty() &&
                        code_line[0] != ';') {
                        DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Adding line to source: {}", code_line);
                        source << code_line << "\n";
                    } else {
                        DEBUG_INFO(Logging::DebugCategory::TEST_EXECUTION, "Skipping line: {}", code_line);
                    }
                }
            }
            
            // Stop when we've closed all braces
            if (brace_depth == 0 && i > static_cast<size_t>(brace_line)) {
                break;
            }
        }
    }
    
    source << "HALT\n"; // Ensure test terminates
    
    // Assemble the extracted source
    std::string asm_code = source.str();
    
    // Debug output
    Logging::DebugHandler::instance().report(Logging::DebugCategory::TEST_EXECUTION, 
        "=== Generated Assembly ===\n" + asm_code + "=========================\n", 
        Logging::DebugLevel::DETAIL);
    
    assembler.clear_errors();
    
    // Enable quiet mode to suppress parse error logging during test assembly
    Logging::ErrorHandler::instance().set_quiet_mode(true);
    
    // Get base path from the current test file for include resolution
    std::string base_path = std::filesystem::path(current_file_path).parent_path().string();
    auto bytecode = assembler.assemble_string(asm_code, base_path);
    
    // Disable quiet mode after assembly
    Logging::ErrorHandler::instance().set_quiet_mode(false);

    if (!bytecode.empty() && (Logging::HexDumper::is_enabled() || Config::verbose)) {
         std::string dump = Logging::HexDumper::format_bytecode(bytecode);
         Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_HEXDUMP, 
             "=== Bytecode Hexdump ===\n" + dump + "\n", 
             Logging::DebugLevel::DETAIL);
    }
    
    if (assembler.has_errors()) {
        Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_PARSING, "Assembly errors:", Logging::DebugLevel::CRITICAL);
        for (const auto& err : assembler.get_errors()) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::ASM_PARSING, "  " + err, Logging::DebugLevel::CRITICAL);
        }
    }
    
    return bytecode;
}

int64_t TestExecutor::get_expected_value(const Assembler::Expression& expr) {
    if (expr.type == Assembler::ASTNodeType::IMMEDIATE) {
        const auto* imm = static_cast<const Assembler::ImmediateExpression*>(&expr);
        return imm->value;
    }
    return 0;
}

int TestExecutor::get_register_number(const Assembler::Expression& expr) {
    if (expr.type == Assembler::ASTNodeType::REGISTER) {
        const auto* reg = static_cast<const Assembler::RegisterExpression*>(&expr);
        
        // Handle 32-bit aliases
        if (reg->name == "EAX") return 0;
        if (reg->name == "ECX") return 1;
        if (reg->name == "EDX") return 2;
        if (reg->name == "EBX") return 3;
        if (reg->name == "ESP") return 4;
        if (reg->name == "EBP") return 5;
        if (reg->name == "ESI") return 6;
        if (reg->name == "EDI") return 7;

        // Handle 64-bit names
        if (reg->name == "RAX") return 0;
        if (reg->name == "RCX") return 1;
        if (reg->name == "RDX") return 2;
        if (reg->name == "RBX") return 3;
        if (reg->name == "RSP") return 4;
        if (reg->name == "RBP") return 5;
        if (reg->name == "RSI") return 6;
        if (reg->name == "RDI") return 7;

        // Handle special registers (from cpu_registers.hpp)
        if (reg->name == "RIP") return 47;
        if (reg->name == "RFLAGS" || reg->name == "FLAGS") return 48;
        if (reg->name == "MSW") return 49;

        // XMM / YMM registers
        // Engine register enum: XMM0 starts at 50 and uses pairs (low/high 64-bit parts)
        // XMMn -> 50 + 2*n, XMMn_HIGH -> 50 + 2*n + 1, YMMn aliases the low part.
        auto is_all_digits = [](const std::string& s) -> bool {
            if (s.empty()) return false;
            for (unsigned char c : s) {
                if (!std::isdigit(c)) return false;
            }
            return true;
        };

        if (reg->name.rfind("XMM", 0) == 0) {
            std::string suffix = reg->name.substr(3);
            bool high = false;
            const std::string high_tag = "_HIGH";
            if (suffix.size() > high_tag.size() &&
                suffix.compare(suffix.size() - high_tag.size(), high_tag.size(), high_tag) == 0) {
                high = true;
                suffix = suffix.substr(0, suffix.size() - high_tag.size());
            }
            if (is_all_digits(suffix)) {
                const int idx = std::stoi(suffix);
                if (idx >= 0 && idx < 16) {
                    return 50 + idx * 2 + (high ? 1 : 0);
                }
            }
        }

        if (reg->name.rfind("YMM", 0) == 0) {
            const std::string suffix = reg->name.substr(3);
            if (is_all_digits(suffix)) {
                const int idx = std::stoi(suffix);
                if (idx >= 0 && idx < 16) {
                    return 50 + idx * 2;
                }
            }
        }

        // Extract register number from name (e.g., "R0" -> 0)
        if (reg->name.size() > 1 && reg->name[0] == 'R') {
            try {
                std::string num_part = reg->name.substr(1);
                // Handle R8D-R15D style
                if (num_part.back() == 'D' && num_part.size() > 1) {
                    num_part.pop_back();
                }
                return std::stoi(num_part);
            } catch (...) {
                // Ignore parsing errors
            }
        }
    }
    return 0;
}

int TestExecutor::get_st_register_number(const Assembler::Expression& expr) {
    if (expr.type == Assembler::ASTNodeType::REGISTER) {
        const auto* reg = static_cast<const Assembler::RegisterExpression*>(&expr);
        // Extract ST register number from name (e.g., "ST0" -> 0, "ST7" -> 7)
        if (reg->name.size() >= 3 && reg->name.substr(0, 2) == "ST") {
            int st_num = std::stoi(reg->name.substr(2));
            if (st_num >= 0 && st_num <= 7) {
                return st_num;
            }
        }
    }
    throw std::runtime_error(fmt::format("Invalid ST register specification"));
}

uint32_t TestExecutor::get_memory_address(const Assembler::Expression& expr) {
    if (expr.type == Assembler::ASTNodeType::IMMEDIATE) {
        const auto* imm = static_cast<const Assembler::ImmediateExpression*>(&expr);
        return static_cast<uint32_t>(imm->value);
    }
    return 0;
}

void TestExecutor::print_results(const std::vector<TestResult>& results) {
    size_t total_tests = results.size();
    size_t passed_tests = 0;
    size_t failed_tests = 0;
    size_t skipped_tests = 0;
    size_t total_assertions = 0;
    size_t passed_assertions = 0;
    double total_execution_time = 0.0;
    
    // Filtered counts (only tests that should be shown)
    size_t shown_tests = 0;
    size_t shown_passed = 0;
    size_t shown_failed = 0;
    size_t shown_assertions = 0;
    size_t shown_passed_assertions = 0;
    double shown_execution_time = 0.0;
    
    // Count by category
    std::map<std::string, size_t> category_totals;
    std::map<std::string, size_t> category_passed;
    std::map<std::string, size_t> shown_category_totals;
    std::map<std::string, size_t> shown_category_passed;
    
    for (const auto& result : results) {
        // Count skipped tests separately
        if (result.skipped) {
            skipped_tests++;
            total_execution_time += result.execution_time_ms;
            continue;
        }
        
        // Count all tests for overall stats
        if (result.passed) {
            passed_tests++;
        } else {
            failed_tests++;
        }
        total_assertions += result.assertions.size();
        passed_assertions += result.get_passed_count();
        total_execution_time += result.execution_time_ms;
        
        // Track by category (all tests)
        if (!result.category.empty()) {
            category_totals[result.category]++;
            if (result.passed) {
                category_passed[result.category]++;
            }
        }
        
        // Filter based on show mode
        bool should_show = false;
        switch (Config::test_show_mode) {
            case TestShowMode::ALL:
                should_show = true;
                break;
            case TestShowMode::FAILS:
                should_show = !result.passed;
                break;
            case TestShowMode::SUCCESS:
                should_show = result.passed;
                break;
        }
        
        if (!should_show) {
            continue;  // Skip this test in output
        }
        
        // Count shown tests
        shown_tests++;
        if (result.passed) {
            shown_passed++;
        } else {
            shown_failed++;
        }
        shown_assertions += result.assertions.size();
        shown_passed_assertions += result.get_passed_count();
        shown_execution_time += result.execution_time_ms;
        
        // Track shown by category
        if (!result.category.empty()) {
            shown_category_totals[result.category]++;
            if (result.passed) {
                shown_category_passed[result.category]++;
            }
        }
        
        // For non-quiet mode, show detailed test information
        if (!Config::quiet_assembly_test) {
            // Print test name
            if (!result.test_name.empty()) {
                fmt::print("\033[1mTest: {}\033[0m\n", result.test_name);
            }
            
            // Print test metadata only when --show-metadata is enabled
            if (Config::show_test_metadata) {
                if (!result.description.empty()) {
                    fmt::print("  Description: {}\n", result.description);
                }
                if (!result.author.empty()) {
                    fmt::print("  Author: {}\n", result.author);
                }
                if (!result.category.empty()) {
                    fmt::print("  Category: {}\n", result.category);
                }
                if (!result.tags.empty()) {
                    std::string tags_str = "  Tags: ";
                    for (size_t i = 0; i < result.tags.size(); ++i) {
                        if (i > 0) tags_str += ", ";
                        tags_str += result.tags[i];
                    }
                    fmt::print("{}\n", tags_str);
                }
            }
            
            // Print test result
            if (result.skipped) {
                fmt::print("\033[33m⊘ Test skipped\033[0m\n");
            } else if (result.passed) {
                fmt::print("\033[32m✓ Test completed successfully\033[0m\n");
            } else {
                fmt::print(stderr, "\033[31m✗ Test failed\033[0m\n");
                
                if (!result.error_message.empty()) {
                    fmt::print(stderr, "  Error: {}\n", result.error_message);
                }
                
                // Print failed assertions with detailed information
                for (const auto& assertion : result.assertions) {
                    if (!assertion.passed) {
                        fmt::print(stderr, "  \033[31m✗\033[0m {} at line {}: {}\n",
                            assertion.assertion_type,
                            assertion.line,
                            assertion.message);
                    } else {
                        fmt::print("  \033[32m✓\033[0m {} at line {}: {}\n",
                            assertion.assertion_type,
                            assertion.line,
                            assertion.message);
                    }
                }
            }
            fmt::print("\n"); // Add spacing between tests
        } else {
            // For quiet mode, print test name and description, plus error details for failures
            if (result.passed) {
                fmt::print("{}\n", result.test_name);
            } else {
                fmt::print(stderr, "{}\n", result.test_name);
            }
            if (!result.description.empty()) {
                if (result.passed) {
                    fmt::print("  {}\n", result.description);
                } else {
                    fmt::print(stderr, "  {}\n", result.description);
                }
            }
            
            // In quiet mode, still show error details for failed tests
            if (!result.passed) {
                if (!result.error_message.empty()) {
                    fmt::print(stderr, "  Error: {}\n", result.error_message);
                }
                
                // Print failed assertions with detailed information
                for (const auto& assertion : result.assertions) {
                    if (!assertion.passed) {
                        fmt::print(stderr, "  \033[31m✗\033[0m {} at line {}: {}\n",
                            assertion.assertion_type,
                            assertion.line,
                            assertion.message);
                    }
                }
            }
        }
    }
    
    // Print enhanced summary (skip in quiet mode)
    if (!Config::quiet_assembly_test) {
        fmt::print("\033[1m=== Test Summary ===\033[0m\n");
        
        // Show filtered statistics if filtering is active
        if (Config::test_show_mode != TestShowMode::ALL) {
            std::string mode_name;
            switch (Config::test_show_mode) {
                case TestShowMode::FAILS: mode_name = "Failed"; break;
                case TestShowMode::SUCCESS: mode_name = "Successful"; break;
                default: mode_name = "Shown"; break;
            }
            
            fmt::print("{} Tests: {} (\033[32mpassed: {}\033[0m, \033[31mfailed: {}\033[0m)\n",
                mode_name, shown_tests, shown_passed, shown_failed);
            
            fmt::print("{} Assertions: {} (\033[32mpassed: {}\033[0m, \033[31mfailed: {}\033[0m)\n",
                mode_name, shown_assertions, shown_passed_assertions, 
                shown_assertions - shown_passed_assertions);
        } else {
            // Show all statistics
            fmt::print("Total Tests: {} (\033[32mpassed: {}\033[0m, \033[31mfailed: {}\033[0m, \033[36mskipped: {}\033[0m)\n",
                total_tests, passed_tests, failed_tests, skipped_tests);
            
            fmt::print("Total Assertions: {} (\033[32mpassed: {}\033[0m, \033[31mfailed: {}\033[0m)\n",
                total_assertions, passed_assertions, total_assertions - passed_assertions);
        }
        
        fmt::print("Total Execution Time: {:.2f}ms\n", total_execution_time);
        
        // Category breakdown - show filtered categories if filtering is active
        auto& category_display = (Config::test_show_mode != TestShowMode::ALL) ? shown_category_totals : category_totals;
        auto& category_display_passed = (Config::test_show_mode != TestShowMode::ALL) ? shown_category_passed : category_passed;
        
        if (!category_display.empty()) {
            fmt::print("\033[1mBy Category:\033[0m\n");
            for (const auto& [category, total] : category_display) {
                size_t cat_passed = category_display_passed[category];
                size_t cat_failed = total - cat_passed;
                const char* color = (cat_failed == 0) ? "\033[32m" : "\033[33m";
                fmt::print("  {}{}: {}/{} passed\033[0m\n", color, category, cat_passed, total);
            }
        }
        
        // Overall result
        if (Config::test_show_mode != TestShowMode::ALL) {
            // When filtering, show result based on what was shown
            if (shown_tests == 0) {
                std::string mode_name;
                switch (Config::test_show_mode) {
                    case TestShowMode::FAILS: mode_name = "failed"; break;
                    case TestShowMode::SUCCESS: mode_name = "successful"; break;
                    default: mode_name = "matching"; break;
                }
                fmt::print("\033[1;36mNo {} tests found.\033[0m\n", mode_name);
            } else if (shown_passed == shown_tests) {
                fmt::print("\033[1;32m✓ All {} shown tests passed!\033[0m\n", shown_tests);
            } else {
                fmt::print(stderr, "\033[1;31m✗ {} test(s) failed\033[0m\n", shown_failed);
            }
        } else {
            // Show overall result for all tests
            if (passed_tests == total_tests) {
                fmt::print("\033[1;32m✓ All tests passed!\033[0m\n");
            } else {
                fmt::print(stderr, "\033[1;31m✗ {} test(s) failed\033[0m\n", failed_tests);
            }
        }
    } else {
        // In quiet mode, show a simplified summary
        if (Config::test_show_mode != TestShowMode::ALL) {
            // Show filtered statistics
            std::string mode_name;
            switch (Config::test_show_mode) {
                case TestShowMode::FAILS: mode_name = "Failed"; break;
                case TestShowMode::SUCCESS: mode_name = "Successful"; break;
                default: mode_name = "Shown"; break;
            }
            
            fmt::print("{} Tests: {} (passed: {}, failed: {})\n",
                mode_name, shown_tests, shown_passed, shown_failed);
        } else {
            // Show all statistics
            fmt::print("Total Tests: {} (passed: {}, failed: {}, skipped: {})\n",
                total_tests, passed_tests, failed_tests, skipped_tests);
        }
    }
}

} // namespace Testing
