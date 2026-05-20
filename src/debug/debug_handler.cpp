#include "../config.hpp"  // Include config for debug flag check
#include "debug_handler.hpp"
#include "logger.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <unistd.h>  // for isatty

namespace Logging {

DebugHandler& DebugHandler::instance() {
    static DebugHandler handler;
    return handler;
}

DebugHandler::DebugHandler() 
    : level_filter_mode_(false)
    , minimum_level_(DebugLevel::INFO)
    , frequency_throttling_enabled_(false)
    , max_frequency_per_second_(100)
    , session_recording_enabled_(false)
    , max_recorded_entries_(10000)
    , force_next_(false)
    , suppress_output_(false)
    , next_pc_(0)
    , next_cycle_(0)
    , next_line_(0) {
    
    // All categories disabled by default - only enable when Config::debug is true
    // This ensures no output appears without -d or -v flags
    
    // Verbose categories explicitly disabled
    enabled_categories_[DebugCategory::CPU_DISPATCHER] = false;
    enabled_categories_[DebugCategory::CPU_PREDICTION] = false;
    enabled_categories_[DebugCategory::MEM_BOUNDS] = false;
    enabled_categories_[DebugCategory::PERF_TIMING] = false;
}

void DebugHandler::report(const DebugContext& debug) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    
    // Check if message should be filtered
    if (should_filter_message(debug)) {
        clear_next_context();
        return;
    }
    
    // Record in session if enabled
    if (session_recording_enabled_) {
        if (recorded_session_.size() >= max_recorded_entries_) {
            recorded_session_.erase(recorded_session_.begin());
        }
        recorded_session_.push_back(debug);
    }
    
    // Update statistics
    message_counts_[debug.category]++;
    
    // Log the debug message (now using direct output to avoid deadlock)
    log_debug(debug);
    
    // Clear context for next message
    clear_next_context();
}

void DebugHandler::report(DebugCategory category, const std::string& message, DebugLevel level) {
    DebugContext debug(category, message, level);
    
    // Apply pending context
    debug.pc = next_pc_;
    debug.cycle = next_cycle_;
    debug.function = next_function_;
    debug.file = next_file_;
    debug.line = next_line_;
    debug.metadata = next_metadata_;
    
    report(debug);
}

void DebugHandler::report_cpu(const std::string& message, uint32_t pc, 
                             uint32_t cycle, const std::string& registers_context) {
    DebugContext debug(DebugCategory::CPU_EXECUTION, message);
    debug.pc = pc != 0 ? pc : next_pc_;
    debug.cycle = cycle != 0 ? cycle : next_cycle_;
    debug.context_data = registers_context;
    debug.function = next_function_;
    debug.file = next_file_;
    debug.line = next_line_;
    debug.metadata = next_metadata_;
    
    report(debug);
}

void DebugHandler::report_memory(const std::string& message, uint32_t address,
                                uint32_t value, const std::string& operation) {
    DebugContext debug(DebugCategory::MEM_ACCESS, message);
    debug.pc = next_pc_;
    debug.cycle = next_cycle_;
    debug.function = next_function_;
    debug.file = next_file_;
    debug.line = next_line_;
    debug.metadata = next_metadata_;
    
    if (!operation.empty()) {
        debug.metadata["operation"] = operation;
    }
    debug.metadata["address"] = fmt::format("0x{:08X}", address);
    debug.metadata["value"] = fmt::format("0x{:08X}", value);
    
    report(debug);
}

void DebugHandler::report_instruction(const std::string& instruction_name, uint32_t pc,
                                     const std::string& operands_info, 
                                     const std::string& result_info) {
    DebugContext debug(DebugCategory::ASM_INSTRUCTION, 
                      fmt::format("{} {}", instruction_name, operands_info));
    debug.pc = pc;
    debug.cycle = next_cycle_;
    debug.function = next_function_;
    debug.file = next_file_;
    debug.line = next_line_;
    debug.metadata = next_metadata_;
    
    if (!result_info.empty()) {
        debug.metadata["result"] = result_info;
    }
    debug.metadata["instruction"] = instruction_name;
    
    report(debug);
}

