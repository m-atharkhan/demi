#include <iostream>
#include <vector>

#include "../../../src/engine/engine.hpp"

int main() {
    demi::Engine vm;

    // Minimal program: HALT
    std::vector<uint8_t> program = { 0xFF };
    if (!vm.load_executable(program, 0x1000)) {
        std::cerr << "load failed: " << vm.last_error() << "\n";
        return 1;
    }

    std::cout << "PC before: 0x" << std::hex << vm.get_pc() << std::dec << "\n";

    uint64_t rax = 0;
    if (vm.get_register_u64(0 /* RAX */, rax)) {
        std::cout << "RAX before: " << rax << "\n";
    }

    // Step once (HALT), then introspect again.
    vm.step();

    std::cout << "PC after: 0x" << std::hex << vm.get_pc() << std::dec << "\n";

    uint32_t flags = 0;
    if (vm.get_flags(flags)) {
        std::cout << "FLAGS: 0x" << std::hex << flags << std::dec << "\n";
    }

    std::cout << "ticks: " << vm.get_tick_count() << "\n";
    return 0;
}
