#include "logger.hpp"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <regex>

#include <fmt/core.h>

#include "../config.hpp"

namespace Logging {

// ============================================================================
// Singleton Access
// ============================================================================

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

// ============================================================================
// Constructor and Destructor
// ============================================================================

Logger::Logger()
    : current_level_(LogLevel::INFO)
    , force_next_(false)
    , file_logging_enabled_(true) {

    // Initialize log file if specified in config
    if (!Config::debug_file.empty()) {
        set_log_file(Config::debug_file);
    }
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

// ============================================================================
// Core Logging Interface
// ============================================================================

Logger& Logger::level(LogLevel lvl) {
    current_level_ = lvl;
    buffer_.str(""); // Clear buffer
    buffer_.clear(); // Clear any error flags
    return *this;
}

Logger& Logger::force() {
    force_next_ = true;
    return *this;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
        log(current_level_, buffer_.str());
        buffer_.str("");
        buffer_.clear();
    }
    return *this;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);

    // Skip empty messages (trim whitespace and ANSI codes first)
    std::string trimmed_message = message;
    
    // Remove ANSI escape sequences first
    std::regex ansi_code_re("\033\\[[0-9;]*m");
    trimmed_message = std::regex_replace(trimmed_message, ansi_code_re, "");
    
    // Then trim whitespace
    trimmed_message.erase(0, trimmed_message.find_first_not_of(" \t\n\r"));
    trimmed_message.erase(trimmed_message.find_last_not_of(" \t\n\r") + 1);
    if (trimmed_message.empty()) {
        force_next_ = false; // Reset force flag
        return;
    }

    // Check if message should be filtered
    if (should_filter_message(level)) {
        force_next_ = false; // Reset force flag
        return;
    }

    // Reset force flag after checking
    force_next_ = false;

    // Generate timestamp
    std::string timestamp = generate_timestamp();

    // Format the complete log line
    std::string formatted_message = format_log_line(level, message, timestamp);

    // Output to console
    write_to_console(level, formatted_message);

    // Write to file if enabled
    if (file_logging_enabled_) {
        write_to_file(formatted_message);
    }
}

// ============================================================================
// Convenience Methods
// ============================================================================

Logger& Logger::success() {
    return level(LogLevel::SUCCESS);
}

Logger& Logger::info() {
    return level(LogLevel::INFO);
}

Logger& Logger::warn() {
    return level(LogLevel::WARNING);
}

Logger& Logger::error(const std::string& extra_info) {
    Config::error_count++;

    if (!extra_info.empty()) {
        buffer_ << " (" << extra_info << ")";
    }
    return level(LogLevel::ERROR);
}

Logger& Logger::debug() {
    return level(LogLevel::DEBUG);
}

Logger& Logger::running() {
    return level(LogLevel::RUNNING);
}

Logger& Logger::demiengine() {
    return level(LogLevel::ENGINE);
}

// ============================================================================
// File Logging Control
// ============================================================================

void Logger::set_file_logging_enabled(bool enabled) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);
    file_logging_enabled_ = enabled;
}

bool Logger::is_file_logging_enabled() const {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);
    return file_logging_enabled_;
}

bool Logger::set_log_file(const std::string& file_path) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);

    // Close existing file if open
    if (log_file_.is_open()) {
        log_file_.close();
    }

    // Try to open new file
    log_file_.open(file_path, std::ios::out | std::ios::trunc);

    if (log_file_.is_open()) {
        // Write header to log file
        log_file_ << "=== DemiEngine Log Session Started ===" << std::endl;
        log_file_ << "Timestamp: " << generate_timestamp() << std::endl;
        log_file_ << "========================================" << std::endl;
        log_file_.flush();
        return true;
    }

    return false;
}

// ============================================================================
// Helper Methods
// ============================================================================

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::SUCCESS:     return "SUCCESS";
        case LogLevel::INFO:        return "INFO";
        case LogLevel::WARNING:     return "WARNING";
        case LogLevel::ERROR:       return "ERROR";
        case LogLevel::DEBUG:       return "DEBUG";
        case LogLevel::RUNNING:     return "RUNNING";
        case LogLevel::ENGINE:      return "ENGINE";
        case LogLevel::ERRORINFO:   return "ERRORINFO";
        default:                    return "UNKNOWN";
    }
}

std::string Logger::level_to_color(LogLevel level) const {
    switch (level) {
        case LogLevel::SUCCESS:     return "\033[1;32m";     // Bright Green
        case LogLevel::INFO:        return "\033[1;36m";     // Bright Cyan
        case LogLevel::WARNING:     return "\033[1;33m";     // Bright Yellow
        case LogLevel::ERROR:       return "\033[1;31m";     // Bright Red
        case LogLevel::ERRORINFO:   return "\033[1;36m";     // Bright Cyan
        case LogLevel::DEBUG:       return "\033[38;5;208m"; // Orange (ANSI 256-color)
        case LogLevel::RUNNING:     return "\033[1;34m";     // Bright Blue
        case LogLevel::ENGINE:      return "\033[1;35m";     // Bright Magenta
        default:                    return RESET_COLOR;
    }
}

