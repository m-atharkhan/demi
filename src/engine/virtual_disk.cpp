#include "virtual_disk.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace demi {
namespace sandbox {

VirtualDisk::~VirtualDisk() {
    if (mounted_) save();
}

bool VirtualDisk::mount(const std::string& container_path) {
    container_path_ = container_path;
    files_.clear();
    open_files_.clear();
    next_handle_ = 100;

    std::ifstream f(container_path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        mounted_ = true;
        // Seed root directory marker
        FileEntry de;
        de.flags = kDirFlag;
        files_["/.dir"] = de;
        return true;
    }

    std::streamsize file_size = f.tellg();
    if (file_size < 12) { f.close(); mounted_ = true; return true; }
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> raw(static_cast<size_t>(file_size));
    if (!f.read(reinterpret_cast<char*>(raw.data()), file_size)) {
        mounted_ = true; return true;
    }
    f.close();

    if (!parse_container(raw)) files_.clear();
    // Ensure root marker exists
    if (files_.find("/.dir") == files_.end()) {
        FileEntry de; de.flags = kDirFlag;
        files_["/.dir"] = de;
    }
    mounted_ = true;
    return true;
}

bool VirtualDisk::parse_container(const std::vector<uint8_t>& raw) {
    if (raw.size() < 12) return false;
    size_t pos = 0;
    auto read_u32 = [&]() -> uint32_t {
        if (pos + 4 > raw.size()) return 0;
        uint32_t v = static_cast<uint32_t>(raw[pos])
                   | (static_cast<uint32_t>(raw[pos + 1]) << 8)
                   | (static_cast<uint32_t>(raw[pos + 2]) << 16)
                   | (static_cast<uint32_t>(raw[pos + 3]) << 24);
        pos += 4; return v;
    };
    auto read_u16 = [&]() -> uint16_t {
        if (pos + 2 > raw.size()) return 0;
        uint16_t v = static_cast<uint16_t>(raw[pos])
                   | (static_cast<uint16_t>(raw[pos + 1]) << 8);
        pos += 2; return v;
    };

    uint32_t magic = read_u32();
    if (magic != kMagic) return false;
    uint32_t version = read_u32();
    // v1: original format. v2: adds permissions + owner
    if (version != kVersion && version != 2) return false;
    uint32_t file_count = read_u32();

    struct IndexEntry { std::string name; uint32_t data_size; uint32_t flags; uint16_t perms; std::string owner; };
    std::vector<IndexEntry> entries;
    for (uint32_t i = 0; i < file_count; ++i) {
        uint16_t name_len = read_u16();
        if (pos + name_len > raw.size()) return false;
        std::string name(raw.begin() + static_cast<std::ptrdiff_t>(pos),
                         raw.begin() + static_cast<std::ptrdiff_t>(pos + name_len));
        pos += name_len;
        uint32_t data_size = read_u32();
        uint32_t flags = read_u32();
        uint16_t perms = 0644;
        std::string owner = "demi";
        if (version >= 2) {
            perms = read_u16();
            uint16_t owner_len = read_u16();
            if (pos + owner_len <= raw.size()) {
                owner.assign(raw.begin() + static_cast<std::ptrdiff_t>(pos),
                             raw.begin() + static_cast<std::ptrdiff_t>(pos + owner_len));
                pos += owner_len;
            }
            if (flags & kDirFlag) perms = 0755;
        }
        entries.push_back({name, data_size, flags, perms, owner});
    }
    for (const auto& entry : entries) {
        if (pos + entry.data_size > raw.size()) return false;
        FileEntry fe;
        fe.flags = entry.flags;
        fe.permissions = entry.perms;
        fe.owner = entry.owner;
        fe.data.assign(raw.begin() + static_cast<std::ptrdiff_t>(pos),
                       raw.begin() + static_cast<std::ptrdiff_t>(pos + entry.data_size));
        pos += entry.data_size;
        files_[entry.name] = std::move(fe);
    }
    return true;
}

std::vector<uint8_t> VirtualDisk::serialize() const {
    std::vector<uint8_t> out;
    auto write_u32 = [&](uint32_t v) {
        out.push_back(static_cast<uint8_t>(v & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    };
    auto write_u16 = [&](uint16_t v) {
        out.push_back(static_cast<uint8_t>(v & 0xFF));
        out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    };
    write_u32(kMagic);
    write_u32(static_cast<uint32_t>(2));  // v2: permissions + owner
    write_u32(static_cast<uint32_t>(files_.size()));
    for (const auto& kv : files_) {
        write_u16(static_cast<uint16_t>(kv.first.size()));
        out.insert(out.end(), kv.first.begin(), kv.first.end());
        write_u32(static_cast<uint32_t>(kv.second.data.size()));
        write_u32(kv.second.flags);
        write_u16(kv.second.permissions);
        write_u16(static_cast<uint16_t>(kv.second.owner.size()));
        out.insert(out.end(), kv.second.owner.begin(), kv.second.owner.end());
    }
    for (const auto& kv : files_) {
        out.insert(out.end(), kv.second.data.begin(), kv.second.data.end());
    }
    return out;
}

bool VirtualDisk::save() {
    if (!mounted_ || container_path_.empty()) return false;
    std::vector<uint8_t> raw = serialize();
    std::ofstream f(container_path_, std::ios::binary | std::ios::trunc);
    if (!f.is_open()) return false;
    f.write(reinterpret_cast<const char*>(raw.data()),
            static_cast<std::streamsize>(raw.size()));
    f.close();
    return true;
}

// --- Path utilities ---

std::string VirtualDisk::normalize_path(const std::string& path) const {
    std::vector<std::string> parts;
    std::istringstream ss(path);
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (part.empty() || part == ".") continue;
        if (part == "..") {
            if (!parts.empty()) parts.pop_back();
        } else {
            parts.push_back(part);
        }
    }
    if (parts.empty()) return "/";
    std::string result;
    for (const auto& p : parts) result += "/" + p;
    return result;
}

std::string VirtualDisk::resolve_path(const std::string& path) const {
    if (path.empty()) return cwd_;
    std::string resolved;
    if (path[0] == '/') {
        resolved = normalize_path(path);
    } else {
        std::string base = (cwd_ == "/") ? "" : cwd_;
        resolved = normalize_path(base + "/" + path);
    }
    if (resolved.empty()) resolved = "/";
    return resolved;
}

bool VirtualDisk::set_cwd(const std::string& path) {
    std::string new_cwd = resolve_path(path);
    // Verify the directory exists (check for marker or any file under it)
    std::string marker = (new_cwd == "/") ? "/.dir" : new_cwd + "/.dir";
    if (files_.find(marker) != files_.end()) {
        cwd_ = new_cwd;
        return true;
    }
    // Also check if any files exist under this path
    std::string prefix = (new_cwd == "/") ? "/" : new_cwd + "/";
    for (const auto& kv : files_) {
        if (kv.first.size() > prefix.size() &&
            kv.first.compare(0, prefix.size(), prefix) == 0) {
            cwd_ = new_cwd;
            return true;
        }
    }
    // For root, always succeed
    if (new_cwd == "/") { cwd_ = "/"; return true; }
    return false; // directory doesn't exist
}

// --- File operations ---

int VirtualDisk::open(const std::string& filename, int flags) {
    if (!mounted_) return -1;
    std::string resolved = resolve_path(filename);
    if (resolved.empty()) return -1;
    // Can't open a directory marker
    auto it = files_.find(resolved);
    if (it != files_.end() && (it->second.flags & kDirFlag)) return -1;

    bool writing = (flags & 1) != 0;
    if (!exists(resolved)) {
        if (!writing) return -1;
        FileEntry fe;
        files_[resolved] = fe;
        } else if ((flags & 0x200) && writing) {
            // O_TRUNC: truncate existing file
            files_[resolved].data.clear();
    }
    int handle = next_handle_++;
    OpenFile of;
    of.filename = resolved;
    of.position = 0;
    of.flags = flags;
    open_files_[handle] = of;
    return handle;
}

int VirtualDisk::read(int handle, void* buf, int count) {
    if (!mounted_) return -1;
    auto it = open_files_.find(handle);
    if (it == open_files_.end()) return -1;
    OpenFile& of = it->second;
    auto fe_it = files_.find(of.filename);
    if (fe_it == files_.end()) return -1;
    FileEntry& fe = fe_it->second;
    size_t available = fe.data.size() - of.position;
    size_t n = std::min(static_cast<size_t>(count), available);
    if (n > 0) {
        std::memcpy(buf, fe.data.data() + of.position, n);
        of.position += n;
    }
    return static_cast<int>(n);
}

int VirtualDisk::write(int handle, const void* buf, int count) {
    if (!mounted_) return -1;
    auto it = open_files_.find(handle);
    if (it == open_files_.end()) return -1;
    OpenFile& of = it->second;
    auto fe_it = files_.find(of.filename);
    if (fe_it == files_.end()) return -1;
    FileEntry& fe = fe_it->second;
    size_t needed = of.position + static_cast<size_t>(count);
    if (needed > fe.data.size()) fe.data.resize(needed);
    std::memcpy(fe.data.data() + of.position, buf, static_cast<size_t>(count));
    of.position += static_cast<size_t>(count);
    of.dirty = true;
    return count;
}

int VirtualDisk::close(int handle) {
    if (!mounted_) return -1;
    auto it = open_files_.find(handle);
    if (it == open_files_.end()) return -1;
    open_files_.erase(it);
    save();
    return 0;
}

int VirtualDisk::remove(const std::string& filename) {
    if (!mounted_) return -1;
    std::string resolved = resolve_path(filename);
    auto it = files_.find(resolved);
    if (it == files_.end()) return -1;
    if (it->second.flags & kDirFlag) return -1; // use rmdir for dirs
    files_.erase(it);
    save();
    return 0;
}

bool VirtualDisk::exists(const std::string& filename) const {
    return files_.find(resolve_path(filename)) != files_.end();
}

int VirtualDisk::size(const std::string& filename) const {
    auto it = files_.find(resolve_path(filename));
    if (it == files_.end()) return -1;
    return static_cast<int>(it->second.data.size());
}

// --- Directory operations ---

bool VirtualDisk::mkdir(const std::string& path) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(path);
    if (resolved == "/") return true; // root always exists
    std::string marker = resolved + "/.dir";
    if (files_.find(marker) != files_.end()) return true; // already exists
    // Check if parent exists
    size_t last_slash = resolved.rfind('/');
    if (last_slash == std::string::npos) return false;
    std::string parent = resolved.substr(0, last_slash);
    if (parent.empty()) parent = "/";
    if (parent != "/") {
        std::string parent_marker = parent + "/.dir";
        if (files_.find(parent_marker) == files_.end()) return false;
    }
    FileEntry de;
    de.flags = kDirFlag;
    files_[marker] = de;
    save();
    return true;
}

bool VirtualDisk::mkdir_p(const std::string& path) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(path);
    if (resolved == "/") return true;
    // Build path components and create each level
    std::string accumulated;
    size_t start = 0;
    if (resolved[0] == '/') start = 1;
    while (start < resolved.size()) {
        size_t slash = resolved.find('/', start);
        std::string component = (slash == std::string::npos)
            ? resolved.substr(start)
            : resolved.substr(start, slash - start);
        if (!component.empty()) {
            accumulated += "/" + component;
            if (accumulated != "/") {
                std::string marker = accumulated + "/.dir";
                if (files_.find(marker) == files_.end()) {
                    FileEntry de;
                    de.flags = kDirFlag;
                    files_[marker] = de;
                }
            }
        }
        if (slash == std::string::npos) break;
        start = slash + 1;
    }
    save();
    return true;
}

