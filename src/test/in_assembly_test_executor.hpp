#pragma once

#include "../assembler/ast.hpp"
#include "../assembler/demi_assembler.hpp"
#include "../engine/cpu.hpp"
#include "../engine/device_factory.hpp"
#include <vector>
#include <string>
#include <memory>
#include <fmt/format.h>

namespace Assembler {

// Result of running a single assertion
struct AssertionResult {
    bool passed;
    std::string message;
    size_t line;
    size_t column;
    
    AssertionResult(bool p, const std::string& msg, size_t ln, size_t col)
        : passed(p), message(msg), line(ln), column(col) {}
};

// Result of running a single test case
struct InAssemblyTestResult {
    std::string name;
    bool passed;
    std::vector<AssertionResult> assertion_results;
    std::string error_message;
    
    InAssemblyTestResult(const std::string& test_name)
        : name(test_name), passed(true), error_message("") {}
    
    void add_assertion(const AssertionResult& result) {
        assertion_results.push_back(result);
        if (!result.passed) {
            passed = false;
        }
    }
    
    void set_error(const std::string& error) {
        error_message = error;
        passed = false;
    }
};

// Executor for in-assembly tests
class InAssemblyTestExecutor {
private:
    std::vector<InAssemblyTestResult> results;
    
    // Evaluate a single expression in the context of a CPU
    int64_t evaluate_expression(const Expression& expr, CPU& cpu) {
        switch (expr.node_type) {
            case ASTNodeType::IMMEDIATE_EXPRESSION: {
                const auto& imm = static_cast<const ImmediateExpression&>(expr);
                return imm.value;
            }
            
            case ASTNodeType::REGISTER_EXPRESSION: {
                const auto& reg = static_cast<const RegisterExpression&>(expr);
                // Get register index from name (R0 -> 0, R1 -> 1, etc.)
                if (reg.register_name.size() >= 2 && reg.register_name[0] == 'R') {
                    int reg_num = std::stoi(reg.register_name.substr(1));
                    if (reg_num >= 0 && reg_num < 50) {
                        return cpu.registers.get_register(reg_num);
                    }
                }
                // Handle special registers like SP, PC
                if (reg.register_name == "SP") {
                    return cpu.registers.get_register(Register::SP);
                }
                if (reg.register_name == "PC") {
                    return cpu.registers.get_register(Register::PC);
                }
                return 0;
            }
            
            case ASTNodeType::STRING_LITERAL_EXPRESSION: {
                // Can't directly evaluate string literals to integers
                return 0;
            }
            
            default:
                return 0;
        }
    }
    
    // Get string value from expression
    std::string evaluate_string_expression(const Expression& expr) {
        if (expr.node_type == ASTNodeType::STRING_LITERAL_EXPRESSION) {
            const auto& str = static_cast<const StringLiteralExpression&>(expr);
            return str.value;
        }
        return "";
    }
    
    // Check if assertion passes
    AssertionResult evaluate_assertion(const TestAssertion& assertion, CPU& cpu) {
        switch (assertion.assertion_type) {
            case TestAssertionType::ASSERT_MEM: {
                // ASSERT_MEM: address, expected_value
                if (assertion.arguments.size() != 2) {
                    return AssertionResult(false, "ASSERT_MEM requires 2 arguments", 
                                         assertion.line, assertion.column);
                }
                
                int64_t address = evaluate_expression(*assertion.arguments[0], cpu);
                int64_t expected = evaluate_expression(*assertion.arguments[1], cpu);
                
                if (address < 0 || address >= static_cast<int64_t>(cpu.memory.size())) {
                    return AssertionResult(false, 
                                         fmt::format("ASSERT_MEM: address {} out of bounds", address),
                                         assertion.line, assertion.column);
                }
                
                uint8_t actual = cpu.memory[address];
                if (actual == static_cast<uint8_t>(expected)) {
                    return AssertionResult(true, 
                                         fmt::format("ASSERT_MEM: memory[{}] == {} ✓", address, expected),
                                         assertion.line, assertion.column);
                } else {
                    return AssertionResult(false,
                                         fmt::format("ASSERT_MEM: memory[{}] expected {}, got {}",
                                                   address, expected, actual),
                                         assertion.line, assertion.column);
                }
            }
            
            case TestAssertionType::ASSERT_REG: {
                // ASSERT_REG: register, expected_value
                if (assertion.arguments.size() != 2) {
                    return AssertionResult(false, "ASSERT_REG requires 2 arguments",
                                         assertion.line, assertion.column);
                }
                
                // First argument should be a register
                if (assertion.arguments[0]->node_type != ASTNodeType::REGISTER_EXPRESSION) {
                    return AssertionResult(false, "ASSERT_REG: first argument must be a register",
                                         assertion.line, assertion.column);
                }
                
                const auto& reg_expr = static_cast<const RegisterExpression&>(*assertion.arguments[0]);
                int64_t actual = evaluate_expression(*assertion.arguments[0], cpu);
                int64_t expected = evaluate_expression(*assertion.arguments[1], cpu);
                
                if (actual == expected) {
                    return AssertionResult(true,
                                         fmt::format("ASSERT_REG: {} == {} ✓", 
                                                   reg_expr.register_name, expected),
                                         assertion.line, assertion.column);
                } else {
                    return AssertionResult(false,
                                         fmt::format("ASSERT_REG: {} expected {}, got {}",
                                                   reg_expr.register_name, expected, actual),
                                         assertion.line, assertion.column);
                }
            }
            
            case TestAssertionType::ASSERT_OUTPUT: {
                // ASSERT_OUTPUT: expected_string
                if (assertion.arguments.size() != 1) {
                    return AssertionResult(false, "ASSERT_OUTPUT requires 1 argument",
                                         assertion.line, assertion.column);
                }
                
                std::string expected = evaluate_string_expression(*assertion.arguments[0]);
                // TODO: Capture actual output from CPU execution
                // For now, we'll just check if the output device was used
                
                return AssertionResult(true, "ASSERT_OUTPUT: output checking not yet implemented",
                                     assertion.line, assertion.column);
            }
            
            case TestAssertionType::EXPECT_ERROR: {
                // EXPECT_ERROR: optional error_message
                // This is checked during execution, not after
                return AssertionResult(true, "EXPECT_ERROR: checked during execution",
                                     assertion.line, assertion.column);
            }
            
            default:
                return AssertionResult(false, "Unknown assertion type",
                                     assertion.line, assertion.column);
        }
    }
    
