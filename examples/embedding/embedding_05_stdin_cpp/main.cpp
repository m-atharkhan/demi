#include <demi/engine.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static std::vector<uint8_t> read_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        throw std::runtime_error("failed to open: " + path);
    }
    const std::streamsize n = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> bytes(static_cast<size_t>(n));
    if (n > 0) {
        // Safe: bytes is sized to n, reading exactly n bytes
        if (!f.read(reinterpret_cast<char*>(bytes.data()), n)) {
            throw std::runtime_error("failed to read: " + path);
        }
    }
    return bytes;
}

int main(int argc, char** argv) {
    constexpr uint32_t kEchoInputEntryPc = 0x200;

    bool debug = false;
    std::string program_path = "echo_input.hex";
    std::string host_input = "Input from C++ host stdin hook\n";
    int positional = 0;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--debug") {
            debug = true;
            continue;
        }
        if (positional == 0) {
            program_path = arg;
        } else if (positional == 1) {
            host_input = arg;
        }
        positional++;
    }

    demi::Engine vm;
    vm.enable_output_capture(true);

    vm.set_stdout_hook([](int fd, const std::vector<uint8_t>& data) {
        (void)fd;
        std::cout << std::string(data.begin(), data.end());
    });
    if (debug) {
        vm.clear_stdout_hook();
        vm.set_stdout_hook([](int fd, const std::vector<uint8_t>& data) {
            std::cout << "[guest fd=" << fd << "] "
                      << std::string(data.begin(), data.end());
        });
    }

    bool stdin_sent = false;
    vm.set_stdin_hook([&](size_t max_count, std::vector<uint8_t>& data) {
        if (stdin_sent || host_input.empty()) {
            data.clear();
            return;
        }
        const size_t n = std::min(max_count, host_input.size());
        data.assign(host_input.begin(), host_input.begin() + static_cast<std::ptrdiff_t>(n));
        stdin_sent = true;
    });

    const auto program = read_file(program_path);
    if (!vm.load_executable(program, 0)) {
        std::cerr << "load failed: (" << vm.last_error_code() << ") "
                  << vm.last_error() << "\n";
        return 1;
    }
    // echo_input.asm uses .data/.text; entry symbol _start is at 0x200.
    if (!vm.set_pc(kEchoInputEntryPc)) {
        std::cerr << "set_pc failed\n";
        return 1;
    }

    (void)vm.run();

    const auto& captured = vm.get_output_buffer();
    if (debug) {
        std::cout << "\n[captured " << captured.size() << " bytes] "
                  << std::string(captured.begin(), captured.end()) << "\n";
    }
    return 0;
}

