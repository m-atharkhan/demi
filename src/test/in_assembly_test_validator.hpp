#pragma once

#include "../assembler/ast.hpp"
#include <vector>
#include <string>
#include <fmt/format.h>

namespace Assembler {

// Simple test validator that checks if tests were parsed correctly
class InAssemblyTestValidator {
public:
    struct ValidationResult {
        std::string test_name;
        bool valid;
        std::string message;
        int statement_count;
        int assertion_count;
        
        ValidationResult(const std::string& name) 
            : test_name(name), valid(true), statement_count(0), assertion_count(0) {}
    };
    
    static std::vector<ValidationResult> validate_tests(const Program& program) {
        std::vector<ValidationResult> results;
        
        for (const auto& test_case : program.test_cases) {
            ValidationResult result(test_case->name);
            
            // Count statements and assertions
            for (const auto& stmt : test_case->body) {
                result.statement_count++;
                
                // Use dynamic_cast to check if it's a TestAssertion
                if (dynamic_cast<TestAssertion*>(stmt.get()) != nullptr) {
                    result.assertion_count++;
                }
            }
            
            // Validate test has at least one statement
            if (result.statement_count == 0) {
                result.valid = false;
                result.message = "Test has no statements";
            }
            // Validate test has at least one assertion (unless it expects error)
            else if (result.assertion_count == 0) {
                result.valid = false;
                result.message = "Test has no assertions";
            }
            else {
                result.message = fmt::format("Test has {} statement(s), {} assertion(s)",
                                           result.statement_count, result.assertion_count);
            }
            
            results.push_back(result);
        }
        
        return results;
    }
    
    static void print_validation_results(const std::vector<ValidationResult>& results) {
        if (results.empty()) {
            fmt::print("No in-assembly tests found.\n");
            return;
        }
        
        fmt::print("\n");
        fmt::print("\033[36m┌──────────────────────────────────────────────────────┐\033[0m\n");
        fmt::print("\033[36m│     In-Assembly Test Validation                      │\033[0m\n");
        fmt::print("\033[36m└──────────────────────────────────────────────────────┘\033[0m\n");
        
        int valid_count = 0;
        for (const auto& result : results) {
            const char* color = result.valid ? "\033[32m" : "\033[31m";
            const char* symbol = result.valid ? "✓" : "✗";
            
            fmt::print("{}{} {}\033[0m\n", color, symbol, result.test_name);
            fmt::print("  {}\n", result.message);
            
            if (result.valid) {
                valid_count++;
            }
        }
        
        const char* summary_color = (valid_count == static_cast<int>(results.size())) ? "\033[32m" : "\033[33m";
        fmt::print("{}\nTests validated: {} / {}\033[0m\n", 
                  summary_color, valid_count, results.size());
        fmt::print("\033[33mNote: Full test execution not yet implemented - only validation performed\033[0m\n");
    }
};

} // namespace Assembler
