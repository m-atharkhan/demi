#include "test_framework.hpp"
#include "../engine/vfs.hpp"
#include "../engine/virtual_disk.hpp"

#include <fstream>
#include <cstdio>

TEST_CASE(vfs_unrestricted_passthrough, "vfs") {
    demi::sandbox::VirtualFileSystem vfs("/tmp/jail", false);
    auto result = vfs.resolve_safe_path("/etc/hostname");
    if (!result.has_value()) throw AssertionFailure("unrestricted VFS should allow any path");
}

TEST_CASE(vfs_restricted_allows_in_jail, "vfs") {
    std::string jail = "/tmp/demi_vfs_test_jail";
    std::filesystem::create_directories(jail + "/subdir");
    std::ofstream(jail + "/test.txt").close();
    std::ofstream(jail + "/subdir/nested.txt").close();

    demi::sandbox::VirtualFileSystem vfs(jail, true);
    if (!vfs.resolve_safe_path("test.txt").has_value())
        throw AssertionFailure("should resolve file inside jail");
    if (!vfs.resolve_safe_path("subdir/nested.txt").has_value())
        throw AssertionFailure("should resolve nested file inside jail");

    std::filesystem::remove_all(jail);
}

TEST_CASE(vfs_restricted_blocks_traversal, "vfs") {
    std::string jail = "/tmp/demi_vfs_test_jail2";
    std::filesystem::create_directories(jail);
    std::filesystem::create_directories(jail + "/a/b");

    demi::sandbox::VirtualFileSystem vfs(jail, true);
    if (vfs.resolve_safe_path("../../../etc/passwd").has_value())
        throw AssertionFailure("path traversal should be blocked");
    if (!vfs.resolve_safe_path("a/../test.txt").has_value())
        throw AssertionFailure("dot-dot inside jail should be allowed");

    std::filesystem::remove_all(jail);
}

TEST_CASE(vfs_restricted_absolute_stripped, "vfs") {
    std::string jail = "/tmp/demi_vfs_test_jail3";
    std::filesystem::create_directories(jail);
    std::ofstream(jail + "/foo.txt").close();

    demi::sandbox::VirtualFileSystem vfs(jail, true);
    auto result = vfs.resolve_safe_path("/foo.txt");
    if (!result.has_value())
        throw AssertionFailure("absolute path within jail should resolve");

    std::filesystem::remove_all(jail);
}

static std::string make_container(const char* name) {
    std::string path = std::string("/tmp/") + name;
    std::remove(path.c_str());
    return path;
}

