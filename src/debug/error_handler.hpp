#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <fmt/core.h>

namespace Logging {

/**
 * @brief Enumeration of error codes with structured categorization
 * 
 * Error codes are organized by subsystem:
 * - 0x001-0x099: Parser/Lexer errors
 * - 0x100-0x199: Assembly errors
 * - 0x200-0x299: Runtime/CPU errors
 * - 0x300-0x399: Memory errors
 * - 0x400-0x499: I/O errors
 * - 0x500-0x599: Test framework errors
 */
enum class ErrorCode {
    // Parser/Lexer (0x001-0x099)
    PARSE_DUPLICATE_LABEL = 0x001,
    PARSE_UNKNOWN_DIRECTIVE = 0x002,
    PARSE_INVALID_OPERAND = 0x003,
    PARSE_MISSING_OPERAND = 0x004,
    PARSE_INVALID_NUMBER = 0x005,
    PARSE_UNEXPECTED_TOKEN = 0x006,
    PARSE_GENERIC = 0x007,
    
    // Assembly (0x100-0x199)
    ASM_UNKNOWN_INSTRUCTION = 0x100,
    ASM_INVALID_REGISTER = 0x101,
    ASM_INVALID_IMMEDIATE = 0x102,
    ASM_SYMBOL_NOT_FOUND = 0x103,
    ASM_FORWARD_REF_FAILED = 0x104,
    ASM_ENCODING_NOT_IMPL = 0x105,
    ASM_DIRECTIVE_ERROR = 0x106,
    ASM_GENERIC = 0x1FF,
    
    // Runtime/CPU (0x200-0x299)
    CPU_DIVISION_BY_ZERO = 0x200,
    CPU_MODULO_BY_ZERO = 0x201,
    CPU_MEMORY_OUT_OF_BOUNDS = 0x202,
    CPU_INVALID_REGISTER = 0x203,
    CPU_CALL_STACK_OVERFLOW = 0x204,
    CPU_STACK_OVERFLOW = 0x205,
    CPU_STACK_UNDERFLOW = 0x206,
    CPU_INVALID_JUMP = 0x207,
    CPU_INVALID_OPCODE = 0x208,
    CPU_GENERIC = 0x2FF,
    
    // Memory (0x300-0x399)
    MEM_OUT_OF_BOUNDS = 0x300,
    MEM_INVALID_ACCESS = 0x301,
    MEM_INVALID_ALIGNMENT = 0x302,
    MEM_GENERIC = 0x3FF,
    
    // I/O (0x400-0x499)
    IO_FILE_NOT_FOUND = 0x400,
    IO_DIRECTORY_NOT_FOUND = 0x401,
    IO_FILE_READ_ERROR = 0x402,
    IO_FILE_WRITE_ERROR = 0x403,
    IO_INVALID_PATH = 0x404,
    IO_PERMISSION_DENIED = 0x405,
    IO_GENERIC = 0x4FF,
    
    // Test Framework (0x500-0x599)
    TEST_EXCEEDED_MAX_STEPS = 0x500,
    TEST_ASSERTION_FAILED = 0x501,
    TEST_UNEXPECTED_ERROR = 0x502,
    TEST_INVALID_FORMAT = 0x503,
    TEST_GENERIC = 0x5FF,
};

/**
 * @brief Severity levels for errors
 */
enum class ErrorSeverity {
    WARNING,    ///< Non-fatal, operation may continue
    ERROR,      ///< Fatal error, operation must stop
    FATAL,      ///< Critical error, program must terminate
};

/**
 * @brief Structured error information with context
 */
struct ErrorContext {
    ErrorCode code = ErrorCode::ASM_GENERIC;
    ErrorSeverity severity = ErrorSeverity::ERROR;
    std::string message;
    std::string file;           ///< Source file name
    size_t line = 0;            ///< Line number in source
    size_t column = 0;          ///< Column number in source
    uint32_t pc = 0;            ///< Program counter at error
    std::string context_info;   ///< Additional context (register values, etc.)
    std::string suggestion;     ///< User-friendly fix suggestion
    
    ErrorContext() = default;
    ErrorContext(ErrorCode c, const std::string& msg) 
        : code(c), message(msg) {}
};

/**
 * @brief Centralized error handling and reporting
 * 
 * Provides:
 * - Structured error codes for categorization
 * - Rich context information (PC, registers, line/column)
 * - Consistent error message formatting
 * - Prevention of duplicate error reporting
 * - Error recovery suggestions
 */
class ErrorHandler {
public:
    /**
     * @brief Get singleton instance of ErrorHandler
     * @return Reference to ErrorHandler instance
     */
    static ErrorHandler& instance();
    
    // Delete copy/move operations
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    /**
     * @brief Report an error with full context
     * @param error Error context structure
     */
    void report(const ErrorContext& error);
    
    /**
     * @brief Report an error with minimal information
     * @param code Error code
     * @param message Error message
     * @param severity Error severity level
     */
    void report(ErrorCode code, const std::string& message, 
                ErrorSeverity severity = ErrorSeverity::ERROR);
    
    /**
     * @brief Report a runtime error with CPU context
     * @param code Error code
     * @param message Error message
     * @param pc Program counter at error
     * @param context Additional context string
     */
    void report_runtime(ErrorCode code, const std::string& message, 
                       uint32_t pc, const std::string& context = "");
    
    /**
     * @brief Report a parse error with source location
     * @param code Error code
     * @param message Error message
     * @param file Source file name
     * @param line Line number
     * @param column Column number
     */
    void report_parse(ErrorCode code, const std::string& message,
                     const std::string& file = "", size_t line = 0, size_t column = 0);
    
    /**
     * @brief Report an I/O error
     * @param code Error code
     * @param path File path that caused error
     * @param reason Reason for error
     */
    void report_io(ErrorCode code, const std::string& path, const std::string& reason);
    
    /**
     * @brief Get human-readable error code string
     * @param code Error code
     * @return String representation like "[ERROR_0x200]"
     */
    static std::string code_to_string(ErrorCode code);
    
    /**
     * @brief Get human-readable severity string
     * @param severity Severity level
     * @return String representation like "ERROR", "WARNING", "FATAL"
     */
    static std::string severity_to_string(ErrorSeverity severity);
    
    /**
     * @brief Format error context into a detailed message
     * @param error Error context
     * @return Formatted error string ready for logging
     */
    static std::string format_error(const ErrorContext& error);
    
    /**
     * @brief Clear error history (for testing)
     */
    void clear();
    
    /**
     * @brief Get total error count
     * @return Number of errors reported
     */
    size_t get_error_count() const { return error_count_; }
    
    /**
     * @brief Set quiet mode (suppress logging during test parsing)
     * @param quiet If true, only log FATAL errors
     */
    void set_quiet_mode(bool quiet) { quiet_mode_ = quiet; }
    
    /**
     * @brief Get current quiet mode state
     * @return True if in quiet mode
     */
    bool is_quiet_mode() const { return quiet_mode_; }
    
private:
    ErrorHandler() = default;
    
    size_t error_count_ = 0;
    bool quiet_mode_ = false;
    
    /**
     * @brief Log formatted error to Logger and/or stderr
     * @param error Error context
     */
    void log_error(const ErrorContext& error);
};

} // namespace Logging
