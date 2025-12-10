#pragma once

#include "../device.hpp"
#include "../../debug/debug_handler.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdexcept>
#include <algorithm>

namespace fs = std::filesystem;
namespace vhw {

/**
 * A virtual device that reads/writes to a file
 * Reading will get a byte from the file
 * Writing will append a byte to the file
 */
class FileDevice : public VirtualDevice {
public:
    static constexpr uint8_t DEFAULT_PORT = 0x04;

    explicit FileDevice(const std::string& filepath)
        : filepath(filepath), position(0) {
        // Validate file path for security before proceeding
        if (!validateFilePath(filepath)) {
            throw std::runtime_error("Invalid or unsafe file path: " + filepath);
        }

        // Try to open the file for reading
        loadFromFile();
    }

    ~FileDevice() override = default;

    uint8_t read() override {
        std::lock_guard<std::mutex> lock(mutex);

        // If position is at the end of the buffer or beyond, return 0
        if (position >= fileBuffer.size()) {
            return 0;
        }

        // Additional safety check: ensure buffer is not empty
        if (fileBuffer.empty()) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, "File device buffer is empty, cannot read", Logging::DebugLevel::IMPORTANT);
            return 0;
        }

        // Validate position before incrementing to prevent overflow
        if (position == SIZE_MAX) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, "File device position overflow at maximum value, resetting to 0", Logging::DebugLevel::IMPORTANT);
            position = 0;
            return 0;
        }

        // Double-check bounds before accessing (defense in depth)
        if (position >= fileBuffer.size()) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File device position {} is beyond buffer size {}, returning 0", position, fileBuffer.size()), Logging::DebugLevel::IMPORTANT);
            return 0;
        }

        // Return byte at current position and advance
        uint8_t value = fileBuffer[position];
        ++position;
        return value;
    }

    void write(uint8_t value) override {
        std::lock_guard<std::mutex> lock(mutex);

        // Check for position overflow before any operations
        if (position == SIZE_MAX) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, "File device position overflow at maximum value, cannot write", Logging::DebugLevel::CRITICAL);
            return;
        }

        // If writing at the current position or beyond the end, append
        if (position >= fileBuffer.size()) {
            // Check for reasonable buffer size limits to prevent memory exhaustion
            constexpr size_t MAX_BUFFER_SIZE = 100 * 1024 * 1024; // 100MB limit
            if (fileBuffer.size() >= MAX_BUFFER_SIZE) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format(
                    "File device buffer size limit reached ({}), cannot append",
                    MAX_BUFFER_SIZE
                ), Logging::DebugLevel::CRITICAL);
                return;
            }

            fileBuffer.push_back(value);
            position = fileBuffer.size();
        } else {
            // Double-check bounds before accessing (defense in depth)
            if (position >= fileBuffer.size()) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File device position {} is beyond buffer size {}, cannot write", position, fileBuffer.size()), Logging::DebugLevel::CRITICAL);
                return;
            }

            // Update existing byte and advance position safely
            fileBuffer[position] = value;
            ++position;
        }

        // Save to file
        saveToFile();
    }

    std::string getName() const override {
        return fmt::format("File Device ({})", filepath);
    }

    void reset() override {
        std::lock_guard<std::mutex> lock(mutex);
        position = 0;
        loadFromFile();
    }

    /**
     * Set the file position
     */
    void seek(size_t newPosition) {
        std::lock_guard<std::mutex> lock(mutex);
        position = std::min(newPosition, fileBuffer.size());
    }

    /**
     * Get the current file position
     */
    size_t tell() const {
        return position;
    }

    /**
     * Get the file size
     */
    size_t size() const {
        return fileBuffer.size();
    }

