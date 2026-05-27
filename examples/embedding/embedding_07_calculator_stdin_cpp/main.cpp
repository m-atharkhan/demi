#include <demi/engine.hpp>

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
    if (!f.read(reinterpret_cast<char*>(bytes.data()), n)) {
        throw std::runtime_error("failed to read: " + path);
    }
    return bytes;
}

int main(int argc, char** argv) {
    constexpr uint32_t kLineCalculatorEntryPc = 0x200;

    bool debug = false;
    std::string program_path = "line_calculator.hex";
    int positional = 0;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--debug") {
            debug = true;
            continue;
        }
        if (positional == 0) {
            program_path = arg;
        }
        positional++;
    }

    demi::Engine vm;

    vm.set_stdout_hook([&](int fd, const std::vector<uint8_t>& data) {
        if (debug) {
            std::cout << "[guest fd=" << fd << "] "
                      << std::string(data.begin(), data.end());
        } else {
            std::cout << std::string(data.begin(), data.end());
        }
        std::cout.flush();
    });

    vm.set_stdin_hook([](size_t max_count, std::vector<uint8_t>& data) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            data.clear();  // EOF => guest sees read(0)
            return;
        }
        line.push_back('\n');
        if (line.size() > max_count) {
            line.resize(max_count);
        }
        data.assign(line.begin(), line.end());
    });

    const auto program = read_file(program_path);
    if (!vm.load_executable(program, 0)) {
        std::cerr << "load failed: (" << vm.last_error_code() << ") "
                  << vm.last_error() << "\n";
        return 1;
    }

    if (!vm.set_pc(kLineCalculatorEntryPc)) {
        std::cerr << "set_pc failed\n";
        return 1;
    }

    (void)vm.run();
    return 0;
}

