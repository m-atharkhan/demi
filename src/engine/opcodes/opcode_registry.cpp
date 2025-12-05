#include "opcode_registry.hpp"
#include "../cpu.hpp"
#include "../../debug/logger.hpp"
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
extern void handle_and64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_cmp64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_mov64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
extern void handle_inc64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
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
    REGISTER_OPCODE(0x00, handle_nop);        // NOP
    REGISTER_OPCODE(0x01, handle_load_imm);   // LOAD_IMM
    REGISTER_OPCODE(0x02, handle_add);        // ADD
    REGISTER_OPCODE(0x03, handle_sub);        // SUB
    REGISTER_OPCODE(0x04, handle_mov);        // MOV
    REGISTER_OPCODE(0x05, handle_jmp);        // JMP
    REGISTER_OPCODE(0x06, handle_load);       // LOAD
    REGISTER_OPCODE(0x07, handle_store);      // STORE
    REGISTER_OPCODE(0x08, handle_push);       // PUSH
    REGISTER_OPCODE(0x09, handle_pop);        // POP
    REGISTER_OPCODE(0x0A, handle_cmp);        // CMP
    REGISTER_OPCODE(0x0B, handle_jz);         // JZ (JE)
    REGISTER_OPCODE(0x0C, handle_jnz);        // JNZ (JNE)
    REGISTER_OPCODE(0x0D, handle_jl);         // JL
    REGISTER_OPCODE(0x0E, handle_jg);         // JG
    REGISTER_OPCODE(0x0F, handle_jle);        // JLE
    REGISTER_OPCODE(0x10, handle_jge);        // JGE
    REGISTER_OPCODE(0x11, handle_inc);        // INC
    REGISTER_OPCODE(0x12, handle_dec);        // DEC
    REGISTER_OPCODE(0x13, handle_mul);        // MUL
    REGISTER_OPCODE(0x14, handle_div);        // DIV
    REGISTER_OPCODE(0x15, handle_mod);        // MOD
    REGISTER_OPCODE(0x16, handle_and);        // AND
    REGISTER_OPCODE(0x17, handle_or);         // OR
    REGISTER_OPCODE(0x18, handle_xor);        // XOR
    REGISTER_OPCODE(0x19, handle_not);        // NOT
    REGISTER_OPCODE(0x1A, handle_shl);        // SHL
    REGISTER_OPCODE(0x1B, handle_call);       // CALL
    REGISTER_OPCODE(0x1C, handle_ret);        // RET
    REGISTER_OPCODE(0x1D, handle_push_arg);   // PUSH_ARG
    REGISTER_OPCODE(0x1E, handle_pop_arg);    // POP_ARG
    REGISTER_OPCODE(0x1F, handle_push_flag);  // PUSH_FLAG
    REGISTER_OPCODE(0x20, handle_pop_flag);   // POP_FLAG
    REGISTER_OPCODE(0x21, handle_lea);        // LEA
    REGISTER_OPCODE(0x22, handle_swap);       // SWAP
    REGISTER_OPCODE(0x23, handle_shr);        // SHR
    REGISTER_OPCODE(0x24, handle_js);         // JS
    REGISTER_OPCODE(0x25, handle_jns);        // JNS
    REGISTER_OPCODE(0x26, handle_jc);         // JC
    REGISTER_OPCODE(0x27, handle_jnc);        // JNC
    REGISTER_OPCODE(0x28, handle_jo);         // JO
    REGISTER_OPCODE(0x29, handle_jno);        // JNO
    REGISTER_OPCODE(0x2A, handle_in);         // IN
    REGISTER_OPCODE(0x2B, handle_inb);        // INB
    REGISTER_OPCODE(0x2C, handle_inw);        // INW
    REGISTER_OPCODE(0x2D, handle_inl);        // INL
    REGISTER_OPCODE(0x2E, handle_instr);      // INSTR
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
    
    Logger::instance().info() << "Opcode registry initialized with " 
                              << std::count_if(handlers_.begin(), handlers_.end(), 
                                   [](const OpcodeHandler& h) { return h != nullptr; })
                              << " registered opcodes" << std::endl;
}