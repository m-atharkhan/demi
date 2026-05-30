#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <deque>
#include <mutex>
#include <fmt/core.h>
#include "../config.hpp"

namespace Logging {

/**
 * @brief Enumeration of debug context categories with structured codes
 * 
 * Debug categories are organized by subsystem:
 * - 0x001-0x099: CPU/Engine debug
 * - 0x100-0x199: Assembly/Parser debug  
 * - 0x200-0x299: Memory/Storage debug
 * - 0x300-0x399: I/O/Device debug
 * - 0x400-0x499: Test/Execution debug
 * - 0x500-0x599: GUI/Interface debug
 * - 0x600-0x699: Performance/Profiling debug
 */
enum class DebugCategory {
    // CPU/Engine (0x001-0x099)
    CPU_EXECUTION = 0x001,
    CPU_REGISTERS = 0x002,
    CPU_FLAGS = 0x003,
    CPU_STACK = 0x004,
    CPU_JUMP = 0x005,
    CPU_DISPATCHER = 0x006,
    CPU_PREDICTION = 0x007,
    CPU_PIPELINE = 0x008,
    
    // Assembly/Parser (0x100-0x199)
    ASM_PARSING = 0x100,
    ASM_INSTRUCTION = 0x101,
    ASM_DIRECTIVE = 0x102,
    ASM_SYMBOL = 0x103,
    ASM_FORWARD_REF = 0x104,
    ASM_ENCODING = 0x105,
    ASM_OPTIMIZATION = 0x106,
    ASM_HEXDUMP = 0x107,
    
    // Memory/Storage (0x200-0x299)
    MEM_ACCESS = 0x200,
    MEM_ALLOCATION = 0x201,
    MEM_BOUNDS = 0x202,
    MEM_VIRTUAL = 0x203,
    MEM_CACHE = 0x204,
    MEM_PROTECTION = 0x205,
    
    // I/O/Device (0x300-0x399)
    IO_DEVICE = 0x300,
    IO_FILE = 0x301,
    IO_CONSOLE = 0x302,
    IO_RAMDISK = 0x303,
    IO_NETWORK = 0x304,
    IO_INTERRUPT = 0x305,
    
    // Test/Execution (0x400-0x499)
    TEST_EXECUTION = 0x400,
    TEST_ASSERTION = 0x401,
    TEST_FRAMEWORK = 0x402,
    TEST_VALIDATION = 0x403,
    TEST_PERFORMANCE = 0x404,
    
    // GUI/Interface (0x500-0x599)
    GUI_WINDOW = 0x500,
    GUI_CONTROLS = 0x501,
    GUI_RENDERING = 0x502,
    GUI_EVENTS = 0x503,
    GUI_STATE = 0x504,
    
    // Performance/Profiling (0x600-0x699)
    PERF_TIMING = 0x600,
    PERF_COUNTERS = 0x601,
    PERF_OPTIMIZATION = 0x602,
    PERF_BOTTLENECKS = 0x603,
    
    // Generic/Misc
    DEBUG_GENERIC = 0x999
};

/**
 * @brief Debug levels for filtering and importance
 */
enum class DebugLevel {
    TRACE,      ///< Very detailed execution flow (verbose)
    DETAIL,     ///< Detailed state information
    INFO,       ///< General debug information  
    IMPORTANT,  ///< Important events/decisions
    CRITICAL    ///< Critical debug information (always shown)
};

/**
 * @brief Structured debug information with rich context
 */
struct DebugContext {
    DebugCategory category = DebugCategory::DEBUG_GENERIC;
    DebugLevel level = DebugLevel::INFO;
    std::string message;
    std::string function;       ///< Function name where debug originated
    std::string file;           ///< Source file name
    size_t line = 0;            ///< Line number in source
    uint32_t pc = 0;            ///< Program counter at debug point
    uint32_t cycle = 0;         ///< CPU cycle count
    std::string context_data;   ///< Structured context (registers, memory, etc.)
    std::unordered_map<std::string, std::string> metadata; ///< Additional key-value data
    
