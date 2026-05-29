#include "debug_config.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Logging {

void DebugConfig::apply_preset(Preset preset) {
    auto& handler = DebugHandler::instance();
    
    // Reset to defaults first
    handler.reset_to_defaults();
    
    switch (preset) {
        case Preset::SILENT:
            // Only critical messages
            handler.set_minimum_level(DebugLevel::CRITICAL);
            // Disable most categories
            for (auto& [cat, name] : get_all_categories()) {
                handler.set_category_enabled(cat, false);
            }
            break;
            
        case Preset::MINIMAL:
            // Only important and critical messages
            handler.set_minimum_level(DebugLevel::IMPORTANT);
            handler.set_category_enabled(DebugCategory::CPU_EXECUTION, true);
            handler.set_category_enabled(DebugCategory::TEST_EXECUTION, true);
            handler.set_category_enabled(DebugCategory::ASM_PARSING, true);
            break;
            
        case Preset::STANDARD:
            // Default configuration (already set by reset_to_defaults)
            handler.set_minimum_level(DebugLevel::INFO);
            break;
            
        case Preset::DETAILED:
            handler.set_minimum_level(DebugLevel::DETAIL);
            handler.set_category_enabled(DebugCategory::CPU_REGISTERS, true);
            handler.set_category_enabled(DebugCategory::CPU_FLAGS, true);
            handler.set_category_enabled(DebugCategory::MEM_ACCESS, true);
            handler.set_category_enabled(DebugCategory::ASM_INSTRUCTION, true);
            handler.set_category_enabled(DebugCategory::ASM_SYMBOL, true);
            break;
            
        case Preset::VERBOSE:
            handler.set_minimum_level(DebugLevel::TRACE);
            // Enable most debugging categories
            enable_categories_by_pattern("CPU_*");
            enable_categories_by_pattern("ASM_*");
            enable_categories_by_pattern("MEM_*");
            enable_categories_by_pattern("TEST_*");
            break;
            
        case Preset::EVERYTHING:
            handler.set_minimum_level(DebugLevel::TRACE);
            // Enable all categories
            for (auto& [cat, name] : get_all_categories()) {
                handler.set_category_enabled(cat, true);
            }
            break;
    }
}

void DebugConfig::configure_for_cpu_debug(bool detailed) {
    auto& handler = DebugHandler::instance();
    
    handler.set_category_enabled(DebugCategory::CPU_EXECUTION, true);
    handler.set_category_enabled(DebugCategory::CPU_JUMP, true);
    handler.set_category_enabled(DebugCategory::CPU_STACK, true);
    
    if (detailed) {
        handler.set_minimum_level(DebugLevel::DETAIL);
        handler.set_category_enabled(DebugCategory::CPU_REGISTERS, true);
        handler.set_category_enabled(DebugCategory::CPU_FLAGS, true);
        handler.set_category_enabled(DebugCategory::CPU_DISPATCHER, true);
    } else {
        handler.set_minimum_level(DebugLevel::INFO);
    }
}

void DebugConfig::configure_for_assembly_debug(bool detailed) {
    auto& handler = DebugHandler::instance();
    
    handler.set_category_enabled(DebugCategory::ASM_PARSING, true);
    handler.set_category_enabled(DebugCategory::ASM_INSTRUCTION, true);
    
    if (detailed) {
        handler.set_minimum_level(DebugLevel::DETAIL);
        handler.set_category_enabled(DebugCategory::ASM_DIRECTIVE, true);
        handler.set_category_enabled(DebugCategory::ASM_SYMBOL, true);
        handler.set_category_enabled(DebugCategory::ASM_FORWARD_REF, true);
        handler.set_category_enabled(DebugCategory::ASM_ENCODING, true);
    } else {
        handler.set_minimum_level(DebugLevel::INFO);
    }
}

void DebugConfig::configure_for_test_debug(bool detailed) {
    auto& handler = DebugHandler::instance();
    
    handler.set_category_enabled(DebugCategory::TEST_EXECUTION, true);
    handler.set_category_enabled(DebugCategory::TEST_ASSERTION, true);
    
    if (detailed) {
        handler.set_minimum_level(DebugLevel::DETAIL);
        handler.set_category_enabled(DebugCategory::TEST_FRAMEWORK, true);
        handler.set_category_enabled(DebugCategory::TEST_VALIDATION, true);
        handler.set_category_enabled(DebugCategory::TEST_PERFORMANCE, true);
    } else {
        handler.set_minimum_level(DebugLevel::INFO);
    }
}

