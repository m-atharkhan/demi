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
        case DebugCategory::CPU_EXECUTION:   return "CPU_EXECUTION";
        case DebugCategory::CPU_REGISTERS:   return "CPU_REGISTERS";
        case DebugCategory::CPU_FLAGS:       return "CPU_FLAGS";
        case DebugCategory::CPU_STACK:       return "CPU_STACK";
        case DebugCategory::CPU_JUMP:        return "CPU_JUMP";
        case DebugCategory::CPU_DISPATCHER:  return "CPU_DISPATCHER";
        case DebugCategory::CPU_PREDICTION:  return "CPU_PREDICTION";
        case DebugCategory::CPU_PIPELINE:    return "CPU_PIPELINE";
        case DebugCategory::ASM_PARSING:     return "ASM_PARSING";
        case DebugCategory::ASM_INSTRUCTION: return "ASM_INSTRUCTION";
        case DebugCategory::ASM_DIRECTIVE:   return "ASM_DIRECTIVE";
        case DebugCategory::ASM_SYMBOL:      return "ASM_SYMBOL";
        case DebugCategory::ASM_FORWARD_REF: return "ASM_FORWARD_REF";
        case DebugCategory::ASM_ENCODING:    return "ASM_ENCODING";
        case DebugCategory::ASM_OPTIMIZATION:return "ASM_OPTIMIZATION";
        case DebugCategory::ASM_HEXDUMP:     return "ASM_HEXDUMP";
        case DebugCategory::MEM_ACCESS:      return "MEM_ACCESS";
        case DebugCategory::MEM_ALLOCATION:  return "MEM_ALLOCATION";
        case DebugCategory::MEM_BOUNDS:      return "MEM_BOUNDS";
        case DebugCategory::MEM_VIRTUAL:     return "MEM_VIRTUAL";
        case DebugCategory::MEM_CACHE:       return "MEM_CACHE";
        case DebugCategory::MEM_PROTECTION:  return "MEM_PROTECTION";
        case DebugCategory::IO_DEVICE:       return "IO_DEVICE";
        case DebugCategory::IO_FILE:         return "IO_FILE";
        case DebugCategory::IO_CONSOLE:      return "IO_CONSOLE";
        case DebugCategory::IO_RAMDISK:      return "IO_RAMDISK";
        case DebugCategory::IO_NETWORK:      return "IO_NETWORK";
        case DebugCategory::IO_INTERRUPT:    return "IO_INTERRUPT";
        case DebugCategory::TEST_EXECUTION:  return "TEST_EXECUTION";
        case DebugCategory::TEST_ASSERTION:  return "TEST_ASSERTION";
        case DebugCategory::TEST_FRAMEWORK:  return "TEST_FRAMEWORK";
        case DebugCategory::TEST_VALIDATION: return "TEST_VALIDATION";
        case DebugCategory::TEST_PERFORMANCE:return "TEST_PERFORMANCE";
        case DebugCategory::GUI_WINDOW:      return "GUI_WINDOW";
        case DebugCategory::GUI_CONTROLS:    return "GUI_CONTROLS";
        case DebugCategory::GUI_RENDERING:   return "GUI_RENDERING";
        case DebugCategory::GUI_EVENTS:      return "GUI_EVENTS";
        case DebugCategory::GUI_STATE:       return "GUI_STATE";
        case DebugCategory::PERF_TIMING:     return "PERF_TIMING";
        case DebugCategory::PERF_COUNTERS:   return "PERF_COUNTERS";
        case DebugCategory::PERF_OPTIMIZATION:return "PERF_OPTIMIZATION";
        case DebugCategory::PERF_BOTTLENECKS:return "PERF_BOTTLENECKS";
        case DebugCategory::DEBUG_GENERIC:   return "DEBUG_GENERIC";
        default:                             return "UNKNOWN_DEBUG_CATEGORY";
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
    
    std::string category_name = category_to_string(debug.category);
    
    std::string level_name = level_to_string(debug.level);
    
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