bool VirtualDisk::rmdir(const std::string& path) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(path);
    if (resolved == "/") return false;
    std::string prefix = resolved + "/";
    auto it = files_.begin();
    while (it != files_.end()) {
        if (it->first.size() >= prefix.size() &&
            it->first.compare(0, prefix.size(), prefix) == 0) {
            it = files_.erase(it);
        } else {
            ++it;
        }
    }
    save();
    return true;
}

bool VirtualDisk::chmod(const std::string& path, uint16_t mode) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(path);
    auto it = files_.find(resolved);
    if (it == files_.end()) return false;
    // Only allow permission bits (lower 12 bits: setuid/setgid/sticky + rwxrwxrwx)
    it->second.permissions = mode & 0xFFF;
    save();
    return true;
}

bool VirtualDisk::chown(const std::string& path, const std::string& new_owner) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(path);
    auto it = files_.find(resolved);
    if (it == files_.end()) return false;
    it->second.owner = new_owner;
    save();
    return true;
}

bool VirtualDisk::needs_init() const {
    return !exists("/etc/passwd");
}

void VirtualDisk::init_filesystem() {
    if (!mounted_) return;
    mkdir_p("/home");
    mkdir_p("/etc");
    mkdir_p("/bin");
    mkdir_p("/tmp");
    mkdir_p("/var");
    mkdir_p("/var/log");
    mkdir_p("/root");

    // /etc/hostname
    std::string hostname = "hackercraft\n";
    write_file("/etc/hostname", std::vector<uint8_t>(hostname.begin(), hostname.end()));
    auto it = files_.find("/etc/hostname");
    if (it != files_.end()) it->second.permissions = 0644;

    // /etc/motd
    std::string motd = "Welcome to HackerCraft VFS!\n";
    write_file("/etc/motd", std::vector<uint8_t>(motd.begin(), motd.end()));
    it = files_.find("/etc/motd");
    if (it != files_.end()) it->second.permissions = 0644;

    // /var/log/syslog
    write_file("/var/log/syslog", std::vector<uint8_t>());
    it = files_.find("/var/log/syslog");
    if (it != files_.end()) it->second.permissions = 0644;

    // Add root user
    add_user("root", "");

    // Create /etc/passwd content
    std::string passwd = "root:x:0:0:root:/root:/bin/demishell\n";
    std::vector<uint8_t> pw(passwd.begin(), passwd.end());
    write_file("/etc/passwd", pw);
    it = files_.find("/etc/passwd");
    if (it != files_.end()) it->second.permissions = 0644;

    // Create /etc/shadow (empty initially)
    std::string shadow = "root::19234:0:99999:7:::\n";
    std::vector<uint8_t> sh(shadow.begin(), shadow.end());
    write_file("/etc/shadow", sh);
    it = files_.find("/etc/shadow");
    if (it != files_.end()) it->second.permissions = 0600;

    save();
}

