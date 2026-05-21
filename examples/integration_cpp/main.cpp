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
    bool debug_mode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--debug") debug_mode = true;
        else hex_file = argv[i];
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

        if (debug_mode) {
            std::cout << "[DEBUG] Debug mode enabled.\n";
        }

        // 2. Install stdio hooks to intercept program print statements
        std::cout << "[Host] Attaching STDOUT interception hook..." << std::endl;
        std::string captured_output;
        vm.set_stdout_hook([&captured_output](int fd, const std::vector<uint8_t>& data) {
            std::string text(data.begin(), data.end());
            captured_output += text;
            std::cout << "[DEBUG][main.cpp] STDOUT hook called: fd=" << fd << ", size=" << data.size() << ", hex=";
            for (auto b : data) std::cout << std::hex << (int)b << " ";
            std::cout << std::dec << std::endl;
            std::cout << "[VM STDOUT (fd=" << fd << ")] " << text;
        });

        // 3. (Optional) Install a syscall hook to demonstrate custom VM interception.
        // Returning true from the callback bypasses default sandbox handlers.
        std::cout << "[Host] Attaching Syscall Hook (monitoring INT 0x80)..." << std::endl;
        vm.set_syscall_hook([&vm, &captured_output](uint32_t syscall_id, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, int32_t& result) -> bool {
    
            if (syscall_id == 4) { // sys_write (RAX=4, RBX=fd, RCX=buffer, RDX=size)
                uint32_t fd = arg1;
                uint32_t buffer_addr = arg2;
                uint32_t size = arg3;

                // 1. Extract bytes directly from the VM's memory space using peek_memory
                std::vector<uint8_t> extracted_bytes;
                for (uint32_t i = 0; i < size; ++i) {
                    extracted_bytes.push_back(vm.peek_memory(buffer_addr + i));
                }

                // 2. Append directly to our local captured string
                std::string text(extracted_bytes.begin(), extracted_bytes.end());
                captured_output += text;

                std::cout << "[VM STDOUT (fd=" << fd << ")] " << text;

                result = size; // Return number of bytes written
                return true;   // Bypass internal processing!
            }

            if (syscall_id == 1) { // sys_exit
                std::cout << "[Host Hook] Intercepted sys_exit with exit code: " << arg1 << std::endl;
                result = 0;
                return true;   // Bypass internal processing and stop gracefully
            }

            return false;
        });

        // 4. Load the compiled assembly bytecode
        std::cout << "[Host] Loading bytecode file: " << hex_file << "..." << std::endl;
        std::vector<uint8_t> bytecode = load_binary_file(hex_file);
        std::cout << "[Host] Loaded " << bytecode.size() << " bytes of executable binary" << std::endl;

        // Load at 0x1000 to match Demi engine's default entry point
        constexpr uint64_t ENTRY_POINT = 0x1000;
        if (!vm.load_executable(bytecode, ENTRY_POINT)) {
            std::cerr << "[Host ERROR] Failed to load binary executable into VM address space at 0x1000!" << std::endl;
            return 1;
        }

        // Debug: Print first 32 bytes at 0x1000
        if (debug_mode) {
            std::cout << "[DEBUG] First 32 bytes at 0x1000: ";
            // Access VM memory via reflection (assume get_memory() exists or expose it for debug)
            // This is a workaround: re-load the binary and print it
            for (size_t i = 0; i < std::min<size_t>(32, bytecode.size()); ++i) {
                printf("%02X ", bytecode[i]);
            }
            std::cout << std::endl;
        }

        // 5. Run the program until completion
        std::cout << "[Host] Initiating bytecode execution...\n" << std::endl;
        demi::Engine::CompleteReason reason;
        if (!debug_mode) {
            reason = vm.run();
        } else {
            // Manual instruction stepping with debug output
            size_t max_steps = 100000;
            size_t steps = 0;
            bool running = true;
            std::cout << "[DEBUG] Starting instruction trace...\n";
            while (running && steps < max_steps) {
                uint32_t pc = vm.get_pc();
                uint8_t opcode = vm.peek_memory(pc);
                std::cout << "[DEBUG][step] Step " << steps << " PC=0x" << std::hex << pc << " opcode=0x" << (int)opcode << std::dec;
                running = vm.tick();
                std::cout << " (tick returned: " << (running ? "true" : "false") << ")";
                std::cout << std::endl;
                ++steps;
            }
            // After first step, check for errors (pseudo, as API allows)
            std::cout << "[DEBUG] After execution: PC=0x" << std::hex << vm.get_pc() << std::dec << std::endl;
            std::cout << "[DEBUG] Instruction trace complete after " << steps << " steps.\n";
            reason = demi::Engine::CompleteReason::FINISHED; // Placeholder, real reason not available from manual stepping
        }
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
