#include "parser.hpp"

namespace Assembler {

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    
    skip_newlines();
    
    while (!is_at_end()) {
        auto stmt = parse_statement();
        if (stmt) {
            // Check if this is a test case using dynamic_cast
            if (auto* test_ptr = dynamic_cast<TestCase*>(stmt.get())) {
                // Move ownership from stmt to test_cases
                stmt.release();
                program->add_test_case(std::unique_ptr<TestCase>(test_ptr));
            } else {
                program->add_statement(std::move(stmt));
            }
        }
        skip_newlines();
    }
    
    return program;
}

const Token& Parser::current_token() const {
    if (pos >= tokens.size()) {
        static Token eof_token(TokenType::END_OF_FILE, "");
        return eof_token;
    }
    return tokens[pos];
}

const Token& Parser::peek_token(size_t offset) const {
    size_t peek_pos = pos + offset;
    if (peek_pos >= tokens.size()) {
        static Token eof_token(TokenType::END_OF_FILE, "");
        return eof_token;
    }
    return tokens[peek_pos];
}

void Parser::advance() {
    if (pos < tokens.size()) {
        pos++;
    }
}

bool Parser::is_at_end() const {
    return pos >= tokens.size() || current_token().type == TokenType::END_OF_FILE;
}

bool Parser::match(TokenType type) {
    if (current_token().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::consume(TokenType type, const std::string& error_message) {
    if (current_token().type == type) {
        advance();
        return true;
    }
    add_error(error_message, current_token());
    return false;
}

void Parser::skip_newlines() {
    while (current_token().type == TokenType::NEWLINE) {
        advance();
    }
}

std::unique_ptr<Statement> Parser::parse_statement() {
    const Token& token = current_token();
    
    switch (token.type) {
        case TokenType::MNEMONIC: {
            std::string mnemonic = token.text;
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_instruction(mnemonic, line, col);
        }
        
        case TokenType::DIRECTIVE: {
            std::string directive = token.text;
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_directive(directive, line, col);
        }
        
        case TokenType::TEST_DIRECTIVE: {
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_test_case(line, col);
        }
        
        case TokenType::ASSERT_MEM: {
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_test_assertion(TestAssertionType::ASSERT_MEM, line, col);
        }
        
        case TokenType::ASSERT_REG: {
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_test_assertion(TestAssertionType::ASSERT_REG, line, col);
        }
        
        case TokenType::ASSERT_OUTPUT: {
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_test_assertion(TestAssertionType::ASSERT_OUTPUT, line, col);
        }
        
        case TokenType::EXPECT_ERROR: {
            size_t line = token.line;
            size_t col = token.column;
            advance();
            return parse_test_assertion(TestAssertionType::EXPECT_ERROR, line, col);
        }
        
        case TokenType::IDENTIFIER: {
            // Could be a label if followed by colon
            if (peek_token().type == TokenType::COLON) {
                std::string label_name = token.text;
                size_t line = token.line;
                size_t col = token.column;
                advance(); // identifier
                advance(); // colon
                return parse_label(label_name, line, col);
            } else {
                add_error("Unexpected identifier", token);
                advance();
                return nullptr;
            }
        }
        
        case TokenType::NEWLINE:
            advance();
            return nullptr;
            
        case TokenType::END_OF_FILE:
            return nullptr;
            
        default:
            add_error("Unexpected token", token);
            advance();
            return nullptr;
    }
}

std::unique_ptr<Instruction> Parser::parse_instruction(const std::string& mnemonic, size_t line, size_t col) {
    auto instruction = std::make_unique<Instruction>(mnemonic, line, col);
    
    // Parse operands (comma-separated)
    if (current_token().type != TokenType::NEWLINE && 
        current_token().type != TokenType::END_OF_FILE) {
        
        do {
            auto operand = parse_expression();
            if (operand) {
                instruction->add_operand(std::move(operand));
            }
        } while (match(TokenType::COMMA));
    }
    
    return instruction;
}

std::unique_ptr<Directive> Parser::parse_directive(const std::string& directive_name, size_t line, size_t col) {
    auto directive = std::make_unique<Directive>(directive_name, line, col);
    
    // Parse arguments
    if (current_token().type != TokenType::NEWLINE && 
        current_token().type != TokenType::END_OF_FILE) {
        
        do {
            auto arg = parse_expression();
            if (arg) {
                directive->add_argument(std::move(arg));
            }
        } while (match(TokenType::COMMA));
    }
    
    return directive;
}

std::unique_ptr<Label> Parser::parse_label(const std::string& label_name, size_t line, size_t col) {
    return std::make_unique<Label>(label_name, line, col);
}

std::unique_ptr<Expression> Parser::parse_expression() {
    // Handle memory references [expression]
    if (current_token().type == TokenType::LBRACKET) {
        return parse_memory_reference();
    }
    
    return parse_primary_expression();
}

std::unique_ptr<Expression> Parser::parse_primary_expression() {
    const Token& token = current_token();
    
    switch (token.type) {
        case TokenType::REGISTER: {
            std::string reg_name = token.text;
            advance();
            return std::make_unique<RegisterExpression>(reg_name, token.line, token.column);
        }
        
        case TokenType::NUMBER: {
            int64_t value = token.as_int();
            advance();
            return std::make_unique<ImmediateExpression>(value, token.line, token.column);
        }
        
        case TokenType::IDENTIFIER: {
            std::string name = token.text;
            advance();
            return std::make_unique<IdentifierExpression>(name, token.line, token.column);
        }
        
        case TokenType::STRING: {
            std::string value = token.as_string();
            advance();
            return std::make_unique<StringLiteralExpression>(value, token.line, token.column);
        }
        
        default:
            add_error("Expected expression", token);
            advance();
            return nullptr;
    }
}

std::unique_ptr<Expression> Parser::parse_memory_reference() {
    if (!consume(TokenType::LBRACKET, "Expected '['")) {
        return nullptr;
    }
    
    auto base = parse_primary_expression();
    if (!base) {
        return nullptr;
    }
    
    std::unique_ptr<Expression> offset = nullptr;
    
    // Check for offset: [base + offset] or [base - offset]
    if (current_token().type == TokenType::PLUS || current_token().type == TokenType::MINUS) {
        bool is_negative = current_token().type == TokenType::MINUS;
        advance(); // consume + or -
        
        offset = parse_primary_expression();
        if (offset && is_negative) {
            // Convert to negative immediate if it's a number
            if (auto imm = dynamic_cast<ImmediateExpression*>(offset.get())) {
                imm->value = -imm->value;
            }
        }
    }
    
    if (!consume(TokenType::RBRACKET, "Expected ']'")) {
        return nullptr;
    }
    
    return std::make_unique<MemoryReferenceExpression>(std::move(base), std::move(offset));
}

std::unique_ptr<TestCase> Parser::parse_test_case(size_t line, size_t col) {
    // Expect: #test "test name" { ... }
    
    // Parse test name (should be a STRING token)
    if (current_token().type != TokenType::STRING) {
        add_error("Expected string literal for test name", current_token());
        return nullptr;
    }
    
    std::string test_name = current_token().as_string();
    advance();
    
    // Expect opening brace
    if (!consume(TokenType::LBRACE, "Expected '{' to start test body")) {
        return nullptr;
    }
    
    // Create test case
    auto test_case = std::make_unique<TestCase>(test_name, line, col);
    
    // Parse test body statements and metadata
    while (current_token().type != TokenType::RBRACE && 
           current_token().type != TokenType::END_OF_FILE) {
        
        // Handle metadata directives
        if (current_token().type == TokenType::DESCRIPTION) {
            advance();
            if (current_token().type == TokenType::STRING) {
                test_case->set_description(current_token().as_string());
                advance();
            } else {
                add_error("Expected string after #description", current_token());
            }
            continue;
        } else if (current_token().type == TokenType::AUTHOR) {
            advance();
            if (current_token().type == TokenType::STRING) {
                test_case->set_author(current_token().as_string());
                advance();
            } else {
                add_error("Expected string after #author", current_token());
            }
            continue;
        } else if (current_token().type == TokenType::CATEGORY) {
            advance();
            if (current_token().type == TokenType::STRING) {
                test_case->set_category(current_token().as_string());
                advance();
            } else {
                add_error("Expected string after #category", current_token());
            }
            continue;
        } else if (current_token().type == TokenType::TAG) {
            advance();
            if (current_token().type == TokenType::STRING) {
                test_case->add_tag(current_token().as_string());
                advance();
            } else {
                add_error("Expected string after #tag", current_token());
            }
            continue;
        }
        
        // Parse regular statements
        auto stmt = parse_statement();
        if (stmt) {
            test_case->add_statement(std::move(stmt));
        }
    }
    
    // Expect closing brace
    if (!consume(TokenType::RBRACE, "Expected '}' to close test body")) {
        return nullptr;
    }
    
    return test_case;
}

std::unique_ptr<TestAssertion> Parser::parse_test_assertion(TestAssertionType type, size_t line, size_t col) {
    // Parse assertion arguments - all assertions expect comma-separated expressions
    std::vector<std::unique_ptr<Expression>> args;
    
    // Different assertions expect different numbers of arguments:
    // ASSERT_MEM: address, expected_value
    // ASSERT_REG: register_name, expected_value  
    // ASSERT_OUTPUT: expected_string
    // EXPECT_ERROR: error_message (optional)
    
    if (current_token().type == TokenType::NEWLINE || 
        current_token().type == TokenType::END_OF_FILE) {
        // No arguments (only valid for EXPECT_ERROR)
        if (type != TestAssertionType::EXPECT_ERROR) {
            add_error("Expected arguments for assertion", current_token());
            return nullptr;
        }
        return std::make_unique<TestAssertion>(type, std::move(args), line, col);
    }
    
    // Parse comma-separated argument list
    do {
        auto arg = parse_expression();
        if (!arg) {
            add_error("Expected expression in assertion", current_token());
            return nullptr;
        }
        args.push_back(std::move(arg));
        
        if (current_token().type == TokenType::COMMA) {
            advance();
        } else {
            break;
        }
    } while (true);
    
    // Validate argument counts
    switch (type) {
        case TestAssertionType::ASSERT_MEM:
        case TestAssertionType::ASSERT_REG:
            if (args.size() != 2) {
                add_error("Expected 2 arguments for " + 
                         std::string(type == TestAssertionType::ASSERT_MEM ? "#assert_mem" : "#assert_reg"));
                return nullptr;
            }
            break;
        case TestAssertionType::ASSERT_OUTPUT:
            if (args.size() != 1) {
                add_error("Expected 1 argument for #assert_output");
                return nullptr;
            }
            break;
        case TestAssertionType::EXPECT_ERROR:
            if (args.size() > 1) {
                add_error("Expected 0 or 1 arguments for #expect_error");
                return nullptr;
            }
            break;
    }
    
    return std::make_unique<TestAssertion>(type, std::move(args), line, col);
}

void Parser::add_error(const std::string& message) {
    errors.push_back("Parse error: " + message);
}

void Parser::add_error(const std::string& message, const Token& token) {
    errors.push_back("Line " + std::to_string(token.line) + ", Column " + std::to_string(token.column) + 
                    ": " + message + " (got '" + token.text + "')");
}

} // namespace Assembler