    DebugContext() = default;
    DebugContext(DebugCategory cat, const std::string& msg, DebugLevel lvl = DebugLevel::INFO) 
        : category(cat), level(lvl), message(msg) {}
};

/**
 * @brief Advanced debug handler with structured logging, filtering, and context
 * 
 * Provides:
 * - Categorized debug codes for systematic debugging
 * - Rich context information (PC, cycle, registers, memory)
 * - Intelligent filtering by category, level, and frequency
 * - Performance impact monitoring and adaptive throttling
 * - Debug session recording and replay capabilities
 * - Integration with existing Logger system
 */
class DebugHandler {
public:
    /**
     * @brief Get singleton instance of DebugHandler
     * @return Reference to DebugHandler instance
     */
    static DebugHandler& instance();
    
    // Delete copy/move operations
    DebugHandler(const DebugHandler&) = delete;
    DebugHandler& operator=(const DebugHandler&) = delete;
    
    // ============================================================================
    // Core Debug Reporting Interface
    // ============================================================================
    
    /**
     * @brief Report debug information with full context
     * @param debug Debug context structure
     */
    void report(const DebugContext& debug);

    /**
     * @brief Suppress all output (useful for expected errors in tests)
     * @param suppress Whether to suppress output
     */
    void set_suppress_output(bool suppress);
    
    /**
     * @brief Report debug with minimal information
     * @param category Debug category
     * @param message Debug message
     * @param level Debug level
     */
    void report(DebugCategory category, const std::string& message, 
                DebugLevel level = DebugLevel::INFO);
    
    /**
     * @brief Report CPU execution debug with state context
     * @param message Debug message
     * @param pc Program counter
     * @param cycle CPU cycle count
     * @param registers_context Optional register state string
     */
    void report_cpu(const std::string& message, uint32_t pc = 0, 
                   uint32_t cycle = 0, const std::string& registers_context = "");
    
    /**
     * @brief Report memory access debug
     * @param message Debug message
     * @param address Memory address
     * @param value Value read/written
     * @param operation Operation type ("READ", "WRITE", etc.)
     */
    void report_memory(const std::string& message, uint32_t address,
                      uint32_t value = 0, const std::string& operation = "");
    
    /**
     * @brief Report instruction execution debug
     * @param instruction_name Instruction mnemonic
     * @param pc Program counter
     * @param operands_info Operands description
     * @param result_info Result description
     */
    void report_instruction(const std::string& instruction_name, uint32_t pc,
                           const std::string& operands_info = "",
                           const std::string& result_info = "");
    
    /**
     * @brief Report test framework debug
     * @param test_name Test name
     * @param message Debug message
     * @param assertion_info Optional assertion details
     */
    void report_test(const std::string& test_name, const std::string& message,
                    const std::string& assertion_info = "");
    
    // ============================================================================
    // Filtering and Configuration
    // ============================================================================
    
    /**
     * @brief Enable/disable debug category
     * @param category Category to control
     * @param enabled Whether category should be enabled
     */
    void set_category_enabled(DebugCategory category, bool enabled);
    
    /**
     * @brief Check if category is enabled
     * @param category Category to check
     * @return True if category is enabled
     */
    bool is_category_enabled(DebugCategory category) const;
    
    /**
     * @brief Enable/disable specific debug level
     * @param level Level to control
     * @param enabled Whether level should be enabled
     */
    void set_level_enabled(DebugLevel level, bool enabled);

    /**
     * @brief Enable/disable strict level filtering (only show enabled levels)
     * @param enabled Whether to use strict level filtering
     */
    void set_level_filter_mode(bool enabled);

    /**
     * @brief Check if level is enabled
     * @param level Level to check
     * @return True if level is enabled
     */
    bool is_level_enabled(DebugLevel level) const;

    /**
     * @brief Set minimum debug level for output
     * @param level Minimum level to show
     */
    void set_minimum_level(DebugLevel level);
    