bool VirtualDisk::add_user(const std::string& username, const std::string& password) {
    if (!mounted_) return false;
    if (username.empty()) return false;
    if (user_exists(username)) return false;

    // Create home directory
    std::string home = "/home/" + username;
    mkdir_p(home);

    // Add to passwd
    auto pw_it = files_.find("/etc/passwd");
    std::string passwd;
    if (pw_it != files_.end())
        passwd = std::string(pw_it->second.data.begin(), pw_it->second.data.end());
    int uid = 1000;
    // Find next uid
    size_t pos = 0;
    while ((pos = passwd.find(":x:", pos)) != std::string::npos) {
        pos += 3;
        size_t end = passwd.find(':', pos);
        if (end != std::string::npos) {
            int existing = std::stoi(passwd.substr(pos, end - pos));
            if (existing >= uid) uid = existing + 1;
        }
    }
    passwd += username + ":x:" + std::to_string(uid) + ":" + std::to_string(uid)
            + ":" + username + ":" + home + ":/bin/demishell\n";
    write_file("/etc/passwd", std::vector<uint8_t>(passwd.begin(), passwd.end()));

    // Add to shadow
    auto sh_it = files_.find("/etc/shadow");
    std::string shadow;
    if (sh_it != files_.end())
        shadow = std::string(sh_it->second.data.begin(), sh_it->second.data.end());
    shadow += username + ":" + password + ":19234:0:99999:7:::\n";
    write_file("/etc/shadow", std::vector<uint8_t>(shadow.begin(), shadow.end()));

    save();
    return true;
}