std::string Logger::generate_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    // Create timestamp with milliseconds
    char datetime[DATETIME_BUFFER_SIZE];
    size_t result = std::strftime(datetime, DATETIME_BUFFER_SIZE,
                                  "%y-%m-%d %H:%M:%S", &tm);

    if (result == 0) {
        // strftime failed, use fallback
        std::strcpy(datetime, "00-00-00 00:00:00");
    }

    std::ostringstream datetime_with_ms;
    datetime_with_ms << datetime << "."
                     << std::setfill('0') << std::setw(3) << ms.count();

    return datetime_with_ms.str();
}

std::string Logger::format_log_line(LogLevel level, const std::string& message,
                                   const std::string& timestamp) const {
    std::string level_str = level_to_string(level);

    // Special formatting for WARNING messages
    if (level == LogLevel::WARNING) {
        return fmt::format("[{}] [{}] {}",
                          timestamp, level_str, message);
    } else {
        return fmt::format("[{}] [{}] {}",
                          timestamp, level_str, message);
    }
}

bool Logger::should_filter_message(LogLevel level) const {
    // If force flag is set, never filter
    if (force_next_) {
        return false;
    }

    // In quiet mode, filter everything except SUCCESS
    if (Config::quiet) {
        return level != LogLevel::SUCCESS;
    }

    // Never filter ERROR, SUCCESS, WARNING, RUNNING, ENGINE, or ERRORINFO
    switch (level) {
        case LogLevel::ERROR:
        case LogLevel::SUCCESS:
        case LogLevel::WARNING:
        case LogLevel::RUNNING:
        case LogLevel::ENGINE:
        case LogLevel::ERRORINFO:
            return false;

        case LogLevel::DEBUG:
            return !Config::debug;

        case LogLevel::INFO:
            return !Config::verbose;

        default:
            return false;
    }
}

void Logger::write_to_console(LogLevel level, const std::string& formatted_message) const {
    // Determine output stream
    std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;

    // Check if we need to add a newline before the log message
    // This happens when console output was written without ending with a newline
    if (console_needs_newline_) {
        out << std::endl;
        console_needs_newline_ = false;
    }

    // Find the level portion in the formatted message and colorize only that part
    std::string level_color = level_to_color(level);
    std::string level_str = level_to_string(level);
    std::string purple_color = "\033[1;35m"; // Bright Magenta/Purple

    // Look for the pattern [LEVEL] in the formatted message
    std::string level_pattern = "[" + level_str + "]";
    size_t level_pos = formatted_message.find(level_pattern);

    if (level_pos != std::string::npos) {
        // Split the message into parts: before level, level, after level
        std::string before_level = formatted_message.substr(0, level_pos);
        std::string after_level = formatted_message.substr(level_pos + level_pattern.length());

        // Look for timestamp pattern [YY-MM-DD HH:MM:SS.mmm] at the beginning
        size_t timestamp_end = before_level.find(']');
        if (timestamp_end != std::string::npos && before_level[0] == '[') {
            std::string timestamp_part = before_level.substr(0, timestamp_end + 1);
            std::string remaining_before = before_level.substr(timestamp_end + 1);

            // Output with purple timestamp, colored level, and normal message
            out << purple_color << timestamp_part << RESET_COLOR
                << remaining_before << level_color << level_pattern << RESET_COLOR
                << after_level << std::endl;
        } else {
            // Fallback: just color the level if no timestamp found
            out << before_level << level_color << level_pattern << RESET_COLOR << after_level << std::endl;
        }
    } else {
        // Fallback: output without color if we can't find the level pattern
        out << formatted_message << std::endl;
    }
}

std::string Logger::strip_ansi_codes(const std::string& input) const {
    std::string result;
    result.reserve(input.length());
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        if (c == '\033' && i + 1 < input.length() && input[i + 1] == '[') {
            // Start of ANSI escape sequence \033[
            i += 2; // Skip \033[
            
            // Continue skipping until we find a terminating character
            while (i < input.length()) {
                char term_char = input[i];
                // Common ANSI terminating characters
                if (term_char == 'm' || term_char == 'K' || term_char == 'J' || 
                    term_char == 'H' || term_char == 'A' || term_char == 'B' || 
                    term_char == 'C' || term_char == 'D' || term_char == 'f' || 
                    term_char == 'G' || term_char == 'S' || term_char == 'T') {
                    break;
                }
                i++;
            }
            // i now points to the terminating character, which will be skipped by the main loop increment
        } else {
            // Regular character, add to result
            result += c;
        }
    }
    
    return result;
}

void Logger::write_to_file(const std::string& formatted_message) {
    if (!log_file_.is_open()) {
        return;
    }
    
    try {
        std::string clean_message = strip_ansi_codes(formatted_message);
        log_file_ << clean_message << std::endl;
    } catch (const std::exception& e) {
        // If file writing fails, at least output to console
        std::cerr << "Error writing to log file: " << e.what() << std::endl;
    }
}

// Initialize static member
bool Logger::console_needs_newline_ = false;

void Logger::set_console_needs_newline(bool needs_newline) {
    console_needs_newline_ = needs_newline;
}

bool Logger::get_console_needs_newline() {
    return console_needs_newline_;
}

} // namespace Logging
