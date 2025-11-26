#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <fmt/core.h>

#include "cpu.hpp"
#include "cpu_flags.hpp"
#include "../debug/logger.hpp"
#include "../debug/debug_handler.hpp"
#include <iostream>
#include <iomanip>

#include "cpu_registers.hpp"  // Include the new register system
#include "branch_prediction.hpp"  // Include branch prediction
// #include "speculative_execution.hpp"  // Include speculative execution (temporarily disabled)
#include "../config.hpp"  // Include config for quiet mode check
#include "opcodes/opcode_dispatcher.hpp"
#include "opcodes/opcode_dispatcher_threaded.hpp"
#include "opcodes/opcode_dispatcher_unified.hpp"  // Add unified dispatcher
#include "opcodes/opcode_dispatcher_inlined.hpp"  // Add optimized inlined dispatcher
#include "opcodes/opcode_dispatcher_predictive.hpp"  // Add branch predictive dispatcher
#include "opcodes/instruction_fusion.hpp"  // Instruction fusion optimizer

using namespace DemiEngine_Registers;

// Constants for CPU configuration
constexpr size_t CPU_LEGACY_REGISTER_COUNT = 8;  // For backward compatibility
constexpr size_t CPU_DEFAULT_MEMORY_SIZE = 1024 * 1024; // 1MB default (massive increase from 256 bytes)
constexpr size_t CPU_TEST_MEMORY_SIZE = 256; // Maintain old size for test compatibility
constexpr size_t CPU_MIN_MEMORY_SIZE = 256; // Allow 256 bytes minimum for test compatibility
constexpr size_t CPU_MAX_MEMORY_SIZE = 64 * 1024 * 1024; // 64MB maximum for performance
const uint32_t INVALID_ADDR = static_cast<uint32_t>(-1);

// Standalone function to compute valid instruction starts
std::unordered_set<size_t> compute_valid_instruction_starts(const std::vector<uint8_t>& program) {
    std::unordered_set<size_t> starts;
    size_t pc = 0;
    while (pc < program.size()) {
        starts.insert(static_cast<size_t>(pc));
        Opcode opcode = static_cast<Opcode>(program[pc]);
        switch (opcode) {
            case Opcode::LOAD_IMM:
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MOV:
            case Opcode::LOAD:
            case Opcode::STORE:
            case Opcode::CMP:
            case Opcode::MUL:
            case Opcode::DIV:
            case Opcode::AND:
            case Opcode::OR:
            case Opcode::XOR:
            case Opcode::SHL:
            case Opcode::SHR:
            case Opcode::IN:
            case Opcode::OUT:
            case Opcode::INB:
            case Opcode::OUTB:
            case Opcode::INW:
            case Opcode::OUTW:
            case Opcode::INL:
            case Opcode::OUTL:
            case Opcode::INSTR:
            case Opcode::OUTSTR:
            case Opcode::ADD64:
            case Opcode::SUB64:
            case Opcode::MOV64:
            case Opcode::MUL64:
            case Opcode::DIV64:
            case Opcode::AND64:
            case Opcode::OR64:
            case Opcode::XOR64:
            case Opcode::SHL64:
            case Opcode::SHR64:
            case Opcode::CMP64:
            case Opcode::MOVEX:
            case Opcode::ADDEX:
            case Opcode::SUBEX:
            case Opcode::MULEX:
            case Opcode::DIVEX:
            case Opcode::CMPEX:
            case Opcode::LOADEX:
            case Opcode::STOREX:
            case Opcode::PUSHEX:
            case Opcode::POPEX:
                pc += 3;
                break;
            case Opcode::JMP:
            case Opcode::JZ:
            case Opcode::JNZ:
            case Opcode::JS:
            case Opcode::JNS:
            case Opcode::JC:
            case Opcode::JNC:
            case Opcode::JO:
            case Opcode::JNO:
            case Opcode::PUSH:
            case Opcode::POP:
            case Opcode::INC:
            case Opcode::DEC:
            case Opcode::NOT:
            case Opcode::CALL:
            case Opcode::PUSH_ARG:
            case Opcode::POP_ARG:
                pc += 2;
                break;
            case Opcode::HALT:
            case Opcode::NOP:
            case Opcode::RET:
            case Opcode::PUSH_FLAG:
            case Opcode::POP_FLAG:
            case Opcode::MODE32:
            case Opcode::MODE64:
                pc += 1;
                break;
            case Opcode::LOAD_IMM64:
                pc += 10; // opcode + reg + 8-byte immediate
                break;
            case Opcode::MODECMP:
                pc += 2; // opcode + mode value
                break;
            case Opcode::DB:
                if (pc + 2 < program.size()) {
                    uint8_t length = program[pc + 2];
                    pc += 3 + length;
                } else {
                    pc += 1;
                }
                break;
            default:
                pc += 1;
                break;
        }
    }
    return starts;
}

