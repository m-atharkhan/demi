#pragma once

#include "debug_handler.hpp"
#include <string>
#include <unordered_map>

namespace Logging {

/**
 * @brief Configuration utility for the debug handler
 * 
 * Provides easy configuration presets and utilities for managing debug output
 */
class DebugConfig {
public:
    /**
     * @brief Predefined debug configuration presets
     */
    enum class Preset {
        SILENT,         ///< Only critical messages
        MINIMAL,        ///< Only important messages
        STANDARD,       ///< Default configuration  
        DETAILED,       ///< More verbose output
        VERBOSE,        ///< Very detailed debugging
        EVERYTHING      ///< All debug categories enabled
    };

    /**
     * @brief Apply a debug configuration preset
     * @param preset The preset to apply
     */
    static void apply_preset(Preset preset);
    
    /**
     * @brief Configure debug handler for CPU debugging
     * @param detailed Whether to include detailed register/memory info
     */
    static void configure_for_cpu_debug(bool detailed = true);
    
    /**
     * @brief Configure debug handler for assembly/parsing debugging
     * @param detailed Whether to include detailed parsing info
     */
    static void configure_for_assembly_debug(bool detailed = true);
    
    /**
     * @brief Configure debug handler for test debugging
     * @param detailed Whether to include detailed test execution info
     */
    static void configure_for_test_debug(bool detailed = true);
    
    /**
     * @brief Configure debug handler for memory debugging
     * @param detailed Whether to include detailed memory access info
     */
    static void configure_for_memory_debug(bool detailed = true);
    
    /**
     * @brief Configure debug handler for performance debugging
     * @param detailed Whether to include detailed performance metrics
     */
    static void configure_for_performance_debug(bool detailed = true);
    
    /**
     * @brief Load debug configuration from file
     * @param filepath Path to configuration file
     * @return True if loaded successfully
     */
    static bool load_from_file(const std::string& filepath);
    
    /**
     * @brief Save current debug configuration to file
     * @param filepath Path to save configuration
     * @return True if saved successfully
     */
    static bool save_to_file(const std::string& filepath);
    
    /**
     * @brief Get human-readable description of current configuration
     * @return String describing current debug settings
     */
    static std::string get_current_config_description();
    
    /**
     * @brief Enable debug categories by pattern
     * @param pattern Category pattern (e.g., "CPU_*", "ASM_*", "*_EXECUTION")
     */
    static void enable_categories_by_pattern(const std::string& pattern);
    
    /**
     * @brief Disable debug categories by pattern
     * @param pattern Category pattern (e.g., "CPU_*", "ASM_*", "*_EXECUTION")
     */
    static void disable_categories_by_pattern(const std::string& pattern);
    
private:
    /**
     * @brief Check if category name matches pattern
     * @param category_name Name of the category
     * @param pattern Pattern to match against
     * @return True if category matches pattern
     */
    static bool matches_pattern(const std::string& category_name, const std::string& pattern);
    
    /**
     * @brief Get all debug categories as strings
     * @return Map of category enum to string representation
     */
    static std::unordered_map<DebugCategory, std::string> get_all_categories();
};

} // namespace Logging