bool VirtualDisk::del_user(const std::string& username) {
    if (!mounted_) return false;
    if (username == "root") return false; // can't delete root
    if (!user_exists(username)) return false;

    // Remove from passwd
    auto pw_it = files_.find("/etc/passwd");
    if (pw_it != files_.end()) {
        std::string passwd(pw_it->second.data.begin(), pw_it->second.data.end());
        std::string new_passwd;
        size_t pos = 0;
        while (pos < passwd.size()) {
            size_t nl = passwd.find('\n', pos);
            if (nl == std::string::npos) nl = passwd.size();
            std::string line = passwd.substr(pos, nl - pos);
            pos = nl + 1;
            if (line.empty()) continue;
            if (line.compare(0, username.size(), username) == 0 &&
                line.size() > username.size() && line[username.size()] == ':')
                continue; // skip this user
            new_passwd += line + "\n";
        }
        write_file("/etc/passwd", std::vector<uint8_t>(new_passwd.begin(), new_passwd.end()));
    }

    // Remove from shadow
    auto sh_it = files_.find("/etc/shadow");
    if (sh_it != files_.end()) {
        std::string shadow(sh_it->second.data.begin(), sh_it->second.data.end());
        std::string new_shadow;
        size_t pos = 0;
        while (pos < shadow.size()) {
            size_t nl = shadow.find('\n', pos);
            if (nl == std::string::npos) nl = shadow.size();
            std::string line = shadow.substr(pos, nl - pos);
            pos = nl + 1;
            if (line.empty()) continue;
            if (line.compare(0, username.size(), username) == 0 &&
                line.size() > username.size() && line[username.size()] == ':')
                continue;
            new_shadow += line + "\n";
        }
        write_file("/etc/shadow", std::vector<uint8_t>(new_shadow.begin(), new_shadow.end()));
    }

    // Remove home directory
    std::string home = "/home/" + username;
    std::string prefix = home + "/";
    auto it = files_.begin();
    while (it != files_.end()) {
        if (it->first == home + "/.dir" ||
            (it->first.size() > prefix.size() &&
             it->first.compare(0, prefix.size(), prefix) == 0)) {
            it = files_.erase(it);
        } else {
            ++it;
        }
    }

    save();
    return true;
}

bool VirtualDisk::chpasswd(const std::string& username, const std::string& new_password) {
    if (!mounted_) return false;
    if (!user_exists(username)) return false;

    auto sh_it = files_.find("/etc/shadow");
    if (sh_it == files_.end()) return false;
    std::string shadow(sh_it->second.data.begin(), sh_it->second.data.end());
    std::string new_shadow;
    size_t pos = 0;
    while (pos < shadow.size()) {
        size_t nl = shadow.find('\n', pos);
        if (nl == std::string::npos) nl = shadow.size();
        std::string line = shadow.substr(pos, nl - pos);
        pos = nl + 1;
        if (line.empty()) continue;
        size_t c1 = line.find(':');
        if (c1 != std::string::npos && line.substr(0, c1) == username) {
            // Replace password field
            size_t c2 = line.find(':', c1 + 1);
            if (c2 != std::string::npos)
                line = username + ":" + new_password + line.substr(c2);
            else
                line = username + ":" + new_password;
        }
        new_shadow += line + "\n";
    }
    write_file("/etc/shadow", std::vector<uint8_t>(new_shadow.begin(), new_shadow.end()));
    save();
    return true;
}

