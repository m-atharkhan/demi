#include "assembly_test_executor.hpp"
#include "../assembler/lexer.hpp"
#include "../assembler/parser.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>

namespace Testing {

std::vector<TestResult> TestExecutor::execute_tests_from_file(const std::string& filename) {
    std::vector<TestResult> results;
    
    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::instance().error() << fmt::format("Failed to open test file: {}", filename) << std::endl;
        return results;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();
    
    // Store source for extraction
    current_source = source;
    
    // Lex and parse
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.has_errors()) {
        Logger::instance().error() << "Lexer errors in test file:" << std::endl;
        for (const auto& error : lexer.get_errors()) {
            Logger::instance().error() << "  " << error << std::endl;
        }
        return results;
    }
    
    Parser parser(tokens);
    auto program = parser.parse();
    
    if (parser.has_errors()) {
        Logger::instance().error() << "Parser errors in test file:" << std::endl;
        for (const auto& error : parser.get_errors()) {
            Logger::instance().error() << "  " << error << std::endl;
        }
        return results;
    }
    
    // Execute each test
    if (!Config::quiet_assembly_test) {
        Logger::instance().info() << fmt::format("\033[1;36m=== Running tests from {} ===\033[0m", filename) << std::endl;
    }
    
    for (const auto& test : program->test_cases) {
        results.push_back(execute_test(*test, program->statements));
    }
    
    return results;
}