// CPU constructor
CPU::CPU(size_t memory_size)
    : cpu_mode(CPUMode::MODE_32BIT), // Default to 32-bit mode for backward compatibility
      registers(TOTAL_REGISTERS, 0), legacy_registers(CPU_LEGACY_REGISTER_COUNT, 0) {

    // Determine actual memory size to use
    if (memory_size == 0) {
        memory_size = CPU_DEFAULT_MEMORY_SIZE; // Use default 1MB
    }

    // Validate memory size bounds
    if (memory_size < CPU_MIN_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Memory size {} bytes is below minimum {}, using minimum",
            memory_size, CPU_MIN_MEMORY_SIZE) << std::endl;
        memory_size = CPU_MIN_MEMORY_SIZE;
    }
    if (memory_size > CPU_MAX_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Memory size {} bytes exceeds maximum {}, using maximum",
            memory_size, CPU_MAX_MEMORY_SIZE) << std::endl;
        memory_size = CPU_MAX_MEMORY_SIZE;
    }

    // Initialize memory
    memory.resize(memory_size, 0);

    // Initialize special registers
    registers[static_cast<size_t>(Register::RIP)] = 0;  // Program counter
    registers[static_cast<size_t>(Register::RSP)] = memory.size();  // Stack pointer
    registers[static_cast<size_t>(Register::RBP)] = memory.size();  // Frame pointer
    registers[static_cast<size_t>(Register::RFLAGS)] = 0;  // Flags

    arg_offset = 0;
    last_accessed_addr = INVALID_ADDR;
    last_modified_addr = INVALID_ADDR;

    // Sync legacy registers for backward compatibility
    sync_legacy_registers();

    // Only log CPU initialization if not in test mode
    if (!Config::test_mode) {
        Logger::instance().info() << fmt::format(
            "Virtual CPU initialized with {} bytes ({:.1f}MB) of memory and {} total registers",
            memory.size(), memory.size() / (1024.0 * 1024.0), TOTAL_REGISTERS) << std::endl;
    }
}

CPU::~CPU() = default;

// Extended register access methods
uint64_t CPU::get_register(Register reg) const {
    auto index = static_cast<size_t>(reg);
    if (index < TOTAL_REGISTERS) {
        return registers[index];
    }
    return 0;
}

void CPU::set_register(Register reg, uint64_t value) {
    auto index = static_cast<size_t>(reg);
    if (index < TOTAL_REGISTERS) {
        uint64_t old_value = registers[index];
        registers[index] = value;
        
        // Only sync legacy registers if we're modifying a legacy register (R0-R7)
        // This prevents FPU/extended register updates from corrupting legacy registers
        if (index < CPU_LEGACY_REGISTER_COUNT) {
            sync_legacy_registers();
        }

        // Print register update if value changed
        if (old_value != value) {
            print_register_update(reg, old_value, value);
        }
    }
}

// Get register name for debugging
std::string CPU::get_register_name(Register reg) const {
    return RegisterNames::get_name(reg);
}

// Synchronize legacy 32-bit registers with new 64-bit registers (for backward compatibility)
void CPU::sync_legacy_registers() {
    for (size_t i = 0; i < CPU_LEGACY_REGISTER_COUNT && i < TOTAL_REGISTERS; ++i) {
        legacy_registers[i] = static_cast<uint32_t>(registers[i]);
    }
}