bool VirtualDisk::copy_file(const std::string& src, const std::string& dst) {
    if (!mounted_) return false;
    std::string resolved_src = resolve_path(src);
    std::string resolved_dst = resolve_path(dst);
    auto it = files_.find(resolved_src);
    if (it == files_.end()) return false;
    FileEntry fe = it->second; // copy
    files_[resolved_dst] = fe;
    save();
    return true;
}

bool VirtualDisk::move_file(const std::string& src, const std::string& dst) {
    if (!mounted_) return false;
    std::string resolved_src = resolve_path(src);
    std::string resolved_dst = resolve_path(dst);
    auto it = files_.find(resolved_src);
    if (it == files_.end()) return false;
    FileEntry fe = std::move(it->second);
    files_.erase(it);
    files_[resolved_dst] = std::move(fe);
    save();
    return true;
}

bool VirtualDisk::link_file(const std::string& src, const std::string& dst) {
    if (!mounted_) return false;
    std::string rs = resolve_path(src), rd = resolve_path(dst);
    auto it = files_.find(rs);
    if (it == files_.end()) return false;
    files_[rd] = it->second;  // share data (copy-on-write semantics)
    save();
    return true;
}

std::string VirtualDisk::du_stats(const std::string& prefix) const {
    std::string pfx = prefix.empty() ? cwd_ : resolve_path(prefix);
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    int count = 0; uint64_t total = 0;
    for (const auto& kv : files_) {
        if (kv.first.size() > pfx.size() && kv.first.compare(0, pfx.size(), pfx) == 0 &&
            !(kv.second.flags & kDirFlag)) { count++; total += kv.second.data.size(); }
    }
    return std::to_string(count) + " files, " + std::to_string(total) + " bytes\n";
}

std::string VirtualDisk::find_files(const std::string& pattern) const {
    std::string result;
    for (const auto& kv : files_) {
        if (kv.first.find(pattern) != std::string::npos && !(kv.second.flags & kDirFlag))
            result += kv.first + "\n";
    }
    return result;
}

std::string VirtualDisk::stat_file(const std::string& path) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "File not found\n";
    const auto& fe = it->second;
    bool dir = (fe.flags & kDirFlag) != 0;
    auto perm_str = [](uint16_t p, bool d) -> std::string {
        std::string s = d ? "d" : ".";
        s += (p&0400)?"r":"-"; s += (p&0200)?"w":"-"; s += (p&0100)?"x":"-";
        s += (p&0040)?"r":"-"; s += (p&0020)?"w":"-"; s += (p&0010)?"x":"-";
        s += (p&0004)?"r":"-"; s += (p&0002)?"w":"-"; s += (p&0001)?"x":"-";
        return s;
    };
    return "  File: " + rp + "\n  Type: " + (dir ? "directory\n" : "regular file\n") +
           "  Size: " + std::to_string(fe.data.size()) + "\n  Perms: " +
           perm_str(fe.permissions, dir) + " (" + std::to_string(fe.permissions) + ")\n" +
           "  Owner: " + (fe.owner.empty() ? "demi" : fe.owner) + "\n";
}

std::string VirtualDisk::wc_file(const std::string& path) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "File not found\n";
    const auto& d = it->second.data;
    int lines = 0, words = 0, chars = static_cast<int>(d.size());
    bool in_word = false;
    for (uint8_t b : d) {
        if (b == '\n') lines++;
        if (b == ' ' || b == '\n' || b == '\t') { in_word = false; }
        else if (!in_word) { in_word = true; words++; }
    }
    return std::to_string(lines) + " " + std::to_string(words) + " " + std::to_string(chars) + " " + rp + "\n";
}

std::string VirtualDisk::head_file(const std::string& path, int lines) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "";
    if (lines <= 0) lines = 10;
    const auto& d = it->second.data;
    int count = 0; size_t pos = 0;
    while (pos < d.size() && count < lines) {
        if (d[pos] == '\n') count++;
        pos++;
    }
    return std::string(d.begin(), d.begin() + static_cast<std::ptrdiff_t>(pos));
}

std::string VirtualDisk::tail_file(const std::string& path, int lines) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "";
    if (lines <= 0) lines = 10;
    const auto& d = it->second.data;
    int total = 0; for (uint8_t b : d) if (b == '\n') total++;
    int skip = (total > lines) ? total - lines : 0;
    int count = 0; size_t pos = 0;
    while (pos < d.size() && count < skip) { if (d[pos] == '\n') count++; pos++; }
    return std::string(d.begin() + static_cast<std::ptrdiff_t>(pos), d.end());
}