void DebugConfig::configure_for_memory_debug(bool detailed) {
    auto& handler = DebugHandler::instance();
    
    handler.set_category_enabled(DebugCategory::MEM_ACCESS, true);
    
    if (detailed) {
        handler.set_minimum_level(DebugLevel::DETAIL);
        handler.set_category_enabled(DebugCategory::MEM_ALLOCATION, true);
        handler.set_category_enabled(DebugCategory::MEM_BOUNDS, true);
        handler.set_category_enabled(DebugCategory::MEM_VIRTUAL, true);
        handler.set_category_enabled(DebugCategory::MEM_CACHE, true);
        handler.set_category_enabled(DebugCategory::MEM_PROTECTION, true);
    } else {
        handler.set_minimum_level(DebugLevel::INFO);
    }
}

void DebugConfig::configure_for_performance_debug(bool detailed) {
    auto& handler = DebugHandler::instance();
    
    handler.set_category_enabled(DebugCategory::PERF_TIMING, true);
    
    if (detailed) {
        handler.set_minimum_level(DebugLevel::DETAIL);
        handler.set_category_enabled(DebugCategory::PERF_COUNTERS, true);
        handler.set_category_enabled(DebugCategory::PERF_OPTIMIZATION, true);
        handler.set_category_enabled(DebugCategory::PERF_BOTTLENECKS, true);
        
        // Enable frequency throttling for performance monitoring
        handler.set_frequency_throttling(true, 50);  // 50 messages per second
    } else {
        handler.set_minimum_level(DebugLevel::INFO);
    }
}

