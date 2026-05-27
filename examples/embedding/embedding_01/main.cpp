#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "../../../src/engine/engine.hpp"

static std::vector<uint8_t> load_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filepath);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

int main(int argc, char* argv[]) {
    std::string hex_file = "hello.hex";
    for (int i = 1; i < argc; ++i) hex_file = argv[i];

    demi::Engine vm;
    std::string captured_output;

    vm.set_stdout_hook([&](int fd, const std::vector<uint8_t>& data) {
        std::string text(data.begin(), data.end());
        captured_output += text;
        std::cout << "[VM STDOUT (fd=" << fd << ")] " << text;
    });

    auto binary = load_binary_file(hex_file);
    // Load program at base address 0 so that label-based addresses
    // in the assembled binary (e.g., `msg`) line up directly with
    // the VM's linear memory for syscalls like sys_write.
    if (!vm.load_executable(binary, 0)) {
        std::cerr << "load failed: (" << vm.last_error_code() << ") " << vm.last_error() << std::endl;
        return 1;
    }
    vm.run();

    std::cout << "Execution finished. Output: " << captured_output << std::endl;
    return 0;
}
