#include "demi_assembler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "preprocessor.hpp"
#include "../test/in_assembly_test_validator.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace Assembler {

std::vector<uint8_t> DemiAssembler::assemble_string(const std::string& source) {
    clear_errors();

    // Preprocessing
    Preprocessor preprocessor;
    std::string preprocessed_source = preprocessor.preprocess(source);
    
    if (preprocessor.has_errors()) {
        collect_errors(preprocessor.get_errors());
        return {};
    }

    // Lexical analysis
    Lexer lexer(preprocessed_source);
    auto tokens = lexer.tokenize();

    if (lexer.has_errors()) {
        collect_errors(lexer.get_errors());
        return {};
    }

    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();

    if (parser.has_errors()) {
        collect_errors(parser.get_errors());
        return {};
    }

    // Code generation
    AssemblerEngine assembler;
    auto bytecode = assembler.assemble(*program);

    if (assembler.has_errors()) {
        collect_errors(assembler.get_errors());
        return {};
    }

    // Store symbols for debugging
    symbols = assembler.get_symbols();
    
    // Store entry address
    entry_address = assembler.get_entry_address();
    
    // Store memory size from .memory directive
    memory_size = assembler.get_memory_size();

    return bytecode;
}

std::vector<uint8_t> DemiAssembler::assemble_string(const std::string& source, const std::string& base_path) {
    clear_errors();

    // Preprocessing with proper base path
    Preprocessor preprocessor;
    std::string preprocessed_source = preprocessor.preprocess(source, base_path);
    
    if (preprocessor.has_errors()) {
        collect_errors(preprocessor.get_errors());
        return {};
    }

    // Lexical analysis
    Lexer lexer(preprocessed_source);
    auto tokens = lexer.tokenize();

    if (lexer.has_errors()) {
        collect_errors(lexer.get_errors());
        return {};
    }

    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();

    if (parser.has_errors()) {
        collect_errors(parser.get_errors());
        return {};
    }

    // Code generation
    AssemblerEngine assembler;
    auto bytecode = assembler.assemble(*program);

    if (assembler.has_errors()) {
        collect_errors(assembler.get_errors());
        return {};
    }

    // Store symbols and entry address
    symbols = assembler.get_symbols();
    entry_address = assembler.get_entry_address();
    
    // Store memory size from .memory directive
    memory_size = assembler.get_memory_size();

    return bytecode;
}

std::vector<uint8_t> DemiAssembler::assemble_file(const std::string& filename) {
    std::string source = read_file(filename);
    if (source.empty() && has_errors()) {
        return {};
    }

    clear_errors();

    // Get the directory of the source file for relative includes
    std::string base_path = std::filesystem::path(filename).parent_path().string();
    
    // Preprocessing with proper base path
    Preprocessor preprocessor;
    std::string preprocessed_source = preprocessor.preprocess(source, base_path);
    
    if (preprocessor.has_errors()) {
        collect_errors(preprocessor.get_errors());
        return {};
    }

    // Lexical analysis
    Lexer lexer(preprocessed_source);
    auto tokens = lexer.tokenize();

    if (lexer.has_errors()) {
        collect_errors(lexer.get_errors());
        return {};
    }

    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();

    if (parser.has_errors()) {
        collect_errors(parser.get_errors());
        return {};
    }

    // Check for conflicting test assertions - simplified for now
    // InAssemblyTestValidator test_validator;
    // auto validation_errors = test_validator.validate_test_assertions(*program);

    // Assembly
    AssemblerEngine assembler;
    auto bytecode = assembler.assemble(*program);

    if (assembler.has_errors()) {
        collect_errors(assembler.get_errors());
        return {};
    }

    // Store symbols for debugging
    symbols = assembler.get_symbols();
    
    // Store entry address
    entry_address = assembler.get_entry_address();
    
    // Store memory size from .memory directive
    memory_size = assembler.get_memory_size();

    return bytecode;
}

void DemiAssembler::clear_errors() {
    all_errors.clear();
    symbols.clear();
    memory_size = 0;
}

void DemiAssembler::collect_errors(const std::vector<std::string>& errors) {
    all_errors.insert(all_errors.end(), errors.begin(), errors.end());
}

std::string DemiAssembler::read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        all_errors.push_back("Cannot open file: " + filename);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int DemiAssembler::validate_and_print_tests(const std::string& filename) {
    clear_errors();
    
    std::string source = read_file(filename);
    if (source.empty()) {
        return 0;
    }
    
    // Lexical analysis
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.has_errors()) {
        collect_errors(lexer.get_errors());
        std::cerr << "Lexer errors while parsing tests:\n";
        for (const auto& error : lexer.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
        return 0;
    }
    
    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();
    
    if (parser.has_errors()) {
        collect_errors(parser.get_errors());
        std::cerr << "Parser errors while parsing tests:\n";
        for (const auto& error : parser.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
        return 0;
    }
    
    // Validate tests
    auto validation_results = InAssemblyTestValidator::validate_tests(*program);
    
    // Print results
    InAssemblyTestValidator::print_validation_results(validation_results);
    
    // Count valid tests
    int valid_count = 0;
    for (const auto& result : validation_results) {
        if (result.valid) {
            valid_count++;
        }
    }
    
    return valid_count;
}

// Convenience functions
std::vector<uint8_t> assemble(const std::string& source) {
    DemiAssembler assembler;
    auto bytecode = assembler.assemble_string(source);

    if (assembler.has_errors()) {
        std::cerr << "Assembly errors:\n";
        for (const auto& error : assembler.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
    }

    return bytecode;
}

std::vector<uint8_t> assemble_file(const std::string& filename) {
    DemiAssembler assembler;
    auto bytecode = assembler.assemble_file(filename);

    if (assembler.has_errors()) {
        std::cerr << "Assembly errors:\n";
        for (const auto& error : assembler.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
    }

    return bytecode;
}

} // namespace Assembler