void DebugHandler::report_test(const std::string& test_name, const std::string& message,
                              const std::string& assertion_info) {
    DebugContext debug(DebugCategory::TEST_EXECUTION, message);
    debug.function = next_function_;
    debug.file = next_file_;
    debug.line = next_line_;
    debug.metadata = next_metadata_;
    debug.metadata["test_name"] = test_name;
    
    if (!assertion_info.empty()) {
        debug.metadata["assertion"] = assertion_info;
    }
    
    report(debug);
}

void DebugHandler::set_category_enabled(DebugCategory category, bool enabled) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    enabled_categories_[category] = enabled;
}

bool DebugHandler::is_category_enabled(DebugCategory category) const {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    auto it = enabled_categories_.find(category);
    return it != enabled_categories_.end() ? it->second : false;
}

void DebugHandler::set_level_enabled(DebugLevel level, bool enabled) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    enabled_levels_[level] = enabled;
}

void DebugHandler::set_level_filter_mode(bool enabled) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    level_filter_mode_ = enabled;
}

bool DebugHandler::is_level_enabled(DebugLevel level) const {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    auto it = enabled_levels_.find(level);
    return it != enabled_levels_.end() ? it->second : false;
}

void DebugHandler::set_minimum_level(DebugLevel level) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    minimum_level_ = level;
}

void DebugHandler::set_frequency_throttling(bool enabled, size_t max_frequency) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    frequency_throttling_enabled_ = enabled;
    max_frequency_per_second_ = max_frequency;
}

void DebugHandler::set_session_recording(bool enabled, size_t max_entries) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    session_recording_enabled_ = enabled;
    max_recorded_entries_ = max_entries;
    if (!enabled) {
        recorded_session_.clear();
    }
}

void DebugHandler::enable_default_categories() {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    // Enable common categories when debug mode is activated
    enabled_categories_[DebugCategory::CPU_EXECUTION] = true;
    enabled_categories_[DebugCategory::CPU_REGISTERS] = true;
    enabled_categories_[DebugCategory::ASM_INSTRUCTION] = true;
    enabled_categories_[DebugCategory::MEM_ACCESS] = true;
    enabled_categories_[DebugCategory::TEST_EXECUTION] = true;
    enabled_categories_[DebugCategory::ASM_PARSING] = true;
    enabled_categories_[DebugCategory::ASM_FORWARD_REF] = true;
}

DebugHandler& DebugHandler::with_metadata(const std::string& key, const std::string& value) {
    next_metadata_[key] = value;
    return *this;
}

DebugHandler& DebugHandler::with_pc(uint32_t pc) {
    next_pc_ = pc;
    return *this;
}

DebugHandler& DebugHandler::with_cycle(uint32_t cycle) {
    next_cycle_ = cycle;
    return *this;
}

DebugHandler& DebugHandler::with_location(const std::string& func, const std::string& file, size_t line) {
    next_function_ = func;
    next_file_ = file;
    next_line_ = line;
    return *this;
}

DebugHandler& DebugHandler::force() {
    force_next_ = true;
    return *this;
}

