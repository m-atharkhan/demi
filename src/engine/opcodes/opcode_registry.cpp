#include "opcode_registry.hpp"
#include "../cpu.hpp"
#include "../../debug/debug_handler.hpp"
#include <fmt/format.h>

// External handler function declarations
extern void handle_nop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_load_imm(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_add(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_sub(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mul(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_div(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mod(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inc(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_dec(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mov(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jnz(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_js(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jns(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jc(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jnc(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jo(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jno(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jg(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jl(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jge(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_jle(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_load(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_loadr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_storer(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_lea(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_store(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_swap(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_push(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_push_flag(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_pop_flag(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_halt(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_and(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_or(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_xor(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_not(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_shl(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_shr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_call(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_ret(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_push_arg(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_pop_arg(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_in(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_out(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inb(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_outb(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inw(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_outw(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inl(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_outl(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_instr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_outstr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_db(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_add64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_sub64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mul64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_div64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mod64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_and64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_or64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_xor64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mov64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inc64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_not64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_dec64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_load_imm64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_movex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_addex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_subex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_loadex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_storex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Interrupt operations
extern void handle_cli(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_sti(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_int(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_iret(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// FPU Operations - Forward declarations (existing implementations)
extern void handle_FLD(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FST(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSTP(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FILD(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FIST(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FISTP(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FADD(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSUB(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FMUL(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FDIV(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSIN(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FCOS(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FTAN(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSQRT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FABS(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FCHS(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FINIT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FCLEX(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSTCW(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FLDCW(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FSTSW(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FCOMPP(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_FUCOMPP(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// SIMD Operations - Forward declarations
extern void handle_VADD(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_VMUL(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_VDOT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_VMAX(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_VBROADCAST(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_VCMPGT(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_PACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_UNPACKB(CPU& cpu, const std::vector<uint8_t>& program, bool& running);

// Get the singleton instance
OpcodeRegistry& OpcodeRegistry::instance() {
    static OpcodeRegistry instance;
    return instance;
}

// Register an opcode handler
void OpcodeRegistry::register_opcode(uint8_t opcode, OpcodeHandler handler) {
    handlers_[opcode] = handler;
}

// Get handler for an opcode
OpcodeHandler OpcodeRegistry::get_handler(uint8_t opcode) const {
    return handlers_[opcode];
}

// Get all handlers
const std::array<OpcodeHandler, 256>& OpcodeRegistry::get_handlers() const {
    return handlers_;
}

// Check if an opcode is registered
bool OpcodeRegistry::is_registered(uint8_t opcode) const {
    return handlers_[opcode] != nullptr;
}

// Initialize all opcode handlers - SINGLE SOURCE OF TRUTH
void OpcodeRegistry::initialize_handlers() {
    if (initialized_) return;
    
    // Clear all handlers first
    handlers_.fill(nullptr);
    
    // Register all opcodes - this is the ONLY place opcode mappings are defined
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::NOP), handle_nop);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::LOAD_IMM), handle_load_imm);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::ADD), handle_add);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::SUB), handle_sub);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::MOV), handle_mov);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JMP), handle_jmp);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::LOAD), handle_load);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::STORE), handle_store);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::PUSH), handle_push);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::POP), handle_pop);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::CMP), handle_cmp);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JZ), handle_jz);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JNZ), handle_jnz);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JS), handle_js);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JNS), handle_jns);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JC), handle_jc);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JNC), handle_jnc);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JO), handle_jo);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JNO), handle_jno);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JG), handle_jg);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JL), handle_jl);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JGE), handle_jge);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::JLE), handle_jle);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::INC), handle_inc);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::DEC), handle_dec);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::MUL), handle_mul);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::DIV), handle_div);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::MOD), handle_mod);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::AND), handle_and);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::OR), handle_or);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::XOR), handle_xor);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::NOT), handle_not);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::SHL), handle_shl);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::SHR), handle_shr);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::CALL), handle_call);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::RET), handle_ret);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::PUSH_ARG), handle_push_arg);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::POP_ARG), handle_pop_arg);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::PUSH_FLAG), handle_push_flag);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::POP_FLAG), handle_pop_flag);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::LEA), handle_lea);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::SWAP), handle_swap);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::IN), handle_in);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::INB), handle_inb);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::INW), handle_inw);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::INL), handle_inl);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::INSTR), handle_instr);
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::OUTB), handle_outb);       // OUTB
    REGISTER_OPCODE(0x30, handle_outw);       // OUTW
    REGISTER_OPCODE(0x31, handle_outl);       // OUTL
    REGISTER_OPCODE(0x32, handle_outstr);     // OUTSTR
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::DB), handle_db);         // DB
    
    // Extended 64-bit operations (temporary range until proper 0x50+ range implemented)
    REGISTER_OPCODE(0x34, handle_add64);      // ADD64
    REGISTER_OPCODE(0x35, handle_sub64);      // SUB64
    REGISTER_OPCODE(0x36, handle_mov64);      // MOV64
    REGISTER_OPCODE(0x37, handle_load_imm64); // LOAD_IMM64
    REGISTER_OPCODE(0x38, handle_movex);      // MOVEX
    REGISTER_OPCODE(0x39, handle_addex);      // ADDEX
    REGISTER_OPCODE(0x3A, handle_subex);      // SUBEX
    
    // Proper 64-bit operations (0x50-0x5F range per opcodes.hpp)
    REGISTER_OPCODE(0x50, handle_add64);      // ADD64
    REGISTER_OPCODE(0x51, handle_sub64);      // SUB64
    REGISTER_OPCODE(0x52, handle_mov64);      // MOV64
    REGISTER_OPCODE(0x53, handle_load_imm64); // LOAD_IMM64
    REGISTER_OPCODE(0x54, handle_mul64);      // MUL64
    REGISTER_OPCODE(0x55, handle_div64);      // DIV64
    REGISTER_OPCODE(0x56, handle_and64);      // AND64
    REGISTER_OPCODE(0x57, handle_or64);       // OR64
    REGISTER_OPCODE(0x58, handle_xor64);      // XOR64
    REGISTER_OPCODE(0x59, handle_not64);      // NOT64
    REGISTER_OPCODE(0x5F, handle_mod64);      // MOD64
    REGISTER_OPCODE(0x5C, handle_cmp64);      // CMP64
    REGISTER_OPCODE(0x5D, handle_inc64);      // INC64
    REGISTER_OPCODE(0x5E, handle_dec64);      // DEC64
    
    // Extended register operations (0x60-0x6F range per opcodes.hpp)
    REGISTER_OPCODE(0x60, handle_movex);      // MOVEX
    REGISTER_OPCODE(0x61, handle_addex);      // ADDEX
    REGISTER_OPCODE(0x62, handle_subex);      // SUBEX
    REGISTER_OPCODE(0x66, handle_loadex);     // LOADEX
    REGISTER_OPCODE(0x67, handle_storex);     // STOREX
    
    // FPU Operations (0xA0-0xBF range - using existing implementations)
    REGISTER_OPCODE(0xA0, handle_FLD);       // FLD - Load floating-point value
    REGISTER_OPCODE(0xA1, handle_FST);       // FST - Store floating-point value
    REGISTER_OPCODE(0xA2, handle_FSTP);      // FSTP - Store floating-point value and pop
    REGISTER_OPCODE(0xA3, handle_FILD);      // FILD - Load integer as floating-point
    REGISTER_OPCODE(0xA4, handle_FIST);      // FIST - Store floating-point as integer
    REGISTER_OPCODE(0xA5, handle_FISTP);     // FISTP - Store floating-point as integer and pop
    REGISTER_OPCODE(0xA6, handle_FADD);      // FADD - Floating-point add
    REGISTER_OPCODE(0xA7, handle_FSUB);      // FSUB - Floating-point subtract
    REGISTER_OPCODE(0xA8, handle_FMUL);      // FMUL - Floating-point multiply
    REGISTER_OPCODE(0xA9, handle_FDIV);      // FDIV - Floating-point divide
    REGISTER_OPCODE(0xAA, handle_FSIN);      // FSIN - Floating-point sine
    REGISTER_OPCODE(0xAB, handle_FCOS);      // FCOS - Floating-point cosine
    REGISTER_OPCODE(0xAC, handle_FTAN);      // FTAN - Floating-point tangent
    REGISTER_OPCODE(0xAD, handle_FSQRT);     // FSQRT - Floating-point square root
    REGISTER_OPCODE(0xAE, handle_FABS);      // FABS - Floating-point absolute value
    REGISTER_OPCODE(0xAF, handle_FCHS);      // FCHS - Floating-point change sign
    
    // FPU Control Operations (0xB0-0xB6 range)
    REGISTER_OPCODE(0xB0, handle_FINIT);     // FINIT - Initialize FPU
    REGISTER_OPCODE(0xB1, handle_FCLEX);     // FCLEX - Clear exceptions
    REGISTER_OPCODE(0xB2, handle_FSTCW);     // FSTCW - Store control word
    REGISTER_OPCODE(0xB3, handle_FLDCW);     // FLDCW - Load control word
    REGISTER_OPCODE(0xB4, handle_FSTSW);     // FSTSW - Store status word
    REGISTER_OPCODE(0xB5, handle_FCOMPP);    // FCOMPP - Compare and pop twice
    REGISTER_OPCODE(0xB6, handle_FUCOMPP);   // FUCOMPP - Unordered compare and pop twice
    
    // SIMD Operations (0xD4-0xDB range)
    REGISTER_OPCODE(0xD4, handle_VADD);      // VADD - Vector add
    REGISTER_OPCODE(0xD5, handle_VMUL);      // VMUL - Vector multiply
    REGISTER_OPCODE(0xD6, handle_VDOT);      // VDOT - Vector dot product
    REGISTER_OPCODE(0xD7, handle_VMAX);      // VMAX - Vector maximum
    REGISTER_OPCODE(0xD8, handle_VBROADCAST); // VBROADCAST - Vector broadcast
    REGISTER_OPCODE(0xD9, handle_VCMPGT);    // VCMPGT - Vector compare greater than
    REGISTER_OPCODE(0xDA, handle_PACKB);     // PACKB - Pack bytes
    REGISTER_OPCODE(0xDB, handle_UNPACKB);   // UNPACKB - Unpack bytes
    
    REGISTER_OPCODE(static_cast<uint8_t>(Opcode::OUT), handle_out);        // OUT
    REGISTER_OPCODE(0x41, handle_loadr);      // LOADR - Load indirect (address in register)
    REGISTER_OPCODE(0x43, handle_storer);     // STORER - Store indirect (address in register)
    
    // Interrupt operations (matching x86 conventions)
    REGISTER_OPCODE(0xCD, handle_int);        // INT - Software interrupt
    REGISTER_OPCODE(0xCF, handle_iret);       // IRET - Interrupt return
    REGISTER_OPCODE(0xFA, handle_cli);        // CLI - Clear interrupt flag
    REGISTER_OPCODE(0xFB, handle_sti);        // STI - Set interrupt flag
    
    REGISTER_OPCODE(0xFF, handle_halt);       // HALT
    
    initialized_ = true;
    
    Logging::DebugHandler::instance().report(Logging::DebugCategory::CPU_EXECUTION,
        fmt::format("Opcode registry initialized with {} registered opcodes", 
            std::count_if(handlers_.begin(), handlers_.end(), 
                [](const OpcodeHandler& h) { return h != nullptr; })), Logging::DebugLevel::INFO);
}