// Synchronize from legacy registers to new registers (when legacy code modifies registers)
void CPU::sync_from_legacy_registers() {
    for (size_t i = 0; i < CPU_LEGACY_REGISTER_COUNT && i < TOTAL_REGISTERS; ++i) {
        // Only update the lower 32 bits, preserve upper 32 bits
        registers[i] = (registers[i] & 0xFFFFFFFF00000000ULL) | legacy_registers[i];
    }
}

// Factory method for test compatibility - creates CPU with old memory size
CPU CPU::create_test_cpu() {
    return CPU(CPU_TEST_MEMORY_SIZE);
}

// Dynamic memory resizing
void CPU::resize_memory(size_t new_size) {
    // Validate new size bounds
    if (new_size < CPU_MIN_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Cannot resize memory to {} bytes (below minimum {})",
            new_size, CPU_MIN_MEMORY_SIZE) << std::endl;
        return;
    }
    if (new_size > CPU_MAX_MEMORY_SIZE) {
        Logger::instance().warn() << fmt::format(
            "Cannot resize memory to {} bytes (exceeds maximum {})",
            new_size, CPU_MAX_MEMORY_SIZE) << std::endl;
        return;
    }

    size_t old_size = memory.size();
    memory.resize(new_size, 0); // Initialize new memory to zero

    // Adjust stack pointer if it's now out of bounds
    auto current_sp = static_cast<size_t>(registers[static_cast<size_t>(Register::RSP)]);
    if (current_sp >= new_size) {
        registers[static_cast<size_t>(Register::RSP)] = new_size - 4; // Move stack pointer to valid location
        registers[static_cast<size_t>(Register::RBP)] = registers[static_cast<size_t>(Register::RSP)]; // Reset frame pointer too
    }

    Logger::instance().info() << fmt::format(
        "Memory resized from {} bytes ({:.1f}MB) to {} bytes ({:.1f}MB)",
        old_size, old_size / (1024.0 * 1024.0),
        new_size, new_size / (1024.0 * 1024.0)) << std::endl;
}

// Reset the CPU state
void CPU::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(legacy_registers.begin(), legacy_registers.end(), 0);
    std::fill(memory.begin(), memory.end(), 0); // Clear memory

    // Reset CPU mode to 32-bit for backward compatibility
    cpu_mode = CPUMode::MODE_32BIT;

    // Reset special registers
    registers[static_cast<size_t>(Register::RIP)] = 0;
    registers[static_cast<size_t>(Register::RSP)] = memory.size();
    registers[static_cast<size_t>(Register::RBP)] = memory.size();
    registers[static_cast<size_t>(Register::RFLAGS)] = 0;

    arg_offset = 0; // Initialize arg_offset for PUSH_ARG/POP_ARG operations
    call_depth = 0; // Reset call stack depth
    last_accessed_addr = INVALID_ADDR; // Clear highlight
    last_modified_addr = INVALID_ADDR; // Clear highlight

    // Sync legacy registers
    sync_legacy_registers();
}

// Print the CPU state (debugging information)
void CPU::print_state(const std::string& info) const {
    // If debug is not enabled, do not print the state
    if (!Config::debug) return;

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    std::ostringstream pc_ss;
    pc_ss << "PC=0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << get_pc();
    oss << "[" << pc_ss.str() << "] "<< std::string("(" + info + ")") << " ";

    // Add CPU mode indicator
    oss << "MODE=" << (is_64bit_mode() ? "x64" : "x32") << " ";

    for (size_t i = 0; i < legacy_registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(legacy_registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << get_sp() << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << get_flags();

    // Use DebugHandler instead of Logger to avoid deadlock
    DEBUG_INFO(Logging::DebugCategory::CPU_REGISTERS, "{}", oss.str());
}

void CPU::print_stack_frame(const std::string& label) const {
    // If debug is not enabled, do not print the state
    if (!Config::debug) return;

    // Use DebugHandler instead of Logger to avoid deadlock
    DEBUG_INFO(Logging::DebugCategory::CPU_STACK, "[{}] FP=0x{:X} SP=0x{:X} arg_offset={}", 
        label, get_fp(), get_sp(), arg_offset);
}

// Fetches the next byte as an operand and advances PC
uint8_t CPU::fetch_operand() {
    DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "Fetching operand at PC=0x{:04X}", get_pc());
    if (get_pc() + 1 >= memory.size()) {
        DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "Operand fetch out of bounds at PC=0x{:04X}, memory size={}", get_pc(), memory.size());
        return 0;
    }
    uint8_t operand = memory[get_pc() + 1];
    DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "Fetched operand=0x{:02X}, advancing PC from 0x{:04X}", operand, get_pc());
    set_pc(get_pc() + 1);
    DEBUG_TRACE(Logging::DebugCategory::CPU_EXECUTION, "PC advanced to 0x{:04X}", get_pc());
    return operand;
}

