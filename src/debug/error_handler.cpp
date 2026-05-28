#include "error_handler.hpp"
#include "logger.hpp"
#include "debug_handler.hpp"
#include "../config.hpp"
#include <iostream>

namespace Logging {

ErrorHandler& ErrorHandler::instance() {
    static ErrorHandler handler;
    return handler;
}

void ErrorHandler::report(const ErrorContext& error) {
    error_count_++;
    Config::error_count++;
    log_error(error);
}

void ErrorHandler::report(ErrorCode code, const std::string& message, 
                         ErrorSeverity severity) {
    ErrorContext error(code, message);
    error.severity = severity;
    report(error);
}

void ErrorHandler::report_runtime(ErrorCode code, const std::string& message,
                                 uint32_t pc, const std::string& context) {
    ErrorContext error(code, message);
    error.pc = pc;
    error.context_info = context;
    error.severity = ErrorSeverity::ERROR;
    report(error);
}

void ErrorHandler::report_parse(ErrorCode code, const std::string& message,
                               const std::string& file, size_t line, size_t column) {
    ErrorContext error(code, message);
    error.file = file;
    error.line = line;
    error.column = column;
    error.severity = ErrorSeverity::ERROR;
    report(error);
}

void ErrorHandler::report_io(ErrorCode code, const std::string& path, 
                            const std::string& reason) {
    std::string message = fmt::format("I/O operation failed on '{}': {}", path, reason);
    ErrorContext error(code, message);
    error.severity = ErrorSeverity::ERROR;
    report(error);
}

std::string ErrorHandler::code_to_string(ErrorCode code) {
    switch (code) {
        // Parser/Lexer
        case ErrorCode::PARSE_DUPLICATE_LABEL:
            return "PARSE_DUPLICATE_LABEL (0x001)";
        case ErrorCode::PARSE_UNKNOWN_DIRECTIVE:
            return "PARSE_UNKNOWN_DIRECTIVE (0x002)";
        case ErrorCode::PARSE_INVALID_OPERAND:
            return "PARSE_INVALID_OPERAND (0x003)";
        case ErrorCode::PARSE_MISSING_OPERAND:
            return "PARSE_MISSING_OPERAND (0x004)";
        case ErrorCode::PARSE_INVALID_NUMBER:
            return "PARSE_INVALID_NUMBER (0x005)";
        case ErrorCode::PARSE_UNEXPECTED_TOKEN:
            return "PARSE_UNEXPECTED_TOKEN (0x006)";
        case ErrorCode::PARSE_GENERIC:
            return "PARSE_ERROR (0x007)";
            
        // Assembly
        case ErrorCode::ASM_UNKNOWN_INSTRUCTION:
            return "ASM_UNKNOWN_INSTRUCTION (0x100)";
        case ErrorCode::ASM_INVALID_REGISTER:
            return "ASM_INVALID_REGISTER (0x101)";
        case ErrorCode::ASM_INVALID_IMMEDIATE:
            return "ASM_INVALID_IMMEDIATE (0x102)";
        case ErrorCode::ASM_SYMBOL_NOT_FOUND:
            return "ASM_SYMBOL_NOT_FOUND (0x103)";
        case ErrorCode::ASM_FORWARD_REF_FAILED:
            return "ASM_FORWARD_REF_FAILED (0x104)";
        case ErrorCode::ASM_ENCODING_NOT_IMPL:
            return "ASM_ENCODING_NOT_IMPL (0x105)";
        case ErrorCode::ASM_DIRECTIVE_ERROR:
            return "ASM_DIRECTIVE_ERROR (0x106)";
        case ErrorCode::ASM_GENERIC:
            return "ASM_ERROR (0x1FF)";
            
        // Runtime/CPU
        case ErrorCode::CPU_DIVISION_BY_ZERO:
            return "CPU_DIVISION_BY_ZERO (0x200)";
        case ErrorCode::CPU_MODULO_BY_ZERO:
            return "CPU_MODULO_BY_ZERO (0x201)";
        case ErrorCode::CPU_MEMORY_OUT_OF_BOUNDS:
            return "CPU_MEMORY_OUT_OF_BOUNDS (0x202)";
        case ErrorCode::CPU_INVALID_REGISTER:
            return "CPU_INVALID_REGISTER (0x203)";
        case ErrorCode::CPU_CALL_STACK_OVERFLOW:
            return "CPU_CALL_STACK_OVERFLOW (0x204)";
        case ErrorCode::CPU_STACK_OVERFLOW:
            return "CPU_STACK_OVERFLOW (0x205)";
        case ErrorCode::CPU_STACK_UNDERFLOW:
            return "CPU_STACK_UNDERFLOW (0x206)";
        case ErrorCode::CPU_INVALID_JUMP:
            return "CPU_INVALID_JUMP (0x207)";
        case ErrorCode::CPU_INVALID_OPCODE:
            return "CPU_INVALID_OPCODE (0x208)";
        case ErrorCode::CPU_GENERIC:
            return "CPU_ERROR (0x2FF)";
            
        // Memory
        case ErrorCode::MEM_OUT_OF_BOUNDS:
            return "MEM_OUT_OF_BOUNDS (0x300)";
        case ErrorCode::MEM_INVALID_ACCESS:
            return "MEM_INVALID_ACCESS (0x301)";
        case ErrorCode::MEM_INVALID_ALIGNMENT:
            return "MEM_INVALID_ALIGNMENT (0x302)";
        case ErrorCode::MEM_GENERIC:
            return "MEM_ERROR (0x3FF)";
            
        // I/O
        case ErrorCode::IO_FILE_NOT_FOUND:
            return "IO_FILE_NOT_FOUND (0x400)";
        case ErrorCode::IO_DIRECTORY_NOT_FOUND:
            return "IO_DIRECTORY_NOT_FOUND (0x401)";
        case ErrorCode::IO_FILE_READ_ERROR:
            return "IO_FILE_READ_ERROR (0x402)";
        case ErrorCode::IO_FILE_WRITE_ERROR:
            return "IO_FILE_WRITE_ERROR (0x403)";
        case ErrorCode::IO_INVALID_PATH:
            return "IO_INVALID_PATH (0x404)";
        case ErrorCode::IO_PERMISSION_DENIED:
            return "IO_PERMISSION_DENIED (0x405)";
        case ErrorCode::IO_GENERIC:
            return "IO_ERROR (0x4FF)";
            
        // Test Framework
        case ErrorCode::TEST_EXCEEDED_MAX_STEPS:
            return "TEST_EXCEEDED_MAX_STEPS (0x500)";
        case ErrorCode::TEST_ASSERTION_FAILED:
            return "TEST_ASSERTION_FAILED (0x501)";
        case ErrorCode::TEST_UNEXPECTED_ERROR:
            return "TEST_UNEXPECTED_ERROR (0x502)";
        case ErrorCode::TEST_INVALID_FORMAT:
            return "TEST_INVALID_FORMAT (0x503)";
        case ErrorCode::TEST_GENERIC:
            return "TEST_ERROR (0x5FF)";
            
        default:
            return "UNKNOWN_ERROR";
    }
}

std::string ErrorHandler::severity_to_string(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::WARNING:
            return "WARNING";
        case ErrorSeverity::ERROR:
            return "ERROR";
        case ErrorSeverity::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

std::string ErrorHandler::format_error(const ErrorContext& error) {
    std::ostringstream oss;
    
    // Header with code and severity
    oss << "[" << code_to_string(error.code) << "] [" 
        << severity_to_string(error.severity) << "] ";
    
    // Location information if available
    if (!error.file.empty()) {
        oss << error.file;
        if (error.line > 0) {
            oss << ":" << error.line;
            if (error.column > 0) {
                oss << ":" << error.column;
            }
        }
        oss << " ";
    }
    
    // Main message
    oss << error.message;
    
    // Context information (CPU state, registers, etc.)
    if (!error.context_info.empty()) {
        oss << "\n  Context: " << error.context_info;
    }
    
    // PC if available
    if (error.pc != 0) {
        oss << "\n  At PC: 0x" << std::hex << error.pc << std::dec;
    }
    
    // Suggestion for fixing
    if (!error.suggestion.empty()) {
        oss << "\n  Suggestion: " << error.suggestion;
    }
    
    return oss.str();
}

void ErrorHandler::log_error(const ErrorContext& error) {
    // In quiet mode or test mode, only log FATAL errors
    if ((quiet_mode_ || Config::test_mode) && error.severity != ErrorSeverity::FATAL) {
        return;
    }
    
    std::string formatted = format_error(error);
    
    // Log through DebugHandler based on severity
    switch (error.severity) {
        case ErrorSeverity::WARNING:
            DebugHandler::instance().report(DebugCategory::DEBUG_GENERIC, formatted, DebugLevel::IMPORTANT);
            break;
        case ErrorSeverity::ERROR:
        case ErrorSeverity::FATAL:
            DebugHandler::instance().report(DebugCategory::DEBUG_GENERIC, formatted, DebugLevel::CRITICAL);
            break;
    }
}

void ErrorHandler::clear() {
    error_count_ = 0;
    Config::error_count = 0;
}

} // namespace Logging