std::string DebugHandler::category_to_string(DebugCategory category) {
    switch (category) {
        // CPU/Engine
        case DebugCategory::CPU_EXECUTION:
            return "CPU_EXECUTION (0x001)";
        case DebugCategory::CPU_REGISTERS:
            return "CPU_REGISTERS (0x002)";
        case DebugCategory::CPU_FLAGS:
            return "CPU_FLAGS (0x003)";
        case DebugCategory::CPU_STACK:
            return "CPU_STACK (0x004)";
        case DebugCategory::CPU_JUMP:
            return "CPU_JUMP (0x005)";
        case DebugCategory::CPU_DISPATCHER:
            return "CPU_DISPATCHER (0x006)";
        case DebugCategory::CPU_PREDICTION:
            return "CPU_PREDICTION (0x007)";
        case DebugCategory::CPU_PIPELINE:
            return "CPU_PIPELINE (0x008)";
            
        // Assembly/Parser
        case DebugCategory::ASM_PARSING:
            return "ASM_PARSING (0x100)";
        case DebugCategory::ASM_INSTRUCTION:
            return "ASM_INSTRUCTION (0x101)";
        case DebugCategory::ASM_DIRECTIVE:
            return "ASM_DIRECTIVE (0x102)";
        case DebugCategory::ASM_SYMBOL:
            return "ASM_SYMBOL (0x103)";
        case DebugCategory::ASM_FORWARD_REF:
            return "ASM_FORWARD_REF (0x104)";
        case DebugCategory::ASM_ENCODING:
            return "ASM_ENCODING (0x105)";
        case DebugCategory::ASM_OPTIMIZATION:
            return "ASM_OPTIMIZATION (0x106)";
        case DebugCategory::ASM_HEXDUMP:
            return "ASM_HEXDUMP (0x107)";
            
        // Memory/Storage
        case DebugCategory::MEM_ACCESS:
            return "MEM_ACCESS (0x200)";
        case DebugCategory::MEM_ALLOCATION:
            return "MEM_ALLOCATION (0x201)";
        case DebugCategory::MEM_BOUNDS:
            return "MEM_BOUNDS (0x202)";
        case DebugCategory::MEM_VIRTUAL:
            return "MEM_VIRTUAL (0x203)";
        case DebugCategory::MEM_CACHE:
            return "MEM_CACHE (0x204)";
        case DebugCategory::MEM_PROTECTION:
            return "MEM_PROTECTION (0x205)";
            
        // I/O/Device
        case DebugCategory::IO_DEVICE:
            return "IO_DEVICE (0x300)";
        case DebugCategory::IO_FILE:
            return "IO_FILE (0x301)";
        case DebugCategory::IO_CONSOLE:
            return "IO_CONSOLE (0x302)";
        case DebugCategory::IO_RAMDISK:
            return "IO_RAMDISK (0x303)";
        case DebugCategory::IO_NETWORK:
            return "IO_NETWORK (0x304)";
        case DebugCategory::IO_INTERRUPT:
            return "IO_INTERRUPT (0x305)";
            
        // Test/Execution
        case DebugCategory::TEST_EXECUTION:
            return "TEST_EXECUTION (0x400)";
        case DebugCategory::TEST_ASSERTION:
            return "TEST_ASSERTION (0x401)";
        case DebugCategory::TEST_FRAMEWORK:
            return "TEST_FRAMEWORK (0x402)";
        case DebugCategory::TEST_VALIDATION:
            return "TEST_VALIDATION (0x403)";
        case DebugCategory::TEST_PERFORMANCE:
            return "TEST_PERFORMANCE (0x404)";
            
        // GUI/Interface
        case DebugCategory::GUI_WINDOW:
            return "GUI_WINDOW (0x500)";
        case DebugCategory::GUI_CONTROLS:
            return "GUI_CONTROLS (0x501)";
        case DebugCategory::GUI_RENDERING:
            return "GUI_RENDERING (0x502)";
        case DebugCategory::GUI_EVENTS:
            return "GUI_EVENTS (0x503)";
        case DebugCategory::GUI_STATE:
            return "GUI_STATE (0x504)";
            
        // Performance/Profiling
        case DebugCategory::PERF_TIMING:
            return "PERF_TIMING (0x600)";
        case DebugCategory::PERF_COUNTERS:
            return "PERF_COUNTERS (0x601)";
        case DebugCategory::PERF_OPTIMIZATION:
            return "PERF_OPTIMIZATION (0x602)";
        case DebugCategory::PERF_BOTTLENECKS:
            return "PERF_BOTTLENECKS (0x603)";
            
        case DebugCategory::DEBUG_GENERIC:
            return "DEBUG_GENERIC (0x999)";
            
        default:
            return "UNKNOWN_DEBUG_CATEGORY";
    }
}

