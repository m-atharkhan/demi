/**
 * @file system_info.hpp
 * @brief System information utilities for memory detection
 * 
 * Provides cross-platform utilities for querying system resources,
 * particularly available memory, to support dynamic memory allocation.
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>

#ifdef __linux__
#include <unistd.h>
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace DemiSystem {

/**
 * @class SystemInfo
 * @brief Static utility class for querying system information
 */
class SystemInfo {
public:
    /**
     * @brief Get total physical memory on the system
     * @return Total physical memory in bytes
     */
    static size_t get_total_memory() {
#ifdef __linux__
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            return static_cast<size_t>(info.totalram) * info.mem_unit;
        }
        // Fallback: try reading /proc/meminfo
        return get_memory_from_proc();
#elif defined(__APPLE__)
        int64_t memsize;
        size_t len = sizeof(memsize);
        if (sysctlbyname("hw.memsize", &memsize, &len, nullptr, 0) == 0) {
            return static_cast<size_t>(memsize);
        }
        return 0;
#elif defined(_WIN32)
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return static_cast<size_t>(memInfo.ullTotalPhys);
        }
        return 0;
#else
        return 0; // Unknown platform
#endif
    }

    /**
     * @brief Get available (free) memory on the system
     * @return Available memory in bytes
     */
    static size_t get_available_memory() {
#ifdef __linux__
        struct sysinfo info;
        if (sysinfo(&info) == 0) {
            // Available = free + buffers + cached (approximation via freeram)
            return static_cast<size_t>(info.freeram) * info.mem_unit;
        }
        return get_available_from_proc();
#elif defined(__APPLE__)
        mach_port_t host_port = mach_host_self();
        vm_size_t page_size;
        vm_statistics64_data_t vm_stat;
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;

        if (host_page_size(host_port, &page_size) != KERN_SUCCESS) {
            return 0;
        }
        if (host_statistics64(host_port, HOST_VM_INFO64, 
                             reinterpret_cast<host_info64_t>(&vm_stat), &count) != KERN_SUCCESS) {
            return 0;
        }
        return static_cast<size_t>(vm_stat.free_count + vm_stat.inactive_count) * page_size;
#elif defined(_WIN32)
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return static_cast<size_t>(memInfo.ullAvailPhys);
        }
        return 0;
#else
        return 0; // Unknown platform