std::string VirtualDisk::grep_file(const std::string& path, const std::string& pattern) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "";
    const auto& d = it->second.data;
    std::string result, line;
    for (uint8_t b : d) {
        if (b == '\n') { if (line.find(pattern) != std::string::npos) result += line + "\n"; line.clear(); }
        else line += static_cast<char>(b);
    }
    if (!line.empty() && line.find(pattern) != std::string::npos) result += line + "\n";
    return result;
}

std::string VirtualDisk::diff_files(const std::string& a, const std::string& b) const {
    std::string ra = resolve_path(a), rb = resolve_path(b);
    auto ita = files_.find(ra), itb = files_.find(rb);
    if (ita == files_.end() || itb == files_.end()) return "diff: file not found\n";
    std::string sa(ita->second.data.begin(), ita->second.data.end());
    std::string sb(itb->second.data.begin(), itb->second.data.end());
    if (sa == sb) return "";
    // Simple line diff
    std::string result;
    std::istringstream as(sa), bs(sb);
    std::string la, lb; int n = 0;
    while (std::getline(as, la) && std::getline(bs, lb)) {
        n++; if (la != lb) result += std::to_string(n) + "c" + std::to_string(n) + "\n< " + la + "\n---\n> " + lb + "\n";
    }
    while (std::getline(as, la)) { n++; result += std::to_string(n) + "a" + std::to_string(n) + "\n> " + la + "\n"; }
    while (std::getline(bs, lb)) { n++; result += std::to_string(n) + "d" + std::to_string(n) + "\n< " + lb + "\n"; }
    return result;
}

std::string VirtualDisk::sort_file(const std::string& path) const {
    std::string rp = resolve_path(path);
    auto it = files_.find(rp);
    if (it == files_.end()) return "";
    std::string data(it->second.data.begin(), it->second.data.end());
    std::vector<std::string> lines;
    std::istringstream ss(data); std::string line;
    while (std::getline(ss, line)) lines.push_back(line);
    std::sort(lines.begin(), lines.end());
    std::string result;
    for (const auto& l : lines) result += l + "\n";
    return result;
}

std::string VirtualDisk::df_stats() const {
    uint64_t total = 0; int files = 0, dirs = 0;
    for (const auto& kv : files_) {
        if (kv.second.flags & kDirFlag) dirs++;
        else { files++; total += kv.second.data.size(); }
    }
    return "Files: " + std::to_string(files) + "  Dirs: " + std::to_string(dirs) +
           "  Size: " + std::to_string(total) + " bytes\n";
}

bool VirtualDisk::auth_user(const std::string& username, const std::string& password) {
    if (!mounted_) return false;
    auto it = files_.find("/etc/shadow");
    if (it == files_.end()) return false;
    std::string shadow(it->second.data.begin(), it->second.data.end());

    // Simple line-based parsing
    size_t pos = 0;
    while (pos < shadow.size()) {
        size_t nl = shadow.find('\n', pos);
        if (nl == std::string::npos) nl = shadow.size();
        std::string line = shadow.substr(pos, nl - pos);
        pos = nl + 1;
        if (line.empty()) continue;

        size_t c1 = line.find(':');
        if (c1 == std::string::npos) continue;
        std::string u = line.substr(0, c1);
        if (u != username) continue;

        size_t c2 = line.find(':', c1 + 1);
        if (c2 == std::string::npos) continue;
        std::string pw = line.substr(c1 + 1, c2 - c1 - 1);
        // Empty password matches anything, otherwise must match exactly
        return pw.empty() || pw == password;
    }
    return false;
}

bool VirtualDisk::user_exists(const std::string& username) const {
    auto it = files_.find("/etc/passwd");
    if (it == files_.end()) return false;
    std::string passwd(it->second.data.begin(), it->second.data.end());
    size_t pos = 0;
    while (pos < passwd.size()) {
        size_t nl = passwd.find('\n', pos);
        if (nl == std::string::npos) nl = passwd.size();
        std::string line = passwd.substr(pos, nl - pos);
        pos = nl + 1;
        if (line.compare(0, username.size(), username) == 0 &&
            line.size() > username.size() && line[username.size()] == ':')
            return true;
    }
    return false;
}

std::string VirtualDisk::get_first_user() const {
    auto it = files_.find("/etc/passwd");
    if (it == files_.end()) return "demi";
    std::string passwd(it->second.data.begin(), it->second.data.end());
    size_t pos = 0;
    while (pos < passwd.size()) {
        size_t nl = passwd.find('\n', pos);
        if (nl == std::string::npos) nl = passwd.size();
        std::string line = passwd.substr(pos, nl - pos);
        pos = nl + 1;
        size_t c1 = line.find(':');
        if (c1 == std::string::npos) continue;
        std::string user = line.substr(0, c1);
        if (user != "root") return user;
    }
    return "demi";
}