// Reads a 32-bit value from memory at the given address (little-endian)
uint32_t CPU::read_mem32(uint32_t addr) const {
    if (addr + 3 >= memory.size()) {
        DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "Memory read out of bounds at address=0x{:08X}, memory size={}", addr, memory.size());
        return 0;
    }
    last_accessed_addr = addr;
    return (static_cast<uint32_t>(memory[addr])      ) |
           (static_cast<uint32_t>(memory[addr + 1]) << 8 ) |
           (static_cast<uint32_t>(memory[addr + 2]) << 16) |
           (static_cast<uint32_t>(memory[addr + 3]) << 24);
}

// Writes a 32-bit value to memory at the given address (little-endian)
void CPU::write_mem32(uint32_t addr, uint32_t value) {
    if (addr + 3 >= memory.size()) {
        DEBUG_CRITICAL(Logging::DebugCategory::MEM_BOUNDS, "Memory write out of bounds at address=0x{:08X}, memory size={}", addr, memory.size());
        return;
    }
    last_modified_addr = addr;
    memory[addr    ] = static_cast<uint8_t>(value      );
    memory[addr + 1] = static_cast<uint8_t>(value >> 8 );
    memory[addr + 2] = static_cast<uint8_t>(value >> 16);
    memory[addr + 3] = static_cast<uint8_t>(value >> 24);
}

void CPU::execute(const std::vector<uint8_t>& program, uint32_t entry_address, size_t max_steps) {
    // Check if program fits in memory
    if (program.size() > memory.size()) {
        throw std::runtime_error("Program size (" + std::to_string(program.size()) + 
                                " bytes) exceeds available memory (" + std::to_string(memory.size()) + " bytes)");
    }
    
    // Copy program into memory
    std::copy(program.begin(), program.end(), memory.begin());
    // Debug: Print entry address and PC before setting
    DEBUG_CPU("Starting execution at entry address 0x{:04X}, initial PC was 0x{:04X}", entry_address, get_pc());
    // Use entry address if provided, otherwise default to 0
    set_pc(entry_address);
    DEBUG_CPU("PC set to entry address: 0x{:04X}", get_pc());
    registers[static_cast<size_t>(Register::RSP)] = memory.size() - 4; // Stack pointer starts at the end of memory
    registers[static_cast<size_t>(Register::RBP)] = get_sp();
    bool running = true;
    size_t step_count = 0;

    while (get_pc() < program.size() && running) {
        DEBUG_DETAIL(Logging::DebugCategory::CPU_EXECUTION, "Execution loop iteration: running={}, step_count={}", running, step_count);
        
        // Check step limit if provided (0 means unlimited)
        if (max_steps > 0 && step_count >= max_steps) {
            throw std::runtime_error("Test exceeded maximum execution steps (possible infinite loop)");
        }
        
        // Try instruction fusion first for performance
        // If fusion doesn't apply, use branch-predictive dispatcher
        if (!InstructionFusion::try_instruction_fusion(*this, program, running)) {
            // Use branch-predictive dispatcher with speculative execution
            dispatch_opcode_with_prediction(*this, program, running);
        }
        
        step_count++;
    }
}