TEST_CASE(vdisk_init_and_mount, "vfs") {
    std::string c = make_container("demi_test_vdisk.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();
    if (vd.needs_init()) throw AssertionFailure("after init, should not need init");
    if (!vd.is_mounted()) throw AssertionFailure("should be mounted");
    if (!vd.save()) throw AssertionFailure("save should succeed");
    std::ifstream check(c, std::ios::binary);
    if (!check.good()) throw AssertionFailure("container file should exist after save");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_write_and_read, "vfs") {
    std::string c = make_container("demi_test_vdisk2.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    std::string content = "Hello, VirtualDisk!";
    std::vector<uint8_t> data(content.begin(), content.end());
    if (!vd.write_file("hello.txt", data))
        throw AssertionFailure("write_file should succeed");
    if (!vd.exists("hello.txt"))
        throw AssertionFailure("file should exist after write");

    auto readback = vd.read_file("hello.txt");
    std::string read_str(readback.begin(), readback.end());
    if (read_str != content)
        throw AssertionFailure("read_file content mismatch");
    if (vd.size("hello.txt") != static_cast<int>(content.size()))
        throw AssertionFailure("size mismatch");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_list_files, "vfs") {
    std::string c = make_container("demi_test_vdisk3.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    vd.write_file("a.txt", {1, 2, 3});
    vd.write_file("b.txt", {4, 5});

    auto files = vd.list_files();
    if (files.size() < 2) throw AssertionFailure("should have at least 2 files");

    bool has_a = false, has_b = false;
    for (const auto& f : files) {
        if (f == "a.txt") has_a = true;
        if (f == "b.txt") has_b = true;
    }
    if (!has_a || !has_b) throw AssertionFailure("list_files missing entries");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_delete_file, "vfs") {
    std::string c = make_container("demi_test_vdisk4.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    vd.write_file("/temp.txt", {1, 2, 3});
    if (!vd.exists("/temp.txt")) throw AssertionFailure("file should exist before delete");
    if (!vd.delete_file("/temp.txt"))
        throw AssertionFailure("delete_file should succeed on resolved path");
    if (vd.exists("/temp.txt"))
        throw AssertionFailure("file should not exist after delete");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_persistence, "vfs") {
    std::string c = make_container("demi_test_vdisk5.vfs");
    {
        demi::sandbox::VirtualDisk vd;
        vd.mount(c);
        vd.init_filesystem();
        vd.write_file("persist.txt", {'P', 'E', 'R', 'S', 'I', 'S', 'T'});
        vd.save();
    }
    {
        demi::sandbox::VirtualDisk vd;
        if (!vd.mount(c)) throw AssertionFailure("remount should succeed");
        if (!vd.exists("persist.txt")) throw AssertionFailure("persisted file should exist");
        auto data = vd.read_file("persist.txt");
        std::string s(data.begin(), data.end());
        if (s != "PERSIST") throw AssertionFailure("persisted data mismatch");
    }
    std::remove(c.c_str());
}

TEST_CASE(vdisk_directory_operations, "vfs") {
    std::string c = make_container("demi_test_vdisk6.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    if (!vd.mkdir("mydir")) throw AssertionFailure("mkdir should succeed");
    if (!vd.mkdir_p("a/b/c")) throw AssertionFailure("mkdir_p should succeed");

    vd.write_file("/a/b/c/nested.txt", {'X'});
    if (!vd.exists("/a/b/c/nested.txt")) throw AssertionFailure("nested file should exist");

    if (!vd.rmdir("/a")) throw AssertionFailure("rmdir should succeed");
    if (vd.exists("/a/b/c/nested.txt")) throw AssertionFailure("file gone after rmdir");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_overwrite_file, "vfs") {
    std::string c = make_container("demi_test_vdisk7.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    vd.write_file("overwrite.txt", {'A', 'A', 'A'});
    vd.write_file("overwrite.txt", {'B', 'B'});

    auto data = vd.read_file("overwrite.txt");
    if (data.size() != 2) throw AssertionFailure("overwritten file wrong size");
    if (data[0] != 'B' || data[1] != 'B') throw AssertionFailure("overwritten content mismatch");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_empty_file, "vfs") {
    std::string c = make_container("demi_test_vdisk8.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    vd.write_file("empty.txt", {});
    if (!vd.exists("empty.txt")) throw AssertionFailure("empty file should exist");
    if (!vd.read_file("empty.txt").empty()) throw AssertionFailure("empty file should be empty");
    if (vd.size("empty.txt") != 0) throw AssertionFailure("empty file size should be 0");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_nonexistent_read, "vfs") {
    std::string c = make_container("demi_test_vdisk9.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    if (!vd.read_file("no_such_file.txt").empty())
        throw AssertionFailure("reading non-existent file should return empty");
    if (vd.exists("no_such_file.txt"))
        throw AssertionFailure("non-existent file should not exist");
    std::remove(c.c_str());
}

TEST_CASE(vdisk_large_file, "vfs") {
    std::string c = make_container("demi_test_vdisk11.vfs");
    demi::sandbox::VirtualDisk vd;
    vd.mount(c);
    vd.init_filesystem();

    std::vector<uint8_t> large(16384);
    for (size_t i = 0; i < large.size(); i++) large[i] = static_cast<uint8_t>(i & 0xFF);
    vd.write_file("large.bin", large);

    auto readback = vd.read_file("large.bin");
    if (readback.size() != large.size()) throw AssertionFailure("large file size mismatch");
    std::remove(c.c_str());
}
