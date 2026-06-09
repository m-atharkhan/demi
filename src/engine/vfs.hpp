#pragma once
#include <filesystem>
#include <string>
#include <optional>
#include <memory>
#include "virtual_disk.hpp"

namespace demi {
namespace sandbox {

class VirtualFileSystem {
    std::filesystem::path jail_root_;
    bool restrict_io_;
    std::unique_ptr<VirtualDisk> virtual_disk_;

public:
    explicit VirtualFileSystem(const std::string& root, bool restrict_io)
        : jail_root_(std::filesystem::absolute(root)),
          restrict_io_(restrict_io) {
    }

    void set_virtual_disk(std::unique_ptr<VirtualDisk> vd) {
        virtual_disk_ = std::move(vd);
    }
    VirtualDisk* get_virtual_disk() const { return virtual_disk_.get(); }
    bool has_virtual_disk() const { return virtual_disk_ != nullptr; }

    std::optional<std::filesystem::path> resolve_safe_path(const std::string& requested_path) const {
        if (!restrict_io_) {
            return std::filesystem::path(requested_path);
        }

        namespace fs = std::filesystem;
        fs::path req(requested_path);

        fs::path normalized_req = req.lexically_normal();
        fs::path relative_req = normalized_req.is_absolute()
            ? normalized_req.relative_path()
            : normalized_req;

        fs::path absolute_target = jail_root_ / relative_req;

        try {
            fs::path canonical_jail = fs::weakly_canonical(jail_root_);
            fs::path canonical_target = fs::weakly_canonical(absolute_target);

            auto jail_it = canonical_jail.begin();
            auto jail_end = canonical_jail.end();
            auto target_it = canonical_target.begin();
            auto target_end = canonical_target.end();

            for (; jail_it != jail_end; ++jail_it, ++target_it) {
                if (target_it == target_end || *jail_it != *target_it) {
                    return std::nullopt;
                }
            }
            return canonical_target;
        } catch (const fs::filesystem_error&) {
            return std::nullopt;
        }
    }
};

} // namespace sandbox
} // namespace demi
