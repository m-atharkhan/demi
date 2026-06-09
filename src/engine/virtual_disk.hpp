#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace demi {
namespace sandbox {

// Stores virtual files inside a single .vfs container file.
// Supports directory hierarchy via '/' in filenames.
class VirtualDisk {
public:
    struct FileEntry {
        std::vector<uint8_t> data;
        uint32_t flags = 0;
        uint16_t permissions = 0644;  // rw-r--r--
        std::string owner;             // owning user
    };

    struct OpenFile {
        std::string filename;
        size_t position = 0;
        int flags = 0;
        bool dirty = false;
    };

    VirtualDisk() = default;
    ~VirtualDisk();

    bool mount(const std::string& container_path);
    bool save();
    bool is_mounted() const { return mounted_; }

    // --- File operations ---
    int open(const std::string& filename, int flags);
    int read(int handle, void* buf, int count);
    int write(int handle, const void* buf, int count);
    int close(int handle);
    int remove(const std::string& filename);
    bool exists(const std::string& filename) const;
    int size(const std::string& filename) const;

    // --- Directory operations ---
    std::string get_cwd() const { return cwd_; }
    bool set_cwd(const std::string& path);
    // Resolves path: if relative, prepends cwd_. Normalizes ".." and ".".
    std::string resolve_path(const std::string& path) const;
    // Creates a directory marker entry.
    bool mkdir(const std::string& path);
    // Creates directory and all missing parents (mkdir -p).
    bool mkdir_p(const std::string& path);
    // Removes directory marker and all entries under the directory prefix.
    bool rmdir(const std::string& path);

    // --- Permissions & users ---
    std::string get_user() const { return current_user_; }
    void set_user(const std::string& u) { current_user_ = u; }
    bool chmod(const std::string& path, uint16_t mode);
    bool chown(const std::string& path, const std::string& new_owner);

    // --- User management ---
    bool needs_init() const;
    void init_filesystem();
    bool add_user(const std::string& username, const std::string& password);
    bool del_user(const std::string& username);
    bool auth_user(const std::string& username, const std::string& password);
    bool chpasswd(const std::string& username, const std::string& new_password);
    bool copy_file(const std::string& src, const std::string& dst);
    bool move_file(const std::string& src, const std::string& dst);
    bool link_file(const std::string& src, const std::string& dst);
    std::string du_stats(const std::string& prefix) const;
    std::string find_files(const std::string& pattern) const;
    std::string stat_file(const std::string& path) const;
    std::string wc_file(const std::string& path) const;
    std::string head_file(const std::string& path, int lines) const;
    std::string tail_file(const std::string& path, int lines) const;
    std::string grep_file(const std::string& path, const std::string& pattern) const;
    std::string diff_files(const std::string& a, const std::string& b) const;
    std::string sort_file(const std::string& path) const;
    std::string df_stats() const;
    bool user_exists(const std::string& username) const;
    std::string get_first_user() const;
    void set_current_user(const std::string& u) { current_user_ = u; }
    void set_previous_user(const std::string& u) { previous_user_ = u; }
    std::string get_previous_user() const { return previous_user_; }

    // --- Listing ---
    // Lists direct children of the given prefix (newline-separated).
    // Directories are shown with trailing '/'.
    int list_display_at(const std::string& prefix, char* buf, int buf_size) const;
    // Like list_display_at but includes file sizes: "name  SIZE\n"
    int list_display_long(const std::string& prefix, char* buf, int buf_size) const;
    // Like list_display_at but includes hidden files (starting with .)
    int list_display_all(const std::string& prefix, char* buf, int buf_size) const;
    // Long listing with human-readable sizes (1.2K, 3.4M, etc.)
    int list_display_human(const std::string& prefix, char* buf, int buf_size) const;
    // Recursive listing: shows full paths for all files under prefix
    int list_display_recursive(const std::string& prefix, char* buf, int buf_size) const;
    // Convenience: lists children of cwd_.
    int list_display_cwd(char* buf, int buf_size) const;
    // Lists all files (null-separated, for C API).
    int list(char* buf, int buf_size) const;
    // Newline-separated listing of all files (deprecated in favor of list_display_cwd).
    int list_display(char* buf, int buf_size) const;

    // --- Host-side access ---
    std::vector<uint8_t> read_file(const std::string& filename) const;
    bool write_file(const std::string& filename, const std::vector<uint8_t>& data);
    bool delete_file(const std::string& filename);
    std::vector<std::string> list_files() const;
    // Lists direct children filenames under prefix.
    std::vector<std::string> list_dir(const std::string& prefix) const;

private:
    static constexpr uint32_t kMagic = 0x48435646;
    static constexpr uint32_t kVersion = 1;
    static constexpr uint32_t kDirFlag = 0x80000000;

    bool mounted_ = false;
    std::string container_path_;
    std::string cwd_ = "/";
    std::string current_user_ = "demi";
    std::string previous_user_;
    std::map<std::string, FileEntry> files_;
    std::set<std::string> dirs_;  // directory markers (paths ending with /)
    std::map<int, OpenFile> open_files_;
    int next_handle_ = 100;

    bool parse_container(const std::vector<uint8_t>& raw);
    std::vector<uint8_t> serialize() const;
    std::string normalize_path(const std::string& path) const;
};

} // namespace sandbox
} // namespace demi
