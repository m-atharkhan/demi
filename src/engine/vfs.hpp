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

        try {
            fs::path canonical_jail = fs::weakly_canonical(jail_root_);
            fs::path canonical_target = fs::weakly_canonical(absolute_target);

            // Check if canonical_target is within canonical_jail by comparing path components
            auto jail_it = canonical_jail.begin();
            auto jail_end = canonical_jail.end();
            auto target_it = canonical_target.begin();
            auto target_end = canonical_target.end();

            for (; jail_it != jail_end; ++jail_it, ++target_it) {
                if (target_it == target_end || *jail_it != *target_it) {
                    return std::nullopt;
                }
            }
            // All jail components matched, so target is inside jail
            return canonical_target;
        } catch (const fs::filesystem_error&) {
            // If canonicalization fails, treat as unsafe
            return std::nullopt;
        }
    }
};

} // namespace sandbox
} // namespace demi