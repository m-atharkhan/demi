#include "vcmpgt.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
#include "../cpu_registers.hpp"
#include <fmt/format.h>

using namespace DemiEngine_Registers;

void handle_VCMPGT(CPU& cpu, const std::vector<uint8_t>& program, bool& running) {
    if (cpu.get_pc() + 2 >= program.size()) {
        Logger::instance().error() << "VCMPGT: Insufficient instruction bytes" << std::endl;
        running = false;
        return;
    }

    uint8_t dest_reg = program[cpu.get_pc() + 1];
    uint8_t src_reg = program[cpu.get_pc() + 2];
    
    // Compare R0-R3 > R4-R7 element-wise
    auto& regs = cpu.get_registers(); // Use legacy register system like LOAD_IMM
    
    uint32_t a0 = regs[static_cast<uint8_t>(R0)];
    uint32_t a1 = regs[static_cast<uint8_t>(R1)];
    uint32_t a2 = regs[static_cast<uint8_t>(R2)];
    uint32_t a3 = regs[static_cast<uint8_t>(R3)];
    
    uint32_t b0 = regs[static_cast<uint8_t>(R4)];
    uint32_t b1 = regs[static_cast<uint8_t>(R5)];
    uint32_t b2 = regs[static_cast<uint8_t>(R6)];
    uint32_t b3 = regs[static_cast<uint8_t>(R7)];
    
    // Compare and set result (0xFF for true, 0x00 for false)
    uint32_t result0 = (a0 > b0) ? 0xFF : 0x00;
    uint32_t result1 = (a1 > b1) ? 0xFF : 0x00;
    uint32_t result2 = (a2 > b2) ? 0xFF : 0x00;
    uint32_t result3 = (a3 > b3) ? 0xFF : 0x00;
    
    // Store results back to R0-R3
    regs[static_cast<uint8_t>(R0)] = result0;
    regs[static_cast<uint8_t>(R1)] = result1;
    regs[static_cast<uint8_t>(R2)] = result2;
    regs[static_cast<uint8_t>(R3)] = result3;
    
    Logger::instance().debug() << fmt::format("VCMPGT: Vector compare [{},{},{},{}] > [{},{},{},{}] = [{},{},{},{}]",
        a0, a1, a2, a3, b0, b1, b2, b3, result0, result1, result2, result3) << std::endl;
    
    cpu.set_pc(cpu.get_pc() + 1);
}