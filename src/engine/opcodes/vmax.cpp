#include "vmax.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>
#include <algorithm>

using namespace DemiEngine_Registers;

void handle_VMAX(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        Logger::instance().error() << "VMAX: Insufficient instruction bytes" << std::endl;
        running = false;
        return;
    }

    // Find maximum in vector R0-R3
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t a0 = regs[static_cast<uint8_t>(R0)];
    uint32_t a1 = regs[static_cast<uint8_t>(R1)];
    uint32_t a2 = regs[static_cast<uint8_t>(R2)];
    uint32_t a3 = regs[static_cast<uint8_t>(R3)];
    
    uint32_t max_val = std::max({a0, a1, a2, a3});
    
    // Store result in R0
    regs[static_cast<uint8_t>(R0)] = max_val;
    
    Logger::instance().debug() << fmt::format("VMAX: Maximum of [{},{},{},{}] = {}",
        a0, a1, a2, a3, max_val) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}