void CPU::print_registers() const {
    std::ostringstream oss;
    oss << "Registers:" << std::endl;
    for (size_t i = 0; i < legacy_registers.size(); ++i) {
        oss << "R" << i << "=0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(legacy_registers[i]) << " ";
    }
    oss << "SP=0x" << std::setw(3) << std::setfill('0') << std::hex << std::uppercase << get_sp() << " ";
    oss << "FLAGS=0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << get_flags();

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_register_update(Register reg, uint64_t old_value, uint64_t new_value) const {
    // Only print if debug mode and extended registers are both enabled
    if (!Config::debug || !Config::extended_registers) return;

    std::string reg_name = get_register_name(reg);
    Logger::instance().debug() << fmt::format(
        "[REG_UPDATE] {} changed: 0x{:016X} -> 0x{:016X}",
        reg_name, old_value, new_value
    ) << std::endl;
}

void CPU::print_extended_registers() const {
    std::ostringstream oss;
    oss << "Extended Registers:" << std::endl;

    // General purpose registers (RAX-R15) - 4 per line
    for (size_t i = 0; i < GENERAL_PURPOSE_COUNT; ++i) {
        if (i % 4 == 0) {
            if (i > 0) oss << std::endl;
            oss << "GP" << (i/4 + 1) << ": ";
        }
        Register reg = static_cast<Register>(i);
        std::string reg_name = get_register_name(reg);

        // Reset all formatting before register name
        oss << std::left << std::setw(3) << std::setfill(' ') << reg_name;

        // Format the hex value with proper settings
        oss << "=0x" << std::right << std::setw(16) << std::setfill('0')
            << std::hex << std::uppercase << get_register_64(reg) << " ";
    }
    oss << std::endl;

    // Special registers
    oss << "Special: ";
    oss << "RIP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RIP) << " ";
    oss << "RSP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RSP) << " ";
    oss << "RBP=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RBP) << " ";
    oss << "RFLAGS=0x" << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << get_register_64(Register::RFLAGS);

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::print_memory(std::size_t start, std::size_t end) const {
    std::ostringstream oss;
    oss << "Memory:" << std::endl;
    for (std::size_t i = start; i < end && i < memory.size(); ++i) {
        // Print address at the start of each row
        if (i % 16 == 0) {
            if (i > start) oss << std::endl;
            oss << "0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << i << ": ";
        }

        // Bounds check for memory access
        uint8_t value = 0;
        if (i < memory.size()) {
            value = memory[i];
        } else {
            oss << "[??] ";
            continue;
        }

        // Print memory value with appropriate highlight
        if (i == last_accessed_addr)
            oss << "[A:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(value) << "] ";
        else if (i == last_modified_addr)
            oss << "[M:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(value) << "] ";
        else
            oss << "[" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(value) << "] ";
    }

    Logger::instance().info() << oss.str() << std::endl;
}

void CPU::run(const std::vector<uint8_t>& program) {
    reset();
    execute(program);
}

bool CPU::step(const std::vector<uint8_t>& program) {
    // Copy program into memory if first step and memory is empty
    if (get_pc() == 0 && memory[0] == 0 && !program.empty()) {
        std::copy(program.begin(), program.end(), memory.begin());
        registers[static_cast<size_t>(Register::RSP)] = memory.size() - 4;
        registers[static_cast<size_t>(Register::RBP)] = get_sp();
    }

    if (get_pc() >= program.size()) {
        return false; // Program ended
    }

    bool running = true;
    
    // Try instruction fusion first, fall back to optimized dispatch
    if (!InstructionFusion::try_instruction_fusion(*this, program, running)) {
        dispatch_opcode_inlined_optimized(*this, program, running);
    }

    return running;
}

uint8_t CPU::readPort(uint8_t port) {
    return vhw::DeviceManager::instance().readPort(port);
}

void CPU::writePort(uint8_t port, uint8_t value) {
    vhw::DeviceManager::instance().writePort(port, value);
}

std::string CPU::readPortString(uint8_t port, uint8_t maxLength) {
    return vhw::DeviceManager::instance().readPortString(port, maxLength);
}

void CPU::writePortString(uint8_t port, const std::string& str) {
    vhw::DeviceManager::instance().writePortString(port, str);
}

