#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace Assembler {

// Forward declarations
class ASTNode;
class Statement;
class Expression;
class Instruction;
class Directive;
class Label;

// AST node types
enum class ASTNodeType {
    PROGRAM,
    INSTRUCTION,
    DIRECTIVE,
    LABEL,
    EXPRESSION,
    REGISTER,
    IMMEDIATE,
    MEMORY_REF,
    IDENTIFIER,
    STRING_LITERAL,
    TEST_CASE,
    TEST_ASSERTION
};

// Base AST node
class ASTNode {
public:
    ASTNodeType type;
    size_t line;
    size_t column;
    
    ASTNode(ASTNodeType t, size_t ln = 0, size_t col = 0) 
        : type(t), line(ln), column(col) {}
    virtual ~ASTNode() = default;
};

// Expression types
class Expression : public ASTNode {
public:
    Expression(ASTNodeType t, size_t ln = 0, size_t col = 0) 
        : ASTNode(t, ln, col) {}
};

class RegisterExpression : public Expression {
public:
    std::string name;
    int register_number;  // -1 if not resolved yet
    
    RegisterExpression(const std::string& reg_name, size_t ln = 0, size_t col = 0)
        : Expression(ASTNodeType::REGISTER, ln, col), name(reg_name), register_number(-1) {}
};

class ImmediateExpression : public Expression {
public:
    int64_t value;
    
    ImmediateExpression(int64_t val, size_t ln = 0, size_t col = 0)
        : Expression(ASTNodeType::IMMEDIATE, ln, col), value(val) {}
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    
    IdentifierExpression(const std::string& id_name, size_t ln = 0, size_t col = 0)
        : Expression(ASTNodeType::IDENTIFIER, ln, col), name(id_name) {}
};

class StringLiteralExpression : public Expression {
public:
    std::string value;
    
    StringLiteralExpression(const std::string& str_value, size_t ln = 0, size_t col = 0)
        : Expression(ASTNodeType::STRING_LITERAL, ln, col), value(str_value) {}
};

class MemoryReferenceExpression : public Expression {
public:
    std::unique_ptr<Expression> base;           // [base + offset]
    std::unique_ptr<Expression> offset;        // optional offset
    
    MemoryReferenceExpression(std::unique_ptr<Expression> base_expr, 
                             std::unique_ptr<Expression> offset_expr = nullptr,
                             size_t ln = 0, size_t col = 0)
        : Expression(ASTNodeType::MEMORY_REF, ln, col), 
          base(std::move(base_expr)), offset(std::move(offset_expr)) {}
};

// Statement types
class Statement : public ASTNode {
public:
    Statement(ASTNodeType t, size_t ln = 0, size_t col = 0) 
        : ASTNode(t, ln, col) {}
};

class Label : public Statement {
public:
    std::string name;
    
    Label(const std::string& label_name, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::LABEL, ln, col), name(label_name) {}
};

class Instruction : public Statement {
public:
    std::string mnemonic;
    std::vector<std::unique_ptr<Expression>> operands;
    
    Instruction(const std::string& mn, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::INSTRUCTION, ln, col), mnemonic(mn) {}
    
    void add_operand(std::unique_ptr<Expression> operand) {
        operands.push_back(std::move(operand));
    }
};

class Directive : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    Directive(const std::string& directive_name, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::DIRECTIVE, ln, col), name(directive_name) {}
    
    void add_argument(std::unique_ptr<Expression> arg) {
        arguments.push_back(std::move(arg));
    }
};

// Test assertion types
enum class TestAssertionType {
    ASSERT_MEM,      // #assert_mem address, expected_value
    ASSERT_REG,      // #assert_reg register, expected_value
    ASSERT_OUTPUT,   // #assert_output expected_string
    EXPECT_ERROR     // #expect_error
};

class TestAssertion : public Statement {
public:
    TestAssertionType assertion_type;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    TestAssertion(TestAssertionType type, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::TEST_ASSERTION, ln, col), assertion_type(type) {}
    
    TestAssertion(TestAssertionType type, std::vector<std::unique_ptr<Expression>>&& args, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::TEST_ASSERTION, ln, col), assertion_type(type), arguments(std::move(args)) {}
    
    void add_argument(std::unique_ptr<Expression> arg) {
        arguments.push_back(std::move(arg));
    }
};

class TestCase : public Statement {
public:
    std::string name;
    std::string description;
    std::string author;
    std::string category;
    std::vector<std::string> tags;
    std::vector<std::unique_ptr<Statement>> body;
    
    TestCase(const std::string& test_name, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::TEST_CASE, ln, col), name(test_name) {}
    
    TestCase(const std::string& test_name, std::vector<std::unique_ptr<Statement>>&& test_body, size_t ln = 0, size_t col = 0)
        : Statement(ASTNodeType::TEST_CASE, ln, col), name(test_name), body(std::move(test_body)) {}
    
    void add_statement(std::unique_ptr<Statement> stmt) {
        body.push_back(std::move(stmt));
    }
    
    void set_description(const std::string& desc) {
        description = desc;
    }
    
    void set_author(const std::string& auth) {
        author = auth;
    }
    
    void set_category(const std::string& cat) {
        category = cat;
    }
    
    void add_tag(const std::string& tag) {
        tags.push_back(tag);
    }
};

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    std::vector<std::unique_ptr<TestCase>> test_cases;
    
    Program() : ASTNode(ASTNodeType::PROGRAM) {}
    
    void add_statement(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }
    
    void add_test_case(std::unique_ptr<TestCase> test) {
        test_cases.push_back(std::move(test));
    }
};

} // namespace Assembler
