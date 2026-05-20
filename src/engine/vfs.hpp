#pragma once
#include <filesystem>
#include <string>
#include <optional>

namespace demi {
namespace sandbox {

class VirtualFileSystem {
    std::filesystem::path jail_root_;
    bool restrict_io_;

public:
    explicit VirtualFileSystem(const std::string& root, bool restrict_io) 
        : jail_root_(std::filesystem::absolute(root)), 
          restrict_io_(restrict_io) {
    }

    // Returns a safe path if authorized, or std::nullopt if it breaks the jail
    std::optional<std::filesystem::path> resolve_safe_path(const std::string& requested_path) const {
        if (!restrict_io_) {
            return std::filesystem::path(requested_path);
        }

        namespace fs = std::filesystem;
        fs::path req(requested_path);
        
        // Remove . and .. components lexically to prevent traversal mapping
        fs::path normalized_req = req.lexically_normal();
        
        // Remove leading slash to make it relative to jail_root_ if it's absolute
        fs::path relative_req = normalized_req.is_absolute() 
            ? normalized_req.relative_path() 
            : normalized_req;
        
        // Construct the absolute path within the jail
        fs::path absolute_target = jail_root_ / relative_req;
        absolute_target = absolute_target.lexically_normal();

        // Check if the resulting path actually starts with the jail_root_
        auto root_str = jail_root_.string();
        auto target_str = absolute_target.string();
        
        // Path containment check
        if (target_str.compare(0, root_str.length(), root_str) == 0) {
            return absolute_target;
        }
        
        return std::nullopt; // Directory traversal attempted or outside bounds
    }
};

} // namespace sandbox
} // namespace demi