// --- Listing ---

int VirtualDisk::list_display_at(const std::string& prefix, char* buf, int buf_size) const {
    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/"; // root

    std::set<std::string> seen;
    int written = 0;

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        // Must start with our prefix
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;

        // Extract the first path component after the prefix
        std::string rest = name.substr(pfx.size());
        size_t slash_pos = rest.find('/');
        std::string child;
        bool is_dir = false;
        if (slash_pos == std::string::npos) {
            child = rest;
        } else {
            child = rest.substr(0, slash_pos);
            is_dir = true;
        }
        // Skip .dir markers
        if (child == ".dir") continue;
        if (seen.find(child) != seen.end()) continue;
        seen.insert(child);

        std::string display = child + (is_dir ? "/\n" : "\n");
        int needed = static_cast<int>(display.size());
        if (written + needed > buf_size) break;
        std::memcpy(buf + written, display.data(), needed);
        written += needed;
    }
    return written;
}

int VirtualDisk::list_display_cwd(char* buf, int buf_size) const {
    return list_display_at(cwd_, buf, buf_size);
}

int VirtualDisk::list_display_long(const std::string& prefix, char* buf, int buf_size) const {
    auto perm_str = [](uint16_t perm, bool dir) -> std::string {
        std::string s = dir ? "d" : ".";
        s += (perm & 0400) ? "r" : "-";
        s += (perm & 0200) ? "w" : "-";
        s += (perm & 0100) ? "x" : "-";
        s += (perm & 0040) ? "r" : "-";
        s += (perm & 0020) ? "w" : "-";
        s += (perm & 0010) ? "x" : "-";
        s += (perm & 0004) ? "r" : "-";
        s += (perm & 0002) ? "w" : "-";
        s += (perm & 0001) ? "x" : "-";
        return s;
    };
    auto fmt_size = [](uint64_t sz, bool dir) -> std::string {
        if (dir) return "    -";
        char tmp[16];
        snprintf(tmp, sizeof(tmp), "%5llu", (unsigned long long)sz);
        return tmp;
    };

    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/";
    std::set<std::string> seen;
    int written = 0;

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;
        std::string rest = name.substr(pfx.size());
        size_t slash_pos = rest.find('/');
        std::string child;
        bool is_dir = false;
        if (slash_pos == std::string::npos) { child = rest; }
        else { child = rest.substr(0, slash_pos); is_dir = true; }
        if (child == ".dir") continue;
        if (seen.find(child) != seen.end()) continue;
        seen.insert(child);

        std::string perms = perm_str(kv.second.permissions, is_dir);
        std::string sz = fmt_size(kv.second.data.size(), is_dir);
        std::string owner = kv.second.owner.empty() ? "demi" : kv.second.owner;
        if (owner.size() < 8) owner.append(8 - owner.size(), ' ');
        else if (owner.size() > 8) owner = owner.substr(0, 8);
        std::string display = perms + "  " + sz + "  " + owner + "  Jan 01 2024  "
                            + child + (is_dir ? "/" : "") + "\n";
        int needed = static_cast<int>(display.size());
        if (written + needed > buf_size) break;
        std::memcpy(buf + written, display.data(), needed);
        written += needed;
    }
    return written;
}

int VirtualDisk::list_display_human(const std::string& prefix, char* buf, int buf_size) const {
    auto human_size = [](uint64_t sz) -> std::string {
        const char* units[] = {"B", "K", "M", "G"};
        int ui = 0;
        double d = static_cast<double>(sz);
        while (d >= 1024.0 && ui < 3) { d /= 1024.0; ui++; }
        char tmp[16];
        if (ui == 0) snprintf(tmp, sizeof(tmp), "%4llu%s", (unsigned long long)sz, units[ui]);
        else snprintf(tmp, sizeof(tmp), "%4.1f%s", d, units[ui]);
        return tmp;
    };
    auto fmt_size = [&](uint64_t sz, bool dir) -> std::string {
        if (dir) return "    -";
        return human_size(sz);
    };
    auto perm_str = [](uint16_t p, bool d) -> std::string {
        std::string s = d ? "d" : ".";
        s += (p & 0400) ? "r" : "-"; s += (p & 0200) ? "w" : "-"; s += (p & 0100) ? "x" : "-";
        s += (p & 0040) ? "r" : "-"; s += (p & 0020) ? "w" : "-"; s += (p & 0010) ? "x" : "-";
        s += (p & 0004) ? "r" : "-"; s += (p & 0002) ? "w" : "-"; s += (p & 0001) ? "x" : "-";
        return s;
    };

    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/";
    std::set<std::string> seen;
    int written = 0;

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;
        std::string rest = name.substr(pfx.size());
        size_t slash_pos = rest.find('/');
        std::string child;
        bool is_dir = false;
        if (slash_pos == std::string::npos) { child = rest; }
        else { child = rest.substr(0, slash_pos); is_dir = true; }
        if (child == ".dir") continue;
        if (seen.find(child) != seen.end()) continue;
        seen.insert(child);

        std::string perms2 = perm_str(kv.second.permissions, is_dir);
        std::string sz2 = fmt_size(kv.second.data.size(), is_dir);
        std::string owner2 = kv.second.owner.empty() ? "demi" : kv.second.owner;
        if (owner2.size() < 8) owner2.append(8 - owner2.size(), ' ');
        std::string display = perms2 + "  " + sz2 + "  " + owner2 + "  Jan 01 2024  "
                            + child + (is_dir ? "/" : "") + "\n";
        int needed = static_cast<int>(display.size());
        if (written + needed > buf_size) break;
        std::memcpy(buf + written, display.data(), needed);
        written += needed;
    }
    return written;
}