#endif
    }

    /**
     * @brief Calculate recommended maximum virtual memory for Demi programs
     * 
     * Returns a safe upper limit based on available system memory.
     * Uses at most 50% of available memory or 25% of total memory.
     * 
     * @return Recommended maximum memory in bytes
     */
    static size_t get_recommended_max_memory() {
        size_t available = get_available_memory();
        size_t total = get_total_memory();
        
        // Use the smaller of: 50% available or 25% total
        size_t from_available = available / 2;
        size_t from_total = total / 4;
        
        size_t recommended = std::min(from_available, from_total);
        
        // Apply reasonable bounds
        constexpr size_t MIN_RECOMMENDED = 1024 * 1024;       // 1MB minimum
        constexpr size_t MAX_RECOMMENDED = 4ULL * 1024 * 1024 * 1024; // 4GB maximum
        
        if (recommended < MIN_RECOMMENDED) {
            recommended = MIN_RECOMMENDED;
        }
        // recommended is an unsigned type (size_t) so it can never exceed
        // MAX_RECOMMENDED when MAX_RECOMMENDED is also size_t. This clamp
        // exists for future-proofing if the type changes — intentional.
        if (recommended > MAX_RECOMMENDED) {
            recommended = MAX_RECOMMENDED;
        }
        
        return recommended;
    }

    /**
     * @brief Calculate optimal memory size based on program size and system resources
     * 
     * This provides a smarter memory allocation that considers:
     * - The program size (bytecode + data)
     * - Available system memory
     * - A reasonable headroom for stack and heap operations
     * 
     * Formula: max(program_size * 4, 1MB) capped by system limits
     * 
     * @param program_size The size of the program bytecode in bytes
     * @return Optimal memory size in bytes
     */
    static size_t calculate_optimal_memory(size_t program_size) {
        // Base memory: at least 4x program size for stack/heap headroom
        // This allows room for:
        // - Program bytecode
        // - Stack operations
        // - Dynamic data/heap
        // - Working memory
        constexpr size_t PROGRAM_MULTIPLIER = 4;
        constexpr size_t MIN_MEMORY = 1024 * 1024;  // 1MB minimum
        
        size_t based_on_program = program_size * PROGRAM_MULTIPLIER;
        size_t optimal = std::max(based_on_program, MIN_MEMORY);
        
        // Cap by system resources
        size_t system_limit = get_recommended_max_memory();
        optimal = std::min(optimal, system_limit);
        
        // Round up to nearest page size (4KB) for efficiency
        constexpr size_t PAGE_SIZE = 4096;
        optimal = ((optimal + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
        
        return optimal;
    }

    /**
     * @brief Parse a memory size string with unit suffixes
     * 
     * Supports the following formats:
     * - Plain numbers: "1048576" (bytes)
     * - KB suffix: "1024K" or "1024KB" 
     * - MB suffix: "16M" or "16MB"
     * - GB suffix: "1G" or "1GB"
     * - Percentage: "50%" (of available memory)
     * - Special values: "auto", "system", "max"
     * 
     * @param size_str The size string to parse
     * @return Size in bytes
     * @throws std::invalid_argument if the string cannot be parsed
     */
    static size_t parse_memory_size(const std::string& size_str) {
        if (size_str.empty()) {
            throw std::invalid_argument("Empty memory size string");
        }

        // Handle special values
        std::string lower = size_str;
        for (auto& c : lower) c = static_cast<char>(std::tolower(c));
        
        if (lower == "auto" || lower == "system") {
            return get_recommended_max_memory();
        }
        if (lower == "max") {
            return get_available_memory();
        }

        // Handle percentage
        if (size_str.back() == '%') {
            std::string num_str = size_str.substr(0, size_str.length() - 1);
            double percent = std::stod(num_str);
            if (percent <= 0 || percent > 100) {
                throw std::invalid_argument("Memory percentage must be between 1 and 100");
            }
            return static_cast<size_t>(get_available_memory() * (percent / 100.0));
        }

        // Parse number with optional suffix
        size_t pos = 0;
        size_t value = 0;
        
        try {
            value = std::stoull(size_str, &pos);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid memory size: " + size_str);
        }

        if (pos == size_str.length()) {
            // Plain number (bytes)
            return value;
        }

        // Get suffix
        std::string suffix = size_str.substr(pos);
        for (auto& c : suffix) c = static_cast<char>(std::toupper(c));

        if (suffix == "K" || suffix == "KB") {
            return value * 1024;
        } else if (suffix == "M" || suffix == "MB") {
            return value * 1024 * 1024;
        } else if (suffix == "G" || suffix == "GB") {
            return value * 1024 * 1024 * 1024;
        } else {
            throw std::invalid_argument("Unknown memory size suffix: " + suffix);
        }
    }

    /**
     * @brief Format a memory size as a human-readable string
     * @param bytes Size in bytes
     * @return Human-readable string (e.g., "16 MB")
     */
    static std::string format_memory_size(size_t bytes) {
        constexpr size_t KB = 1024;
        constexpr size_t MB = 1024 * 1024;
        constexpr size_t GB = 1024 * 1024 * 1024;

        if (bytes >= GB) {
            double gb = static_cast<double>(bytes) / GB;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.2f GB", gb);
            return buf;
        } else if (bytes >= MB) {
            double mb = static_cast<double>(bytes) / MB;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.2f MB", mb);
            return buf;
        } else if (bytes >= KB) {
            double kb = static_cast<double>(bytes) / KB;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.2f KB", kb);
            return buf;
        } else {
            return std::to_string(bytes) + " bytes";
        }
    }

private:
#ifdef __linux__
    /**
     * @brief Read total memory from /proc/meminfo (Linux fallback)
     */
    static size_t get_memory_from_proc() {
        FILE* fp = fopen("/proc/meminfo", "r");
        if (!fp) return 0;
        
        char line[256];
        size_t total = 0;
        
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                unsigned long kb;
                if (sscanf(line + 9, "%lu", &kb) == 1) {
                    total = kb * 1024; // Convert KB to bytes
                }
                break;
            }
        }
        
        fclose(fp);
        return total;
    }

    /**
     * @brief Read available memory from /proc/meminfo (Linux)
     */
    static size_t get_available_from_proc() {
        FILE* fp = fopen("/proc/meminfo", "r");
        if (!fp) return 0;
        
        char line[256];
        size_t available = 0;
        
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemAvailable:", 13) == 0) {
                unsigned long kb;
                if (sscanf(line + 13, "%lu", &kb) == 1) {
                    available = kb * 1024; // Convert KB to bytes
                }
                break;
            }
        }
        
        fclose(fp);
        return available;
    }
#endif
};

} // namespace DemiSystem