bool DebugConfig::load_from_file(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto& handler = DebugHandler::instance();
        std::string line;
        
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            std::istringstream iss(line);
            std::string key, value;
            
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (key == "minimum_level") {
                    if (value == "TRACE") handler.set_minimum_level(DebugLevel::TRACE);
                    else if (value == "DETAIL") handler.set_minimum_level(DebugLevel::DETAIL);
                    else if (value == "INFO") handler.set_minimum_level(DebugLevel::INFO);
                    else if (value == "IMPORTANT") handler.set_minimum_level(DebugLevel::IMPORTANT);
                    else if (value == "CRITICAL") handler.set_minimum_level(DebugLevel::CRITICAL);
                } else if (key.find("category_") == 0) {
                    std::string category_name = key.substr(9); // Remove "category_" prefix
                    bool enabled = (value == "true" || value == "1" || value == "yes");
                    
                    // Try to match category name to enum
                    for (auto& [cat, name] : get_all_categories()) {
                        if (name.find(category_name) != std::string::npos) {
                            handler.set_category_enabled(cat, enabled);
                            break;
                        }
                    }
                }
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool DebugConfig::save_to_file(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto& handler = DebugHandler::instance();
        
        file << "# DemiEngine Debug Configuration\n";
        file << "# Generated automatically\n\n";
        
        file << "# Minimum debug level (TRACE, DETAIL, INFO, IMPORTANT, CRITICAL)\n";
        file << "minimum_level=INFO\n\n";  // Default, since we can't query current level
        
        file << "# Debug categories (true/false)\n";
        for (auto& [cat, name] : get_all_categories()) {
            bool enabled = handler.is_category_enabled(cat);
            file << "category_" << name << "=" << (enabled ? "true" : "false") << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string DebugConfig::get_current_config_description() {
    auto& handler = DebugHandler::instance();
    std::ostringstream oss;
    
    oss << "Current Debug Configuration:\n";
    oss << "===========================\n";
    
    oss << "\nEnabled Categories:\n";
    int enabled_count = 0;
    for (auto& [cat, name] : get_all_categories()) {
        if (handler.is_category_enabled(cat)) {
            oss << "  ✓ " << name << "\n";
            enabled_count++;
        }
    }
    
    oss << "\nTotal enabled categories: " << enabled_count << " / " << get_all_categories().size() << "\n";
    oss << "\nUse DebugConfig::apply_preset() to quickly change configuration.\n";
    oss << "Available presets: SILENT, MINIMAL, STANDARD, DETAILED, VERBOSE, EVERYTHING\n";
    
    return oss.str();
}

void DebugConfig::enable_categories_by_pattern(const std::string& pattern) {
    auto& handler = DebugHandler::instance();
    
    for (auto& [cat, name] : get_all_categories()) {
        if (matches_pattern(name, pattern)) {
            handler.set_category_enabled(cat, true);
        }
    }
}

void DebugConfig::disable_categories_by_pattern(const std::string& pattern) {
    auto& handler = DebugHandler::instance();
    
    for (auto& [cat, name] : get_all_categories()) {
        if (matches_pattern(name, pattern)) {
            handler.set_category_enabled(cat, false);
        }
    }
}

bool DebugConfig::matches_pattern(const std::string& category_name, const std::string& pattern) {
    // Simple pattern matching with wildcards
    if (pattern.find('*') == std::string::npos) {
        return category_name == pattern;
    }
    
    // Handle prefix pattern like "CPU_*"
    if (pattern.back() == '*') {
        std::string prefix = pattern.substr(0, pattern.length() - 1);
        return category_name.substr(0, prefix.length()) == prefix;
    }
    
    // Handle suffix pattern like "*_EXECUTION"
    if (pattern.front() == '*') {
        std::string suffix = pattern.substr(1);
        if (category_name.length() >= suffix.length()) {
            return category_name.substr(category_name.length() - suffix.length()) == suffix;
        }
    }
    
    return false;
}

const std::unordered_map<DebugCategory, std::string>& DebugConfig::get_all_categories() {
    static const std::unordered_map<DebugCategory, std::string> categories = {
        // CPU/Engine
        {DebugCategory::CPU_EXECUTION, "CPU_EXECUTION"},
        {DebugCategory::CPU_REGISTERS, "CPU_REGISTERS"},
        {DebugCategory::CPU_FLAGS, "CPU_FLAGS"},
        {DebugCategory::CPU_STACK, "CPU_STACK"},
        {DebugCategory::CPU_JUMP, "CPU_JUMP"},
        {DebugCategory::CPU_DISPATCHER, "CPU_DISPATCHER"},
        {DebugCategory::CPU_PREDICTION, "CPU_PREDICTION"},
        {DebugCategory::CPU_PIPELINE, "CPU_PIPELINE"},
        
        // Assembly/Parser
        {DebugCategory::ASM_PARSING, "ASM_PARSING"},
        {DebugCategory::ASM_INSTRUCTION, "ASM_INSTRUCTION"},
        {DebugCategory::ASM_DIRECTIVE, "ASM_DIRECTIVE"},
        {DebugCategory::ASM_SYMBOL, "ASM_SYMBOL"},
        {DebugCategory::ASM_FORWARD_REF, "ASM_FORWARD_REF"},
        {DebugCategory::ASM_ENCODING, "ASM_ENCODING"},
        {DebugCategory::ASM_OPTIMIZATION, "ASM_OPTIMIZATION"},
        
        // Memory/Storage
        {DebugCategory::MEM_ACCESS, "MEM_ACCESS"},
        {DebugCategory::MEM_ALLOCATION, "MEM_ALLOCATION"},
        {DebugCategory::MEM_BOUNDS, "MEM_BOUNDS"},
        {DebugCategory::MEM_VIRTUAL, "MEM_VIRTUAL"},
        {DebugCategory::MEM_CACHE, "MEM_CACHE"},
        {DebugCategory::MEM_PROTECTION, "MEM_PROTECTION"},
        
        // I/O/Device
        {DebugCategory::IO_DEVICE, "IO_DEVICE"},
        {DebugCategory::IO_FILE, "IO_FILE"},
        {DebugCategory::IO_CONSOLE, "IO_CONSOLE"},
        {DebugCategory::IO_RAMDISK, "IO_RAMDISK"},
        {DebugCategory::IO_NETWORK, "IO_NETWORK"},
        {DebugCategory::IO_INTERRUPT, "IO_INTERRUPT"},
        
        // Test/Execution
        {DebugCategory::TEST_EXECUTION, "TEST_EXECUTION"},
        {DebugCategory::TEST_ASSERTION, "TEST_ASSERTION"},
        {DebugCategory::TEST_FRAMEWORK, "TEST_FRAMEWORK"},
        {DebugCategory::TEST_VALIDATION, "TEST_VALIDATION"},
        {DebugCategory::TEST_PERFORMANCE, "TEST_PERFORMANCE"},
        
        // GUI/Interface
        {DebugCategory::GUI_WINDOW, "GUI_WINDOW"},
        {DebugCategory::GUI_CONTROLS, "GUI_CONTROLS"},
        {DebugCategory::GUI_RENDERING, "GUI_RENDERING"},
        {DebugCategory::GUI_EVENTS, "GUI_EVENTS"},
        {DebugCategory::GUI_STATE, "GUI_STATE"},
        
        // Performance/Profiling
        {DebugCategory::PERF_TIMING, "PERF_TIMING"},
        {DebugCategory::PERF_COUNTERS, "PERF_COUNTERS"},
        {DebugCategory::PERF_OPTIMIZATION, "PERF_OPTIMIZATION"},
        {DebugCategory::PERF_BOTTLENECKS, "PERF_BOTTLENECKS"},
        
        {DebugCategory::DEBUG_GENERIC, "DEBUG_GENERIC"}
    };
    return categories;
}

} // namespace Logging