std::string DebugHandler::level_to_string(DebugLevel level) {
    switch (level) {
        case DebugLevel::TRACE:
            return "TRACE";
        case DebugLevel::DETAIL:
            return "DETAIL";
        case DebugLevel::INFO:
            return "INFO";
        case DebugLevel::IMPORTANT:
            return "IMPORTANT";
        case DebugLevel::CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

std::string DebugHandler::format_debug(const DebugContext& debug) {
    std::ostringstream oss;
    
    // Header with category and level
    oss << "[" << category_to_string(debug.category) << "] [" 
        << level_to_string(debug.level) << "]";
    
    // PC and cycle information if available
    if (debug.pc != 0) {
        oss << " [PC=0x" << std::hex << debug.pc << std::dec << "]";
    }
    
    if (debug.cycle != 0) {
        oss << " [CYCLE=" << debug.cycle << "]";
    }
    
    // Source location if available
    if (!debug.function.empty()) {
        oss << " [" << debug.function;
        if (!debug.file.empty()) {
            // Extract just filename from path
            size_t pos = debug.file.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? debug.file.substr(pos + 1) : debug.file;
            oss << "@" << filename;
            if (debug.line > 0) {
                oss << ":" << debug.line;
            }
        }
        oss << "]";
    }
    
    oss << " ";
    
    // Main message
    oss << debug.message;
    
    // Context information
    if (!debug.context_data.empty()) {
        oss << "\n  Context: " << debug.context_data;
    }
    
    // Metadata
    if (!debug.metadata.empty()) {
        oss << "\n  Metadata: ";
        bool first = true;
        for (const auto& [key, value] : debug.metadata) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
    }
    
    return oss.str();
}

std::string DebugHandler::get_statistics() const {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    
    std::ostringstream oss;
    oss << "Debug Handler Statistics:\n";
    oss << "========================\n";
    oss << "Minimum Level: " << level_to_string(minimum_level_) << "\n";
    oss << "Frequency Throttling: " << (frequency_throttling_enabled_ ? "ENABLED" : "DISABLED");
    if (frequency_throttling_enabled_) {
        oss << " (max " << max_frequency_per_second_ << "/sec)";
    }
    oss << "\n";
    oss << "Session Recording: " << (session_recording_enabled_ ? "ENABLED" : "DISABLED");
    if (session_recording_enabled_) {
        oss << " (" << recorded_session_.size() << "/" << max_recorded_entries_ << " entries)";
    }
    oss << "\n\n";
    
    oss << "Message Counts by Category:\n";
    for (const auto& [category, count] : message_counts_) {
        if (count > 0) {
            oss << "  " << category_to_string(category) << ": " << count << "\n";
        }
    }
    
    oss << "\nEnabled Categories:\n";
    for (const auto& [category, enabled] : enabled_categories_) {
        if (enabled) {
            oss << "  " << category_to_string(category) << "\n";
        }
    }
    
    return oss.str();
}

bool DebugHandler::export_session(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "DemiEngine Debug Session Export\n";
        file << "==============================\n";
        file << "Total Entries: " << recorded_session_.size() << "\n\n";
        
        for (const auto& debug : recorded_session_) {
            file << format_debug(debug) << "\n\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void DebugHandler::clear() {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    message_counts_.clear();
    last_message_times_.clear();
    recorded_session_.clear();
}

void DebugHandler::reset_to_defaults() {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    
    enabled_categories_.clear();
    enabled_levels_.clear();
    level_filter_mode_ = false;
    minimum_level_ = DebugLevel::INFO;
    frequency_throttling_enabled_ = false;
    max_frequency_per_second_ = 100;
    session_recording_enabled_ = false;
    max_recorded_entries_ = 10000;
    
    // Re-enable common categories
    enabled_categories_[DebugCategory::CPU_EXECUTION] = true;
    enabled_categories_[DebugCategory::CPU_REGISTERS] = true;
    enabled_categories_[DebugCategory::ASM_INSTRUCTION] = true;
    enabled_categories_[DebugCategory::MEM_ACCESS] = true;
    enabled_categories_[DebugCategory::TEST_EXECUTION] = true;
    
    clear();
}

void DebugHandler::set_suppress_output(bool suppress) {
    std::lock_guard<std::mutex> lock(debug_mutex_);
    suppress_output_ = suppress;
}

bool DebugHandler::should_filter_message(const DebugContext& debug) {
    // Check suppression first - this overrides everything including critical messages
    if (suppress_output_) {
        return true;
    }

    // Check if debug mode is enabled first (avoid processing if not needed)
    if (!Config::debug && !force_next_ && debug.level != DebugLevel::CRITICAL) {
        return true;  // Filter out when debug is disabled
    }
    
    // Never filter forced messages or critical messages
    if (force_next_ || debug.level == DebugLevel::CRITICAL) {
        return false;
    }
    
    // When debug mode is on, check if category is explicitly disabled
    // If category is not in the map, it's enabled by default when Config::debug is true
    if (Config::debug) {
        auto cat_it = enabled_categories_.find(debug.category);
        if (cat_it != enabled_categories_.end() && !cat_it->second) {
            return true;  // Category explicitly disabled
        }
        // Otherwise, category is enabled (either not in map or set to true)
    }
    
    // Check level filtering
    if (level_filter_mode_) {
        auto lvl_it = enabled_levels_.find(debug.level);
        bool enabled = lvl_it != enabled_levels_.end() ? lvl_it->second : false;
        if (!enabled) return true;
    } else {
        // Check minimum level
        if (debug.level < minimum_level_) {
            return true;
        }
    }
    
    // Check frequency throttling
    if (frequency_throttling_enabled_ && should_throttle_message(debug.category)) {
        return true;
    }
    
    return false;
}

bool DebugHandler::should_throttle_message(DebugCategory category) {
    auto now = std::chrono::steady_clock::now();
    auto& last_time = last_message_times_[category];
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
    size_t min_interval_ms = 1000 / max_frequency_per_second_;
    
    if (duration.count() < static_cast<long>(min_interval_ms)) {
        return true;  // Throttle
    }
    
    last_time = now;
    return false;
}

void DebugHandler::log_debug(const DebugContext& debug) {
    // Note: Config::debug check is now done in should_filter_message to avoid duplicate checks
    // This function is only called after should_filter_message passes in report()
    
    // FIXED: Direct C stdio output to avoid Logger circular dependency deadlock
    
    // Category name lookup
    std::string category_name;
    switch (debug.category) {
        // CPU/Engine categories
        case DebugCategory::CPU_EXECUTION: category_name = "CPU_EXECUTION"; break;
        case DebugCategory::CPU_REGISTERS: category_name = "CPU_REGISTERS"; break;
        case DebugCategory::CPU_FLAGS: category_name = "CPU_FLAGS"; break;
        case DebugCategory::CPU_STACK: category_name = "CPU_STACK"; break;
        case DebugCategory::CPU_JUMP: category_name = "CPU_JUMP"; break;
        case DebugCategory::CPU_DISPATCHER: category_name = "CPU_DISPATCHER"; break;
        case DebugCategory::CPU_PREDICTION: category_name = "CPU_PREDICTION"; break;
        case DebugCategory::CPU_PIPELINE: category_name = "CPU_PIPELINE"; break;
        
        // Assembly/Parser categories
        case DebugCategory::ASM_PARSING: category_name = "ASM_PARSING"; break;
        case DebugCategory::ASM_INSTRUCTION: category_name = "ASM_INSTRUCTION"; break;
        case DebugCategory::ASM_DIRECTIVE: category_name = "ASM_DIRECTIVE"; break;
        case DebugCategory::ASM_SYMBOL: category_name = "ASM_SYMBOL"; break;
        case DebugCategory::ASM_FORWARD_REF: category_name = "ASM_FORWARD_REF"; break;
        case DebugCategory::ASM_ENCODING: category_name = "ASM_ENCODING"; break;
        case DebugCategory::ASM_OPTIMIZATION: category_name = "ASM_OPTIMIZATION"; break;
        case DebugCategory::ASM_HEXDUMP: category_name = "ASM_HEXDUMP"; break;
        
        // Memory/Storage categories
        case DebugCategory::MEM_ACCESS: category_name = "MEM_ACCESS"; break;
        case DebugCategory::MEM_ALLOCATION: category_name = "MEM_ALLOCATION"; break;
        case DebugCategory::MEM_BOUNDS: category_name = "MEM_BOUNDS"; break;
        case DebugCategory::MEM_VIRTUAL: category_name = "MEM_VIRTUAL"; break;
        case DebugCategory::MEM_CACHE: category_name = "MEM_CACHE"; break;
        case DebugCategory::MEM_PROTECTION: category_name = "MEM_PROTECTION"; break;
        
        // I/O/Device categories
        case DebugCategory::IO_DEVICE: category_name = "IO_DEVICE"; break;
        case DebugCategory::IO_FILE: category_name = "IO_FILE"; break;
        case DebugCategory::IO_CONSOLE: category_name = "IO_CONSOLE"; break;
        case DebugCategory::IO_RAMDISK: category_name = "IO_RAMDISK"; break;
        case DebugCategory::IO_NETWORK: category_name = "IO_NETWORK"; break;
        case DebugCategory::IO_INTERRUPT: category_name = "IO_INTERRUPT"; break;
        
        // Test/Execution categories
        case DebugCategory::TEST_EXECUTION: category_name = "TEST_EXECUTION"; break;
        case DebugCategory::TEST_ASSERTION: category_name = "TEST_ASSERTION"; break;
        case DebugCategory::TEST_FRAMEWORK: category_name = "TEST_FRAMEWORK"; break;
        case DebugCategory::TEST_VALIDATION: category_name = "TEST_VALIDATION"; break;
        case DebugCategory::TEST_PERFORMANCE: category_name = "TEST_PERFORMANCE"; break;
        
        // GUI/Interface categories  
        case DebugCategory::GUI_WINDOW: category_name = "GUI_WINDOW"; break;
        case DebugCategory::GUI_CONTROLS: category_name = "GUI_CONTROLS"; break;
        case DebugCategory::GUI_RENDERING: category_name = "GUI_RENDERING"; break;
        case DebugCategory::GUI_EVENTS: category_name = "GUI_EVENTS"; break;
        case DebugCategory::GUI_STATE: category_name = "GUI_STATE"; break;
        
        // Performance/Profiling categories
        case DebugCategory::PERF_TIMING: category_name = "PERF_TIMING"; break;
        case DebugCategory::PERF_COUNTERS: category_name = "PERF_COUNTERS"; break;
        case DebugCategory::PERF_OPTIMIZATION: category_name = "PERF_OPTIMIZATION"; break;
        case DebugCategory::PERF_BOTTLENECKS: category_name = "PERF_BOTTLENECKS"; break;
        
        // Generic
        case DebugCategory::DEBUG_GENERIC: category_name = "DEBUG_GENERIC"; break;
        
        default: category_name = "UNKNOWN"; break;
    }
    
    // Level name lookup
    std::string level_name;
    switch (debug.level) {
        case DebugLevel::TRACE: level_name = "TRACE"; break;
        case DebugLevel::DETAIL: level_name = "DETAIL"; break;
        case DebugLevel::INFO: level_name = "INFO"; break;
        case DebugLevel::IMPORTANT: level_name = "IMPORTANT"; break;
        case DebugLevel::CRITICAL: level_name = "CRITICAL"; break;
        default: level_name = "UNKNOWN"; break;
    }
    
    // Thread-safe direct output using C stdio (no Logger dependency)
    static std::mutex output_mutex;
    std::lock_guard<std::mutex> lock(output_mutex);
    
    // Extract filename from full path for cleaner output
    std::string filename = debug.file;
    size_t last_slash = filename.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        filename = filename.substr(last_slash + 1);
    }
    
    // Color-coded output based on level (if terminal supports it)
    const char* level_color = "";
    const char* reset_color = "";
    if (isatty(STDERR_FILENO)) {
        switch (debug.level) {
            case DebugLevel::TRACE: level_color = "\033[37m"; break;     // Light gray
            case DebugLevel::DETAIL: level_color = "\033[36m"; break;    // Cyan
            case DebugLevel::INFO: level_color = "\033[32m"; break;      // Green
            case DebugLevel::IMPORTANT: level_color = "\033[33m"; break; // Yellow
            case DebugLevel::CRITICAL: level_color = "\033[31m"; break;  // Red
            default: level_color = ""; break;
        }
        reset_color = "\033[0m";
    }
    
    // Improved format: [LEVEL] Category: Message (Function@File:Line)
    if (!debug.function.empty() || !filename.empty() || debug.line > 0) {
        fprintf(stderr, "%s[%s]%s %-12s: %s \033[90m(%s@%s:%zu)\033[0m\n", 
                level_color,
                level_name.c_str(),
                reset_color,
                category_name.c_str(),
                debug.message.c_str(),
                debug.function.c_str(),
                filename.c_str(),
                debug.line);
    } else {
        fprintf(stderr, "%s[%s]%s %-12s: %s\n", 
                level_color,
                level_name.c_str(),
                reset_color,
                category_name.c_str(),
                debug.message.c_str());
    }
            
    fflush(stderr);
}

void DebugHandler::clear_next_context() {
    force_next_ = false;
    next_metadata_.clear();
    next_pc_ = 0;
    next_cycle_ = 0;
    next_function_.clear();
    next_file_.clear();
    next_line_ = 0;
}

} // namespace Logging