    /**
     * @brief Enable frequency throttling for repeated messages
     * @param enabled Whether to throttle repeated messages
     * @param max_frequency Maximum messages per second per category
     */
    void set_frequency_throttling(bool enabled, size_t max_frequency = 100);
    
    /**
     * @brief Enable debug session recording
     * @param enabled Whether to record debug session
     * @param max_entries Maximum entries to keep in memory
     */
    void set_session_recording(bool enabled, size_t max_entries = 10000);
    
    /**
     * @brief Enable common debug categories (called when debug mode is enabled)
     */
    void enable_default_categories();
    
    // ============================================================================
    // Advanced Features
    // ============================================================================
    
    /**
     * @brief Add custom metadata to next debug message
     * @param key Metadata key
     * @param value Metadata value
     * @return Reference to this DebugHandler for chaining
     */
    DebugHandler& with_metadata(const std::string& key, const std::string& value);
    
    /**
     * @brief Set PC context for next debug message
     * @param pc Program counter value
     * @return Reference to this DebugHandler for chaining
     */
    DebugHandler& with_pc(uint32_t pc);
    
    /**
     * @brief Set cycle context for next debug message  
     * @param cycle CPU cycle count
     * @return Reference to this DebugHandler for chaining
     */
    DebugHandler& with_cycle(uint32_t cycle);
    
    /**
     * @brief Set source location for next debug message
     * @param func Function name
     * @param file File name
     * @param line Line number
     * @return Reference to this DebugHandler for chaining
     */
    DebugHandler& with_location(const std::string& func, const std::string& file, size_t line);
    
    /**
     * @brief Force next debug message to bypass all filters
     * @return Reference to this DebugHandler for chaining
     */
    DebugHandler& force();
    
    // ============================================================================
    // Utility and Information
    // ============================================================================
    
    /**
     * @brief Get human-readable category string
     * @param category Debug category
     * @return String representation like "CPU_EXECUTION (0x001)"
     */
    static std::string category_to_string(DebugCategory category);
    
    /**
     * @brief Get human-readable level string
     * @param level Debug level
     * @return String representation like "DETAIL", "IMPORTANT", etc.
     */
    static std::string level_to_string(DebugLevel level);
    
    /**
     * @brief Format debug context into a detailed message
     * @param debug Debug context
     * @return Formatted debug string ready for logging
     */
    static std::string format_debug(const DebugContext& debug);
    
    /**
     * @brief Get debug statistics for analysis
     * @return String with debug usage statistics
     */
    std::string get_statistics() const;
    
    /**
     * @brief Export recorded debug session to file
     * @param filepath Path to export file
     * @return True if export successful
     */
    bool export_session(const std::string& filepath) const;
    
    /**
     * @brief Clear all recorded debug data
     */
    void clear();
    
    /**
     * @brief Reset all settings to defaults
     */
    void reset_to_defaults();
    
    /**
     * @brief Log formatted debug to appropriate destination
     * @param debug Debug context
     */
    void log_debug(const DebugContext& debug);

private:
    DebugHandler();
    
    // State tracking
    std::unordered_map<DebugCategory, bool> enabled_categories_;
    std::unordered_map<DebugLevel, bool> enabled_levels_;
    bool level_filter_mode_;
    DebugLevel minimum_level_;
    bool frequency_throttling_enabled_;
    size_t max_frequency_per_second_;
    bool session_recording_enabled_;
    size_t max_recorded_entries_;
    bool force_next_;
    bool suppress_output_;
    
    // Context for next message
    std::unordered_map<std::string, std::string> next_metadata_;
    uint32_t next_pc_;
    uint32_t next_cycle_;
    std::string next_function_;
    std::string next_file_;
    size_t next_line_;
    
    // Statistics and throttling
    std::unordered_map<DebugCategory, size_t> message_counts_;
    std::unordered_map<DebugCategory, std::chrono::steady_clock::time_point> last_message_times_;
    std::deque<DebugContext> recorded_session_;
    mutable std::mutex debug_mutex_;
    