// FPU Stack Management Implementation
void CPU::fpu_push(double value) {
    // Convert double to x87 80-bit format (simplified implementation)
    // For now, we'll store as 64-bit mantissa + 16-bit exponent/sign
    
    // Move stack top down (stack grows downward)
    fpu_stack_top = (fpu_stack_top - 1) & 0x7;
    
    // Convert double to our internal representation
    uint64_t mantissa;
    uint64_t exponent_sign;
    
    if (value == 0.0) {
        mantissa = 0;
        exponent_sign = 0;
    } else {
        // Use IEEE 754 double as mantissa for now (simplified)
        union { double d; uint64_t i; } converter;
        converter.d = value;
        mantissa = converter.i;
        exponent_sign = 0; // Simplified - normally would extract IEEE 754 parts
    }
    
    // Store in ST(0) position (current stack top)
    Register st_reg = static_cast<Register>(static_cast<size_t>(Register::ST0) + fpu_stack_top * 2);
    set_fpu_register(st_reg, mantissa, exponent_sign);
    
    // Update tag word to mark register as valid
    fpu_tag_word &= ~(0x3 << (fpu_stack_top * 2)); // Clear tag bits
    fpu_tag_word |= (0x0 << (fpu_stack_top * 2));  // Set as valid (00)
}

double CPU::fpu_pop() {
    // Get value from ST(0)
    double value = fpu_peek(0);
    
    // Mark register as empty
    fpu_tag_word |= (0x3 << (fpu_stack_top * 2)); // Set as empty (11)
    
    // Move stack top up
    fpu_stack_top = (fpu_stack_top + 1) & 0x7;
    
    return value;
}

double CPU::fpu_peek(uint8_t offset) const {
    // Calculate actual register index
    uint8_t reg_index = (fpu_stack_top + offset) & 0x7;
    Register st_reg = static_cast<Register>(static_cast<size_t>(Register::ST0) + reg_index * 2);
    
    uint64_t mantissa, exponent_sign;
    get_fpu_register(st_reg, mantissa, exponent_sign);
    
    // Convert back to double (simplified)
    if (mantissa == 0 && exponent_sign == 0) {
        return 0.0;
    }
    
    union { double d; uint64_t i; } converter;
    converter.i = mantissa;
    return converter.d;
}

void CPU::fpu_store(uint8_t offset, double value) {
    // Calculate actual register index
    uint8_t reg_index = (fpu_stack_top + offset) & 0x7;
    
    // Convert double to our internal representation
    uint64_t mantissa;
    uint64_t exponent_sign;
    
    if (value == 0.0) {
        mantissa = 0;
        exponent_sign = 0;
    } else {
        union { double d; uint64_t i; } converter;
        converter.d = value;
        mantissa = converter.i;
        exponent_sign = 0; // Simplified
    }
    
    // Store in specified ST register
    Register st_reg = static_cast<Register>(static_cast<size_t>(Register::ST0) + reg_index * 2);
    set_fpu_register(st_reg, mantissa, exponent_sign);
    
    // Update tag word to mark register as valid
    fpu_tag_word &= ~(0x3 << (reg_index * 2)); // Clear tag bits
    fpu_tag_word |= (0x0 << (reg_index * 2));  // Set as valid (00)
}

void CPU::fpu_init() {
    // Initialize FPU to default state
    fpu_stack_top = 0;
    fpu_control_word = 0x037F; // Default control word
    fpu_status_word = 0x0000;  // Clear status
    fpu_tag_word = 0xFFFF;     // All registers empty (changed from 0xFF to 0xFFFF for 16-bit)
    
    // Clear all FPU registers safely
    for (int i = 0; i < 8; i++) {
        Register st_reg = static_cast<Register>(static_cast<size_t>(Register::ST0) + i * 2);
        // Use the safer approach by setting registers directly
        set_register(st_reg, 0);  // mantissa part
        Register meta_reg = static_cast<Register>(static_cast<size_t>(st_reg) + 1);
        set_register(meta_reg, 0);  // exponent/sign part
    }
}
