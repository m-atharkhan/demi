#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "../../src/engine/engine.hpp"

// Utility function to load a binary file into a vector of bytes
std::vector<uint8_t> load_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + filepath);
    }
    
    return buffer;
}

int main(int argc, char* argv[]) {
    std::string hex_file = "hello.hex";
    if (argc > 1) {
        hex_file = argv[1];
    }

    std::cout << "==================================================\n";
    std::cout << " DemiEngine C++ Integration API Example\n";
    std::cout << "==================================================\n\n";

    try {
        // 1. Configure the virtual machine sandboxing and execution posture
        demi::Config config;
        config.enable_sandbox = true;         // Intercept and sanitize syscalls via native VFS
        config.strict_io = false;             // Allow generic stdout/stderr output redirection
        config.max_execution_ticks = 100000;  // Quota watchdog to prevent endless loops (DoS)
        config.memory_size = 1024 * 64;       // Allocate a light 64KB addressing space

        std::cout << "[Host] Creating Demi VM instance with sandbox enabled..." << std::endl;
        demi::Engine vm(config);

        // 2. Install stdio hooks to intercept program print statements
        std::cout << "[Host] Attaching STDOUT interception hook..." << std::endl;
        std::string captured_output;
        vm.set_stdout_hook([&captured_output](int fd, const std::vector<uint8_t>& data) {
            std::string text(data.begin(), data.end());
            captured_output += text;
            std::cout << "[VM STDOUT (fd=" << fd << ")] " << text;
        });

        // 3. (Optional) Install a syscall hook to demonstrate custom VM interception.
        // Returning true from the callback bypasses default sandbox handlers.
        std::cout << "[Host] Attaching Syscall Hook (monitoring INT 0x80)..." << std::endl;
        vm.set_syscall_hook([](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
            if (syscall_id == 1) { // sys_exit
                std::cout << "[Host Hook] Intercepted sys_exit with exit code: " << arg1 << std::endl;
                // Let the normal VM handler shut down execution naturally
                return false; 
            }
            if (syscall_id == 4) { // sys_write
                std::cout << "[Host Hook] Intercepted sys_write to fd=" << arg1 
                          << ", buffer=0x" << std::hex << arg2 << ", size=" << std::dec << arg3 << std::endl;
                // Return false to let the built-in sandbox process it (and send bytes to stdout hooks)
                return false;
            }
            return false;
        });

        // 4. Load the compiled assembly bytecode
        std::cout << "[Host] Loading bytecode file: " << hex_file << "..." << std::endl;
        std::vector<uint8_t> bytecode = load_binary_file(hex_file);
        std::cout << "[Host] Loaded " << bytecode.size() << " bytes of executable binary" << std::endl;

        if (!vm.load_executable(bytecode, 0x0)) {
            std::cerr << "[Host ERROR] Failed to load binary executable into VM address space!" << std::endl;
            return 1;
        }

        // 5. Run the program until completion
        std::cout << "[Host] Initiating bytecode execution...\n" << std::endl;
        demi::Engine::CompleteReason reason = vm.run();
        std::cout << "\n[Host] VM execution stopped." << std::endl;

        // 6. Report execution outcomes
        std::cout << "==================================================" << std::endl;
        std::cout << "Execution Summary:" << std::endl;
        std::cout << "  Complete Reason: ";
        switch (reason) {
            case demi::Engine::CompleteReason::FINISHED:
                std::cout << "FINISHED (Success / halt)" << std::endl;
                break;
            case demi::Engine::CompleteReason::WATCHDOG_TIMEOUT:
                std::cout << "WATCHDOG_TIMEOUT (Execution limit reached)" << std::endl;
                break;
            case demi::Engine::CompleteReason::SECURITY_VIOLATION:
                std::cout << "SECURITY_VIOLATION (Forbidden operation)" << std::endl;
                break;
            case demi::Engine::CompleteReason::EXCEPTION:
                std::cout << "EXCEPTION (Instruction faulted or divide by zero)" << std::endl;
                break;
        }
        std::cout << "  Captured Output: \"" << captured_output << "\"" << std::endl;
        std::cout << "==================================================" << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "[Host EXCEPTION] " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
