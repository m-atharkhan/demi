#pragma once
#include <string>
#include <variant>
#include <cstdint>

namespace Assembler {

enum class TokenType {
    // Literals
    IDENTIFIER,      // labels, symbols
    NUMBER,          // numeric literals (decimal, hex, binary)
    STRING,          // string literals
    REGISTER,        // register names (R0, RAX, etc.)
    
    // Instruction mnemonics
    MNEMONIC,        // instruction names (ADD, MOV, JMP, etc.)
    
    // Symbols and operators
    COMMA,           // ,
    COLON,           // :
    SEMICOLON,       // ;
    DOT,             // .
    HASH,            // #
    LBRACE,          // {
    RBRACE,          // }
    LPAREN,          // (
    RPAREN,          // )
    
    // Addressing modes
    LBRACKET,        // [
    RBRACKET,        // ]
    PLUS,            // +
    MINUS,           // -
    ASTERISK,        // *
    
    // Memory size specifiers
    SIZE_SPECIFIER,  // byte, word, dword, qword
    
    // Directives
    DIRECTIVE,       // .data, .text, .org, etc.
    SECTION,         // section
    GLOBAL,          // global
    EXTERN,          // extern
    
    // Test directives
    TEST_DIRECTIVE,  // .test
    ASSERT_MEM,      // .assert_mem
    ASSERT_REG,      // .assert_reg
    ASSERT_FPU,      // .assert_fpu
    ASSERT_OUTPUT,   // .assert_output
    EXPECT_ERROR,    // .expect_error
    ENTRY_POINT,     // .entry_point (test-specific)
    
    // Test metadata directives
    DESCRIPTION,     // .description
    AUTHOR,          // .author
    CATEGORY,        // .category
    TAG,             // .tag
    MAXSTEPS,        // .maxsteps
    MAXCALLDEPTH,    // .maxcalldepth
    TIMEOUT,         // .timeout
    SKIP,            // .skip
    
    // Benchmark directives
    BENCHMARK,       // .benchmark
    WARMUP,          // .warmup
    ITERATIONS,      // .iterations
    MEASURE,         // .measure

    // Preprocessor directives
    INCLUDE,         // .include
    DEFINE,          // .define
    IFDEF,           // .ifdef
    IFNDEF,          // .ifndef
    ELIF,            // .elif
    ELSE,            // .else
    ENDIF,           // .endif
    UNDEF,           // .undef

    // Debug directives
    PRINT,           // .print
    BREAK,           // .break
    DUMP,            // .dump
    MEMDUMP,         // .memdump
    TRACE,           // .trace
    ASSERT,          // .assert
    DUMPSTACK,       // .dumpstack
    WATCH,           // .watch
    UNWATCH,         // .unwatch
    CHECKPOINT,      // .checkpoint
    LOG,             // .log
    DUMPREG,         // .dumpreg
    MEMSET,          // .memset
    STEP,            // .step

    // Special
    NEWLINE,         // \n
    END_OF_FILE,     // EOF
    INVALID          // invalid token
};

struct Token {
    TokenType type;
    std::string text;
    std::variant<int64_t, uint64_t, double, std::string> value;
    size_t line;
    size_t column;
    
    Token(TokenType t, const std::string& txt, size_t ln = 0, size_t col = 0)
        : type(t), text(txt), line(ln), column(col) {}
    
    // Helper methods for value access
    bool is_number() const { return std::holds_alternative<int64_t>(value) || std::holds_alternative<uint64_t>(value); }
    bool is_float() const { return std::holds_alternative<double>(value); }
    bool is_string() const { return std::holds_alternative<std::string>(value); }
    
    int64_t as_int() const {
        if (auto* p = std::get_if<int64_t>(&value)) return *p;
        if (auto* p = std::get_if<uint64_t>(&value)) return static_cast<int64_t>(*p);
        return 0;  // fallback: non-numeric token
    }

    uint64_t as_uint() const {
        if (auto* p = std::get_if<uint64_t>(&value)) return *p;
        if (auto* p = std::get_if<int64_t>(&value)) return static_cast<uint64_t>(*p);
        return 0;  // fallback: non-numeric token
    }

    double as_float() const {
        if (auto* p = std::get_if<double>(&value)) return *p;
        if (auto* p = std::get_if<int64_t>(&value)) return static_cast<double>(*p);
        if (auto* p = std::get_if<uint64_t>(&value)) return static_cast<double>(*p);
        return 0.0;  // fallback: non-numeric token
    }

    std::string as_string() const {
        if (auto* p = std::get_if<std::string>(&value)) return *p;
        return text;  // fallback: raw token text (works for identifiers, mnemonics, etc.)
    }
};

} // namespace Assembler