int VirtualDisk::list_display_all(const std::string& prefix, char* buf, int buf_size) const {
    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/";
    std::set<std::string> seen;
    int written = 0;

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;
        std::string rest = name.substr(pfx.size());
        size_t slash_pos = rest.find('/');
        std::string child;
        bool is_dir = false;
        if (slash_pos == std::string::npos) { child = rest; }
        else { child = rest.substr(0, slash_pos); is_dir = true; }
        if (child == ".dir") {
            if (seen.find("./") != seen.end()) continue;
            seen.insert("./");
            std::string display = "./\n";
            int needed = static_cast<int>(display.size());
            if (written + needed > buf_size) break;
            std::memcpy(buf + written, display.data(), needed);
            written += needed;
        } else {
            if (seen.find(child) != seen.end()) continue;
            seen.insert(child);
            std::string display = child + (is_dir ? "/\n" : "\n");
            int needed = static_cast<int>(display.size());
            if (written + needed > buf_size) break;
            std::memcpy(buf + written, display.data(), needed);
            written += needed;
        }
    }
    return written;
}

int VirtualDisk::list_display_recursive(const std::string& prefix, char* buf, int buf_size) const {
    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/";
    int written = 0;

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;
        if (name.find("/.dir") != std::string::npos) continue;

        std::string display = name.substr(pfx.size());
        bool is_dir = (kv.second.flags & kDirFlag) != 0;
        if (is_dir) {
            // remove trailing /.dir
            size_t pos = display.rfind("/.dir");
            if (pos != std::string::npos) display = display.substr(0, pos) + "/";
        }
        display += "\n";
        int needed = static_cast<int>(display.size());
        if (written + needed > buf_size) break;
        std::memcpy(buf + written, display.data(), needed);
        written += needed;
    }
    return written;
}

int VirtualDisk::list_display(char* buf, int buf_size) const {
    return list_display_cwd(buf, buf_size);
}

int VirtualDisk::list(char* buf, int buf_size) const {
    int written = 0;
    for (const auto& kv : files_) {
        int needed = static_cast<int>(kv.first.size()) + 1;
        if (written + needed > buf_size) break;
        std::memcpy(buf + written, kv.first.c_str(), needed);
        written += needed;
    }
    return written;
}

// --- Host-side access ---

std::vector<uint8_t> VirtualDisk::read_file(const std::string& filename) const {
    auto it = files_.find(resolve_path(filename));
    if (it == files_.end()) return {};
    return it->second.data;
}

bool VirtualDisk::write_file(const std::string& filename, const std::vector<uint8_t>& data) {
    if (!mounted_) return false;
    std::string resolved = resolve_path(filename);
    FileEntry fe;
    fe.data = data;
    files_[resolved] = fe;
    return save();
}

bool VirtualDisk::delete_file(const std::string& filename) {
    return remove(filename) == 0;
}

std::vector<std::string> VirtualDisk::list_files() const {
    std::vector<std::string> names;
    for (const auto& kv : files_) names.push_back(kv.first);
    return names;
}

std::vector<std::string> VirtualDisk::list_dir(const std::string& prefix) const {
    std::vector<std::string> result;
    std::set<std::string> seen;
    std::string pfx = prefix;
    if (!pfx.empty() && pfx.back() != '/') pfx += '/';
    if (pfx == "/") pfx = "/";

    for (const auto& kv : files_) {
        const std::string& name = kv.first;
        if (name.size() <= pfx.size()) continue;
        if (name.compare(0, pfx.size(), pfx) != 0) continue;
        std::string rest = name.substr(pfx.size());
        size_t slash_pos = rest.find('/');
        std::string child = (slash_pos == std::string::npos) ? rest : rest.substr(0, slash_pos);
        if (child == ".dir") continue;
        if (seen.insert(child).second) result.push_back(child);
    }
    return result;
}

} // namespace sandbox
} // namespace demi