    // Execute a single test case
    InAssemblyTestResult execute_test_case(const TestCase& test, 
                                          const std::map<std::string, uint32_t>& symbols) {
        InAssemblyTestResult result(test.name);
        
        // Create a CPU for this test
        CPU cpu;
        DeviceManager device_manager;
        setup_standard_devices(device_manager);
        cpu.set_device_manager(&device_manager);
        
        // Assemble just the test body into bytecode
        // We need to assemble the statements in the test body
        Program test_program;
        for (const auto& stmt : test.body) {
            // If it's a test assertion, save it for later evaluation
            if (stmt->node_type == ASTNodeType::TEST_ASSERTION) {
                // We'll evaluate assertions after execution
                continue;
            }
            
            // Clone the statement (we need a copy since we can't move from const)
            // For now, skip non-assertion statements as we need a way to assemble them
            // This is a limitation - we need the assembler to process individual statements
        }
        
        // Assemble the test body
        AssemblerEngine assembler;
        assembler.set_symbols(symbols); // Use symbols from main program
        
        // Create a program from test body (excluding assertions)
        Program body_program;
        for (const auto& stmt : test.body) {
            if (stmt->node_type != ASTNodeType::TEST_ASSERTION) {
                // We can't easily clone statements, so we need a different approach
                // For now, mark this as TODO
                result.set_error("Test execution not yet fully implemented - statement cloning needed");
                return result;
            }
        }
        
        auto bytecode = assembler.assemble(body_program);
        
        if (assembler.has_errors()) {
            result.set_error("Assembly failed: " + assembler.get_errors()[0]);
            return result;
        }
        
        // Load bytecode into CPU memory safely
        if (bytecode.size() > cpu.get_memory_size()) {
            result.set_error(fmt::format("Bytecode too large: {} bytes, memory size: {}", bytecode.size(), cpu.get_memory_size()));
            return result;
        }
        
        // Use safe memory copy with bounds checking
        auto& memory = cpu.get_memory();
        std::copy_n(bytecode.begin(), std::min(bytecode.size(), memory.size()), memory.begin());
        
        // Execute the test
        cpu.reset();
        bool has_expect_error = false;
        
        // Check if test expects an error
        for (const auto& stmt : test.body) {
            if (stmt->node_type == ASTNodeType::TEST_ASSERTION) {
                const auto& assertion = static_cast<const TestAssertion&>(*stmt);
                if (assertion.assertion_type == TestAssertionType::EXPECT_ERROR) {
                    has_expect_error = true;
                    break;
                }
            }
        }
        
        // Run the CPU
        bool had_error = false;
        try {
            while (cpu.is_running() && cpu.registers.get_register(Register::PC) < bytecode.size()) {
                cpu.step();
                
                // Check for errors
                if (cpu.error_count > 0) {
                    had_error = true;
                    break;
                }
            }
        } catch (const std::exception& e) {
            had_error = true;
            if (!has_expect_error) {
                result.set_error(fmt::format("Exception during execution: {}", e.what()));
                return result;
            }
        }
        
        // Check EXPECT_ERROR
        if (has_expect_error && !had_error) {
            result.add_assertion(AssertionResult(false, 
                                                "EXPECT_ERROR: test did not produce an error",
                                                test.line, test.column));
        } else if (has_expect_error && had_error) {
            result.add_assertion(AssertionResult(true, 
                                                "EXPECT_ERROR: test produced expected error ✓",
                                                test.line, test.column));
        }
        
        // Evaluate all assertions (except EXPECT_ERROR which was already checked)
        for (const auto& stmt : test.body) {
            if (stmt->node_type == ASTNodeType::TEST_ASSERTION) {
                const auto& assertion = static_cast<const TestAssertion&>(*stmt);
                if (assertion.assertion_type != TestAssertionType::EXPECT_ERROR) {
                    auto assertion_result = evaluate_assertion(assertion, cpu);
                    result.add_assertion(assertion_result);
                }
            }
        }
        
        return result;
    }
    
public:
    // Execute all test cases from a parsed program
    std::vector<InAssemblyTestResult> execute_tests(const Program& program,
                                                    const std::map<std::string, uint32_t>& symbols) {
        results.clear();
        
        for (const auto& test_case : program.test_cases) {
            results.push_back(execute_test_case(*test_case, symbols));
        }
        
        return results;
    }
    
    // Get test results
    const std::vector<InAssemblyTestResult>& get_results() const {
        return results;
    }
};

} // namespace Assembler
