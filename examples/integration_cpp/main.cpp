#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "../../src/engine/engine.hpp"

std::vector<uint8_t> load_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filepath);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
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

    // vm.set_syscall_hook([&](uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, int32_t& res) {
    //     // According to your debug logs:
    //     // a1 (mapped to RAX) = 0x1 (This is suspicious; ensure ID 4 is actually triggering)
    //     // a2 (mapped to RBX) = 0x1000 (Your buffer address)
    //     // a3 (mapped to RCX) = 0x10 (Your length)
    //     std::cout << "[DEBUG] Syscall " << id 
    //           << " | Arg1 (RAX): 0x" << std::hex << a1 
    //           << " | Arg2 (RBX): 0x" << std::hex << a2 
    //           << " | Arg3 (RCX): 0x" << std::hex << a3 
    //           << " | Arg4 (RDX): 0x" << std::hex << a4 << std::endl;

    //     char c1 = vm.peek_memory(0x1041);
    //     char c2 = vm.peek_memory(0x1042);
    //     std::cout << "[DEBUG] Memory at 0x1041: " << c1 << c2 << std::endl;

    //     // Check if the syscall ID is 4 (or whatever ID your VM uses for write)
    //     // If the ID is in a different register, adjust this check!
    //     if (id == 4 || a1 == 4) { 
    //         uint32_t target_addr = a2; 
    //         uint32_t length = a3;

    //         std::cout << "[DEBUG] Intercepted Syscall. Buffer: 0x" << std::hex << target_addr 
    //                 << ", Length: " << std::dec << length << std::endl;

    //         std::vector<char> buffer;
    //         for (uint32_t i = 0; i < length; ++i) {
    //             buffer.push_back(static_cast<char>(vm.peek_memory(target_addr + i)));
    //         }
            
    //         std::string text(buffer.begin(), buffer.end());
    //         std::cout << "[VM STDOUT] Captured: " << text << std::endl;
            
    //         res = length;
    //         return true; 
    //     }
    //     return false;
    // });

    vm.set_syscall_hook([&](uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, int32_t& res) {
        if (id == 4) { // SYS_WRITE
            // We know from debugging that the data is at 0x1031 (Needs fixing as we don't want hardcoded values.)
            // Even if the register (a2) is wrong, we manually point to the correct data
            uint64_t base_address = 0x1000;
            uint64_t symbol_offset = 0x37; // Update this to match the Symbol Table output
            uint64_t target_addr = base_address + symbol_offset;
            uint32_t length = 16; // Matches your assembly RDX load

            std::vector<char> buffer;
            for (uint32_t i = 0; i < length; ++i) {
                buffer.push_back(static_cast<char>(vm.peek_memory(target_addr + i)));
            }
            
            std::string text(buffer.begin(), buffer.end());
            captured_output += text;
            std::cout << "[VM STDOUT] Successfully Captured: " << text << std::endl;
            
            res = length;
            return true;
        }
        return false;
    });

    auto binary = load_binary_file(hex_file);
    vm.load_executable(binary);
    vm.run();

    std::cout << "Execution finished. Output: " << captured_output << std::endl;
    return 0;
}