TestResult TestExecutor::execute_test(const Assembler::TestCase& test_case, 
                                      const std::vector<std::unique_ptr<Assembler::Statement>>& program_statements) {
    TestResult result(test_case.name);
    result.set_metadata(test_case.description, test_case.author, test_case.category, test_case.tags);
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Create CPU instance for this test with 256 bytes for test compatibility
        CPU cpu(256);  // Use small memory size so out-of-bounds tests work correctly
        output_capture.clear();
        
        // Note: Test metadata will be printed after test execution based on filter mode
        
        // Assemble test code (main program + test body)
        std::vector<uint8_t> bytecode = assemble_test_code(test_case.body);
        
        Logger::instance().debug() << fmt::format("Bytecode size: {}", bytecode.size()) << std::endl;
        
        if (assembler.has_errors()) {
            std::stringstream error_msg;
            error_msg << "Assembly errors:\n";
            for (const auto& error : assembler.get_errors()) {
                error_msg << "  " << error << "\n";
            }
            result.set_error(error_msg.str());
            return result;
        }
        
        // Execute the test code
        bool expect_error = false;
        bool error_occurred = false;
        
        try {
            Logger::instance().debug() << "About to call cpu.run()..." << std::endl;
            cpu.run(bytecode);
            Logger::instance().debug() << "cpu.run() completed" << std::endl;
            
            // IMPORTANT: Sync legacy registers to new register system
            // The opcodes write to legacy_registers but get_register() reads from registers array
            cpu.sync_from_legacy_registers();
        } catch (const std::exception& e) {
            error_occurred = true;
            // Check if we expected an error
            for (const auto& stmt : test_case.body) {
                if (stmt->type == Assembler::ASTNodeType::TEST_ASSERTION) {
                    const auto* assertion = static_cast<const Assembler::TestAssertion*>(stmt.get());
                    if (assertion->assertion_type == Assembler::TestAssertionType::EXPECT_ERROR) {
                        expect_error = true;
                        break;
                    }
                }
            }
            
            if (!expect_error) {
                result.set_error(fmt::format("Unexpected error during execution: {}", e.what()));
                return result;
            }
        }
        
        // Debug: Print CPU state after execution
        Logger::instance().debug() << fmt::format("After execution: R0={}, R1={}, R2={}, R3={}", 
            cpu.get_register(static_cast<Register>(0)),
            cpu.get_register(static_cast<Register>(1)),
            cpu.get_register(static_cast<Register>(2)),
            cpu.get_register(static_cast<Register>(3))) << std::endl;
        
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
    
    // Print test completion status immediately after execution
    // Apply filtering based on test_show_mode
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
    
    if (!Config::quiet_assembly_test && should_show) {
        // Print test metadata
        if (!result.test_name.empty()) {
            Logger::instance().info() << fmt::format("Test: {}", result.test_name) << std::endl;
        }
        if (!result.description.empty()) {
            Logger::instance().info() << fmt::format("  Description: {}", result.description) << std::endl;
        }
        if (!result.author.empty()) {
            Logger::instance().info() << fmt::format("  Author: {}", result.author) << std::endl;
        }
        if (!result.category.empty()) {
            Logger::instance().info() << fmt::format("  Category: {}", result.category) << std::endl;
        }
        if (!result.tags.empty()) {
            std::string tags_str = "  Tags: ";
            for (size_t i = 0; i < result.tags.size(); ++i) {
                if (i > 0) tags_str += ", ";
                tags_str += result.tags[i];
            }
            Logger::instance().info() << tags_str << std::endl;
        }
        
        // Print test result
        if (result.passed) {
            Logger::instance().info() << "\033[32m✓ Test completed successfully\033[0m" << std::endl;
        } else {
            Logger::instance().error() << "\033[31m✗ Test failed\033[0m" << std::endl;
            
            if (!result.error_message.empty()) {
                Logger::instance().error() << fmt::format("  Error: {}", result.error_message) << std::endl;
            }
            
            // Print failed assertions
            for (const auto& assertion : result.assertions) {
                if (!assertion.passed) {
                    Logger::instance().error() << fmt::format(
                        "  {} at line {}: {}",
                        assertion.assertion_type,
                        assertion.line,
                        assertion.message
                    ) << std::endl;
                }
            }
        }
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
            
            // For legacy registers (R0-R7), mask to 8-bit if expected value is 8-bit
            if (reg_num < 8 && expected <= 255 && actual > 255) {
                actual = actual & 0xFF;
            }
            
            Logger::instance().debug() << fmt::format("Reading R{}: expected={}, actual={}", reg_num, expected, actual) << std::endl;
            
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
    // Extract source lines for the test body
    std::stringstream source;
    std::vector<std::string> source_lines;
    
    // Split source into lines
    std::stringstream ss(current_source);
    std::string line;
    while (std::getline(ss, line)) {
        source_lines.push_back(line);
    }
    
    // Extract lines for each statement in the test body
    for (const auto& stmt : statements) {
        // Skip test assertions - they're not executable code
        if (stmt->type == Assembler::ASTNodeType::TEST_ASSERTION) {
            continue;
        }
        
        // Get line number (1-indexed)
        size_t line_num = stmt->line;
        if (line_num > 0 && line_num <= source_lines.size()) {
            std::string code_line = source_lines[line_num - 1];
            
            // Trim leading whitespace
            size_t start = code_line.find_first_not_of(" \t");
            if (start != std::string::npos) {
                code_line = code_line.substr(start);
                
                // Skip test directives
                if (code_line.find("#test") == 0 || 
                    code_line.find("#assert") == 0 || 
                    code_line.find("#expect") == 0 ||
                    code_line.find("{") == 0 ||
                    code_line.find("}") == 0 ||
                    code_line.empty() ||
                    code_line[0] == ';') {
                    continue;
                }
                
                source << code_line << "\n";
            }
        }
    }
    
    source << "HALT\n"; // Ensure test terminates
    
    // Assemble the extracted source
    std::string asm_code = source.str();
    
    // Debug output
    Logger::instance().debug() << "=== Generated Assembly ===\n" << asm_code << "=========================\n" << std::endl;
    
    assembler.clear_errors();
    auto bytecode = assembler.assemble_string(asm_code);
    
    if (assembler.has_errors()) {
        Logger::instance().error() << "Assembly errors:\n";
        for (const auto& err : assembler.get_errors()) {
            Logger::instance().error() << "  " << err << "\n";
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
        // Extract register number from name (e.g., "R0" -> 0)
        if (reg->name.size() > 1 && reg->name[0] == 'R') {
            return std::stoi(reg->name.substr(1));
        }
    }
    return 0;
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
    size_t total_assertions = 0;
    size_t passed_assertions = 0;
    double total_execution_time = 0.0;
    
    // Count by category
    std::map<std::string, size_t> category_totals;
    std::map<std::string, size_t> category_passed;
    
    for (const auto& result : results) {
        if (result.passed) {
            passed_tests++;
        } else {
            failed_tests++;
        }
        total_assertions += result.assertions.size();
        passed_assertions += result.get_passed_count();
        total_execution_time += result.execution_time_ms;
        
        // Track by category
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
        
        // For quiet mode, print simple test name and description
        if (Config::quiet_assembly_test) {
            if (result.passed) {
                Logger::instance().info() << result.test_name << std::endl;
            } else {
                Logger::instance().error() << result.test_name << std::endl;
            }
            if (!result.description.empty()) {
                if (result.passed) {
                    Logger::instance().info() << fmt::format("  {}", result.description) << std::endl;
                } else {
                    Logger::instance().error() << fmt::format("  {}", result.description) << std::endl;
                }
            }
        }
    }
    
    // Print enhanced summary (skip in quiet mode)
    if (!Config::quiet_assembly_test) {
        Logger::instance().info() << "\n\033[1m=== Test Summary ===\033[0m" << std::endl;
        
        // Overall statistics
        Logger::instance().info() << fmt::format(
            "Total Tests: {} ({}passed: {}{}, {}failed: {}{})",
            total_tests,
            "\033[32m", passed_tests, "\033[0m",
            "\033[31m", failed_tests, "\033[0m"
        ) << std::endl;
        
        Logger::instance().info() << fmt::format(
            "Total Assertions: {} ({}passed: {}{}, {}failed: {}{})",
            total_assertions,
            "\033[32m", passed_assertions, "\033[0m",
            "\033[31m", total_assertions - passed_assertions, "\033[0m"
        ) << std::endl;
        
        Logger::instance().info() << fmt::format(
            "Total Execution Time: {:.2f}ms",
            total_execution_time
        ) << std::endl;
        
        // Category breakdown if categories exist
        if (!category_totals.empty()) {
            Logger::instance().info() << "\n\033[1mBy Category:\033[0m" << std::endl;
            for (const auto& [category, total] : category_totals) {
                size_t cat_passed = category_passed[category];
                size_t cat_failed = total - cat_passed;
                const char* color = (cat_failed == 0) ? "\033[32m" : "\033[33m";
                Logger::instance().info() << fmt::format(
                    "  {}{}: {}/{} passed{}",
                    color, category, cat_passed, total, "\033[0m"
                ) << std::endl;
            }
        }
        
        // Overall result
        std::cout << std::endl;
        if (passed_tests == total_tests) {
            Logger::instance().info() << "\033[1;32m✓ All tests passed!\033[0m" << std::endl;
        } else {
            Logger::instance().error() << fmt::format(
                "\033[1;31m✗ {} test(s) failed\033[0m",
                failed_tests
            ) << std::endl;
        }
    }
}

} // namespace Testing
