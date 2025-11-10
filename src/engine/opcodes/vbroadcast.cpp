#include "vbroadcast.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_VBROADCAST(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 1 >= program.size()) {
        Logger::instance().error() << "VBROADCAST: Insufficient instruction bytes" << std::endl;
        running = false;
        return;
    }

    // Broadcast value from R0 to R4, R5, R6, R7
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t value = regs[static_cast<uint8_t>(R0)];
    
    regs[static_cast<uint8_t>(R4)] = value;
    regs[static_cast<uint8_t>(R5)] = value;
    regs[static_cast<uint8_t>(R6)] = value;
    regs[static_cast<uint8_t>(R7)] = value;
    
    Logger::instance().debug() << fmt::format("VBROADCAST: Broadcasting {} to R4,R5,R6,R7", value) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}