    /**
     * @brief Check if message should be filtered out
     * @param debug Debug context
     * @return True if message should be filtered
     */
    bool should_filter_message(const DebugContext& debug);
    
    /**
     * @brief Check frequency throttling for category
     * @param category Debug category  
     * @return True if message should be throttled
     */
    bool should_throttle_message(DebugCategory category);
    
    /**
     * @brief Clear context for next message
     */
    void clear_next_context();
};

// ============================================================================
// Convenience Macros for Easy Usage
// ============================================================================

#define DEBUG_CPU(message, ...) \
    do { \
        if (!::Config::debug) break; \
        Logging::DebugContext debug_ctx{Logging::DebugCategory::CPU_EXECUTION, fmt::format(message, ##__VA_ARGS__), Logging::DebugLevel::INFO}; \
        debug_ctx.function = __FUNCTION__; \
        debug_ctx.file = __FILE__; \
        debug_ctx.line = __LINE__; \
        Logging::DebugHandler::instance().report(debug_ctx); \
    } while(0)

#define DEBUG_INSTRUCTION(name, pc, operands, result) \
    do { \
        if (!::Config::debug) break; \
        Logging::DebugContext debug_ctx{Logging::DebugCategory::ASM_INSTRUCTION, fmt::format("{} @ PC=0x{:04X}: {} -> {}", name, pc, operands, result), Logging::DebugLevel::TRACE}; \
        debug_ctx.function = __FUNCTION__; \
        debug_ctx.file = __FILE__; \
        debug_ctx.line = __LINE__; \
        Logging::DebugHandler::instance().report(debug_ctx); \
    } while(0)

#define DEBUG_MEMORY(message, addr, value, op) \
    do { \
        if (!::Config::debug) break; \
        Logging::DebugContext debug_ctx{Logging::DebugCategory::MEM_ACCESS, fmt::format("{}: addr=0x{:08X}, value=0x{:08X}, op={}", message, addr, value, op), Logging::DebugLevel::DETAIL}; \
        debug_ctx.function = __FUNCTION__; \
        debug_ctx.file = __FILE__; \
        debug_ctx.line = __LINE__; \
        Logging::DebugHandler::instance().report(debug_ctx); \
    } while(0)

#define DEBUG_CATEGORY(cat, level, message, ...) \
    do { \
        if (!::Config::debug && level != Logging::DebugLevel::CRITICAL) break; \
        Logging::DebugContext debug_ctx{cat, fmt::format(message, ##__VA_ARGS__), level}; \
        debug_ctx.function = __FUNCTION__; \
        debug_ctx.file = __FILE__; \
        debug_ctx.line = __LINE__; \
        Logging::DebugHandler::instance().report(debug_ctx); \
    } while(0)

#define DEBUG_TRACE(cat, message, ...) \
    DEBUG_CATEGORY(cat, Logging::DebugLevel::TRACE, message, ##__VA_ARGS__)

#define DEBUG_DETAIL(cat, message, ...) \
    DEBUG_CATEGORY(cat, Logging::DebugLevel::DETAIL, message, ##__VA_ARGS__)

#define DEBUG_INFO(cat, message, ...) \
    DEBUG_CATEGORY(cat, Logging::DebugLevel::INFO, message, ##__VA_ARGS__)

#define DEBUG_IMPORTANT(cat, message, ...) \
    DEBUG_CATEGORY(cat, Logging::DebugLevel::IMPORTANT, message, ##__VA_ARGS__)

#define DEBUG_CRITICAL(cat, message, ...) \
    DEBUG_CATEGORY(cat, Logging::DebugLevel::CRITICAL, message, ##__VA_ARGS__)

#define DEBUG_FORCE(cat, level, message, ...) \
    Logging::DebugHandler::instance().with_location(__FUNCTION__, __FILE__, __LINE__) \
        .force().report(cat, fmt::format(message, ##__VA_ARGS__), level)

} // namespace Logging