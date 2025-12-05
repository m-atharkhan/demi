#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Logging {

/**
 * @brief Standalone hexdump utility for bytecode and memory visualization
 * 
 * This class provides hexdump functionality independent of the debug system.
 * Hexdumps are always printed when requested, regardless of debug mode.
 */
class HexDumper {
public:
    /**
     * @brief Print a hexdump of bytecode to stdout
     * @param data Vector of bytes to dump
     * @param label Optional label to print before the hexdump
     * @param bytes_per_line Number of bytes to show per line (default: 16)
     */
    static void dump_bytecode(const std::vector<uint8_t>& data, 
                             const std::string& label = "",
                             size_t bytes_per_line = 16) {
        if (!label.empty()) {
            std::cout << "\n" << label << " (" << data.size() << " bytes):\n";
        }
        
        if (data.empty()) {
            std::cout << "  (empty)\n";
            return;
        }
        
        for (size_t i = 0; i < data.size(); i += bytes_per_line) {
            // Print address
            std::cout << "  0x" << std::setw(4) << std::setfill('0') 
                     << std::hex << i << ": ";
            
            // Print hex bytes
            size_t bytes_this_line = std::min(bytes_per_line, data.size() - i);
            for (size_t j = 0; j < bytes_per_line; ++j) {
                if (j < bytes_this_line) {
                    std::cout << std::setw(2) << std::setfill('0') << std::hex 
                             << static_cast<int>(data[i + j]) << " ";
                } else {
                    std::cout << "   ";
                }
            }
            
            // Print ASCII representation
            std::cout << " |";
            for (size_t j = 0; j < bytes_this_line; ++j) {
                unsigned char c = data[i + j];
                if (c >= 32 && c <= 126) {
                    std::cout << c;
                } else {
                    std::cout << '.';
                }
            }
            // Pad ASCII if line is incomplete
            for (size_t j = bytes_this_line; j < bytes_per_line; ++j) {
                std::cout << ' ';
            }
            std::cout << "|";
            
            std::cout << "\n";
        }
        std::cout << std::dec << "\n";
    }
    
    /**
     * @brief Print a hexdump of memory region to stdout
     * @param data Pointer to memory region
     * @param size Size of memory region in bytes
     * @param label Optional label to print before the hexdump
     * @param start_offset Offset to display in addresses (default: 0)
     * @param bytes_per_line Number of bytes to show per line (default: 16)
     */
    static void dump_memory(const uint8_t* data, size_t size,
                           const std::string& label = "",
                           size_t start_offset = 0,
                           size_t bytes_per_line = 16) {
        if (!label.empty()) {
            std::cout << "\n" << label << " (" << size << " bytes at 0x" 
                     << std::hex << start_offset << std::dec << "):\n";
        }
        
        if (size == 0 || data == nullptr) {
            std::cout << "  (empty)\n";
            return;
        }
        
        for (size_t i = 0; i < size; i += bytes_per_line) {
            // Print address
            std::cout << "  0x" << std::setw(4) << std::setfill('0') 
                     << std::hex << (start_offset + i) << ": ";
            
            // Print hex bytes
            size_t bytes_this_line = std::min(bytes_per_line, size - i);
            for (size_t j = 0; j < bytes_per_line; ++j) {
                if (j < bytes_this_line) {
                    std::cout << std::setw(2) << std::setfill('0') << std::hex 
                             << static_cast<int>(data[i + j]) << " ";
                } else {
                    std::cout << "   ";
                }
            }
            
            // Print ASCII representation
            std::cout << " |";
            for (size_t j = 0; j < bytes_this_line; ++j) {
                unsigned char c = data[i + j];
                if (c >= 32 && c <= 126) {
                    std::cout << c;
                } else {
                    std::cout << '.';
                }
            }
            // Pad ASCII if line is incomplete
            for (size_t j = bytes_this_line; j < bytes_per_line; ++j) {
                std::cout << ' ';
            }
            std::cout << "|";
            
            std::cout << "\n";
        }
        std::cout << std::dec << "\n";
    }
    
    /**
     * @brief Generate a formatted hexdump string with ASCII representation
     * @param data Vector of bytes to dump
     * @param bytes_per_line Number of bytes to show per line (default: 16)
     * @return String containing the formatted hexdump
     */
    static std::string format_bytecode(const std::vector<uint8_t>& data,
                                      size_t bytes_per_line = 16) {
        std::ostringstream oss;
        
        if (data.empty()) {
            return "(empty)";
        }
        
        for (size_t i = 0; i < data.size(); i += bytes_per_line) {
            if (i > 0) oss << "\n";
            
            // Print address
            oss << "  0x" << std::setw(4) << std::setfill('0') 
                << std::hex << i << ": ";
            
            // Print hex bytes
            size_t bytes_this_line = std::min(bytes_per_line, data.size() - i);
            for (size_t j = 0; j < bytes_per_line; ++j) {
                if (j < bytes_this_line) {
                    oss << std::setw(2) << std::setfill('0') << std::hex 
                        << static_cast<int>(data[i + j]) << " ";
                } else {
                    oss << "   ";
                }
            }
            
            // Print ASCII representation
            oss << " |";
            for (size_t j = 0; j < bytes_this_line; ++j) {
                unsigned char c = data[i + j];
                if (c >= 32 && c <= 126) {
                    oss << c;
                } else {
                    oss << '.';
                }
            }
            // Pad ASCII if line is incomplete
            for (size_t j = bytes_this_line; j < bytes_per_line; ++j) {
                oss << ' ';
            }
            oss << "|";
        }
        
        return oss.str();
    }

    /**
     * @brief Generate a hexdump string (without printing)
     * @param data Vector of bytes to dump
     * @param bytes_per_line Number of bytes to show per line (default: 16)
     * @return String containing the hexdump
     */
    static std::string to_string(const std::vector<uint8_t>& data,
                                size_t bytes_per_line = 16) {
        std::ostringstream oss;
        
        if (data.empty()) {
            return "(empty)";
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            if (i % bytes_per_line == 0) {
                if (i > 0) oss << "\n";
                oss << "0x" << std::setw(4) << std::setfill('0') 
                    << std::hex << i << ": ";
            }
            oss << std::setw(2) << std::setfill('0') << std::hex 
                << static_cast<int>(data[i]) << " ";
        }
        
        return oss.str();
    }
    
    /**
     * @brief Check if hexdump is enabled (for --hexdump flag)
     */
    static bool is_enabled() {
        return enabled_;
    }
    
    /**
     * @brief Enable hexdump output
     */
    static void enable() {
        enabled_ = true;
    }
    
    /**
     * @brief Disable hexdump output
     */
    static void disable() {
        enabled_ = false;
    }

private:
    static bool enabled_;
};

// Initialize static member
inline bool HexDumper::enabled_ = false;

} // namespace Logging