private:
    /**
     * Validate file path for security (prevents symlink attacks, path traversal, etc.)
     */
    bool validateFilePath(const std::string& path) {
        // Check if path is empty or too long
        if (path.empty() || path.length() >= PATH_MAX) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path is empty or too long: '{}'", path), Logging::DebugLevel::CRITICAL);
            return false;
        }

        // Check for path traversal attempts
        if (path.find("..") != std::string::npos) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path contains path traversal: '{}'", path), Logging::DebugLevel::CRITICAL);
            return false;
        }

        // Check for absolute paths to sensitive directories
        const std::vector<std::string> forbiddenPaths = {
            "/etc/",
            "/proc/",
            "/sys/",
            "/dev/",
            "/boot/",
            "/root/",
            "/var/log/"
        };

        for (const auto& forbidden : forbiddenPaths) {
            if (path.find(forbidden) == 0) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path accesses forbidden directory: '{}'", path), Logging::DebugLevel::CRITICAL);
                return false;
            }
        }

        // Get the directory path and check if it exists and is safe
        fs::path fullPath = fs::absolute(path);
        fs::path parentPath = fullPath.parent_path();

        // Check if parent directory exists and is not a symlink
        if (fs::exists(parentPath)) {
            struct stat st;
            std::string parentPathStr = parentPath.string(); // Convert to string to avoid use-after-free
            if (lstat(parentPathStr.c_str(), &st) == 0) {
                if (S_ISLNK(st.st_mode)) {
                    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path parent directory is a symbolic link (security risk): '{}'", parentPath.string()), Logging::DebugLevel::CRITICAL);
                    return false;
                }
            }
        }

        // If file exists, check if it's a symlink or special file
        if (fs::exists(fullPath)) {
            struct stat st;
            std::string fullPathStr = fullPath.string(); // Convert to string to avoid use-after-free
            if (lstat(fullPathStr.c_str(), &st) == 0) {
                if (S_ISLNK(st.st_mode)) {
                    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path is a symbolic link (security risk): '{}'", path), Logging::DebugLevel::CRITICAL);
                    return false;
                }

                // Only allow regular files
                if (!S_ISREG(st.st_mode)) {
                    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File path is not a regular file: '{}'", path), Logging::DebugLevel::CRITICAL);
                    return false;
                }
            }
        }

        return true;
    }

    void loadFromFile() {
        fileBuffer.clear();

        // Validate file path for security
        if (!validateFilePath(filepath)) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Invalid or unsafe file path: '{}'", filepath), Logging::DebugLevel::CRITICAL);
            return;
        }

        // Try to open the file and read its contents
        std::ifstream file(filepath, std::ios::binary);
        if (file) {
            // Read file into buffer with size validation
            file.seekg(0, std::ios::end);
            auto fileSizePos = file.tellg();
            file.seekg(0, std::ios::beg);

            // Validate file size to prevent excessive memory allocation
            if (fileSizePos < 0) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Error getting file size for '{}'", filepath), Logging::DebugLevel::CRITICAL);
                return;
            }

            size_t fileSize = static_cast<size_t>(fileSizePos);

            // Limit maximum file size to prevent memory exhaustion (e.g., 100MB)
            constexpr size_t MAX_FILE_SIZE = 100 * 1024 * 1024;
            if (fileSize > MAX_FILE_SIZE) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File '{}' is too large ({} bytes, max: {} bytes)", filepath, fileSize, MAX_FILE_SIZE), Logging::DebugLevel::CRITICAL);
                return;
            }

            fileBuffer.resize(fileSize);
            if (fileSize > 0) {
                // Initialize buffer to zeros for security
                std::fill(fileBuffer.begin(), fileBuffer.end(), 0);

                // Attempt to read the file
                file.read(reinterpret_cast<char*>(fileBuffer.data()), fileSize);

                // Check if the read was successful and validate bytes read
                std::streamsize bytesRead = file.gcount();
                if (!file && !file.eof()) {
                    // Read error occurred (not just EOF)
                    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Error reading file '{}': {}", filepath, "I/O error during read"), Logging::DebugLevel::CRITICAL);
                    fileBuffer.clear();
                    return;
                }

                if (bytesRead != static_cast<std::streamsize>(fileSize)) {
                    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("File '{}': expected {} bytes, read {} bytes (file may have changed during read)", filepath, fileSize, bytesRead), Logging::DebugLevel::IMPORTANT);

                    // Resize buffer to actual bytes read to prevent access to uninitialized memory
                    if (bytesRead > 0 && bytesRead < static_cast<std::streamsize>(fileSize)) {
                        fileBuffer.resize(static_cast<size_t>(bytesRead));
                    } else if (bytesRead <= 0) {
                        fileBuffer.clear();
                        return;
                    }
                }
            }

            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Loaded {} bytes from file '{}'", fileSize, filepath), Logging::DebugLevel::INFO);
        } else {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Creating new file '{}'", filepath), Logging::DebugLevel::INFO);
        }
    }

    void saveToFile() {
        // Validate file path for security before writing
        if (!validateFilePath(filepath)) {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Invalid or unsafe file path for writing: '{}'", filepath), Logging::DebugLevel::CRITICAL);
            return;
        }

        // Create directories if needed (but validate the path first)
        fs::path path(filepath);
        if (path.has_parent_path()) {
            try {
                fs::create_directories(path.parent_path());
            } catch (const fs::filesystem_error& e) {
                Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("Failed to create directories for '{}': {}", filepath, e.what()), Logging::DebugLevel::CRITICAL);
                return;
            }
        }

        // Write buffer to file
        std::ofstream file(filepath, std::ios::binary);
        if (file) {
            file.write(reinterpret_cast<const char*>(fileBuffer.data()), fileBuffer.size());
            DEBUG_INFO(Logging::DebugCategory::IO_FILE, "{:22}  Wrote {} bytes to file '{}'", "", fileBuffer.size(), filepath);
        } else {
            Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_FILE, fmt::format("{:22}  Failed to write to file '{}'", "", filepath), Logging::DebugLevel::CRITICAL);
        }
    }

    std::string filepath;
    std::vector<uint8_t> fileBuffer;
    size_t position;
    std::mutex mutex;
};

} // namespace vhw
