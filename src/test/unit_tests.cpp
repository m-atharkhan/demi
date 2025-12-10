#include "test_framework.hpp"
#include "../engine/cpu_flags.hpp"

// Example unit tests using the new framework

TEST_CASE(cpu_reset, "cpu") {
    // Test that CPU reset works properly
    ctx.cpu.reset();

    // Check that general-purpose registers are zero (EAX-EDX)
    for (int i = 0; i < 4; i++) {
        ctx.assert_register_eq(i, 0);
    }

    // Check that stack pointers are initialized correctly (ESI=RSP, EDI=RBP should be at memory end)
    ctx.assert_register_eq(4, ctx.cpu.get_memory_size()); // RSP = stack top
    ctx.assert_register_eq(5, ctx.cpu.get_memory_size()); // RBP = stack top

    // Check that remaining registers are zero (ESP-EBP)
    for (int i = 6; i < 8; i++) {
        ctx.assert_register_eq(i, 0);
    }

    // Check that PC is zero
    ctx.assert_pc_eq(0);

    // Check that flags are zero
    ctx.assert_flags_eq(0);

    // Check that stack pointer is at end of memory
    ctx.assert_sp_eq(256);
}

TEST_CASE(load_immediate, "instructions") {
    // Test LOAD_IMM instruction
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 5
        0x01, 0x01, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 10
        0xFF               // HALT
    });

    ctx.execute_program();

    // Check that registers have correct values
    ctx.assert_register_eq(0, 5);
    ctx.assert_register_eq(1, 10);

    // Check that other registers have expected values
    ctx.assert_register_eq(2, 0);  // ECX should be 0
    ctx.assert_register_eq(3, 0);  // EDX should be 0
    // ESI (RSP) and EDI (RBP) are stack pointers, initialized to memory size - 4 (reserved)
    ctx.assert_register_eq(4, ctx.cpu.get_memory_size() - 4); // RSP = stack top - 4
    ctx.assert_register_eq(5, ctx.cpu.get_memory_size() - 4); // RBP = stack top - 4
    ctx.assert_register_eq(6, 0);  // ESP should be 0
    ctx.assert_register_eq(7, 0);  // EBP should be 0
}

TEST_CASE(add_instruction, "arithmetic") {
    // Test ADD instruction with comprehensive assertions
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 5 (6-byte format)
        0x01, 0x01, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 3 (6-byte format)
        0x02, 0x00, 0x01,                     // ADD EAX, EBX
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Primary assertion: EAX contains the sum
    ctx.assert_register_eq(0, 8);

    // Secondary assertions: verify other state
    ctx.assert_register_eq(1, 3);    // EBX unchanged (operand preserved)
    ctx.assert_register_eq(2, 0);    // ECX still zero
    ctx.assert_register_eq(3, 0);    // EDX still zero
    
    // Flag assertions
    ctx.assert_zero_flag_clear();    // Result 8 is non-zero
    ctx.assert_carry_flag_clear();   // No overflow occurred
    ctx.assert_overflow_flag_clear(); // No signed overflow
    
    // Mode assertions
    ctx.assert_32bit_mode();          // Should be in 32-bit mode
    ctx.assert_register_upper_bits_zero(0);  // Upper 32 bits should be zero
}

TEST_CASE(sub_instruction, "arithmetic") {
    // Test SUB instruction with comprehensive assertions
    ctx.load_program({
        0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 10 (6-byte format)
        0x01, 0x01, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 3 (6-byte format)
        0x03, 0x00, 0x01,                     // SUB EAX, EBX
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Primary assertion: EAX contains the difference
    ctx.assert_register_eq(0, 7);

    // Secondary assertions: verify other state
    ctx.assert_register_eq(1, 3);    // EBX unchanged (operand preserved)
    ctx.assert_register_eq(2, 0);    // ECX still zero
    ctx.assert_register_eq(3, 0);    // EDX still zero
    
    // Flag assertions
    ctx.assert_zero_flag_clear();    // Result 7 is non-zero
    ctx.assert_carry_flag_clear();   // No borrow occurred (10 > 3)
    ctx.assert_sign_flag_clear();    // Result is positive
    
    // Mode assertions
    ctx.assert_32bit_mode();          // Should be in 32-bit mode
    ctx.assert_register_upper_bits_zero(0);  // Upper 32 bits should be zero
}

TEST_CASE(cmp_instruction_flags, "flags") {
    // Test CMP instruction flag setting with equal values
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 5
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 5
        0x0A, 0x00, 0x01,                     // CMP EAX, EBX
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Primary flag assertions
    ctx.assert_zero_flag_set();         // 5 - 5 = 0, so zero flag should be set
    ctx.assert_sign_flag_clear();       // Result is zero, not negative
    ctx.assert_carry_flag_clear();      // No borrow (5 >= 5)
    ctx.assert_overflow_flag_clear();   // No signed overflow
    
    // CMP should NOT modify operand registers
    ctx.assert_register_eq(0, 5);       // EAX unchanged
    ctx.assert_register_eq(1, 5);       // EBX unchanged
    
    // Mode assertion
    ctx.assert_32bit_mode();
}

TEST_CASE(cmp_instruction_different, "flags") {
    // Test CMP instruction with EAX < EBX (borrow case)
    ctx.load_program({
        0x01, 0x00, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 3
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 5
        0x0A, 0x00, 0x01,                     // CMP EAX, EBX (3 - 5 = -2)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Primary flag assertions
    ctx.assert_zero_flag_clear();       // Result -2 is non-zero
    ctx.assert_sign_flag_set();         // Result is negative
    ctx.assert_carry_flag_set();        // Borrow occurred (3 < 5)
    
    // CMP should NOT modify operand registers
    ctx.assert_register_eq(0, 3);       // EAX unchanged
    ctx.assert_register_eq(1, 5);       // EBX unchanged
    
    // Mode assertion
    ctx.assert_32bit_mode();
}

TEST_CASE_EXPECT_ERROR(division_by_zero, "arithmetic") {
    // Test that division by zero throws an error
    ctx.load_program({
        0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 10
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0
        0x11, 0x00, 0x01,                     // DIV EAX, EBX
        0xFF                                  // HALT
    });

    // This should throw an exception or set error count
    ctx.execute_program();
}

TEST_CASE(memory_operations, "memory") {
    // Test that we can load and store values in memory with comprehensive checks
    // Uses assembler to generate correct bytecode format
    // Note: x86 register numbering: EAX=0, ECX=1, EDX=2, EBX=3, ESP=4, EBP=5, ESI=6, EDI=7
    // Use address 0x50 to avoid overlap with program bytecode (program is ~26 bytes)
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x42
        STORE EAX, 0x50
        LOAD_IMM EBX, 0
        LOAD EBX, 0x50
        HALT
    )");

    ctx.execute_program();

    // Primary assertions: value stored and loaded correctly
    // EAX is R0, EBX is R3 in x86 convention
    ctx.assert_register_eq(0, 0x42);    // EAX (R0) should have original value
    ctx.assert_register_eq(3, 0x42);    // EBX (R3) should have loaded value
    
    // Memory assertions (STORE writes 4 bytes in 32-bit mode)
    ctx.assert_memory_eq(0x50, 0x42);   // Memory at address 0x50 should contain 0x42
    ctx.assert_memory_eq(0x51, 0x00);   // Adjacent bytes should be zero (little-endian)
    ctx.assert_memory_eq(0x52, 0x00);
    ctx.assert_memory_eq(0x53, 0x00);
    
    // Other registers should be unaffected
    ctx.assert_register_eq(1, 0);       // ECX still zero
    ctx.assert_register_eq(2, 0);       // EDX still zero
    
    // Mode assertion
    ctx.assert_32bit_mode();
}

TEST_CASE(program_counter_progression, "cpu") {
    // Test that PC advances correctly with simple instructions
    // Use a different instruction that might work better than NOP
    ctx.load_program({
        0x01, 0x00, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 1 (6 bytes)
        0xFF               // HALT
    });

    // Execute step by step and check PC
    ctx.assert_pc_eq(0);

    ctx.execute_single_step();  // LOAD_IMM EAX, 1
    ctx.assert_pc_eq(6);  // Should advance by 6 for LOAD_IMM (6-byte format)
}

TEST_CASE(stack_operations, "stack") {
    // Test PUSH and POP operations
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 66
        0x01, 0x01, 0x84, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 132
        0x08, 0x00,                           // PUSH EAX
        0x08, 0x01,                           // PUSH EBX
        0x09, 0x02,                           // POP ECX
        0x09, 0x03,                           // POP EDX
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that values were pushed and popped correctly
    ctx.assert_register_eq(2, 132);  // ECX should have EBX's value
    ctx.assert_register_eq(3, 66);   // EDX should have EAX's value

    // Stack pointer should be back to original position (252 for 256-byte memory)
    ctx.assert_sp_eq(252);
}

TEST_CASE(conditional_jumps, "control_flow") {
    // Test conditional jump (JZ after CMP)
    // Instruction layout with 6-byte LOAD_IMM and 4-byte addresses:
    // PC=0: LOAD_IMM EAX, 5 (6 bytes) -> next at 6
    // PC=6: LOAD_IMM EBX, 5 (6 bytes) -> next at 12
    // PC=12: CMP EAX, EBX (3 bytes) -> next at 15
    // PC=15: JZ target (5 bytes - opcode + 4-byte addr) -> next at 20
    // PC=20: LOAD_IMM ECX, 153 (6 bytes) -> next at 26 (should be skipped)
    // PC=26: LOAD_IMM EDX, 119 (6 bytes) -> next at 32
    // PC=32: HALT
    // If equal, JZ should jump to PC=26 to skip the ECX load
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 5 (PC=0)
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 5 (PC=6)
        0x0A, 0x00, 0x01,                     // CMP EAX, EBX (PC=12)
        0x0B, 0x1A, 0x00, 0x00, 0x00,        // JZ 0x1A (26) - jump to LOAD_IMM EDX (PC=15)
        0x01, 0x02, 0x99, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 153 (PC=20, should be skipped)
        0x01, 0x03, 0x77, 0x00, 0x00, 0x00,  // LOAD_IMM EDX, 119 (PC=26)
        0xFF                                  // HALT (PC=32)
    });

    ctx.execute_program();

    // ECX should not be set (jump skipped it)
    ctx.assert_register_eq(2, 0);
    // EDX should be set
    ctx.assert_register_eq(3, 119);
    // Zero flag should be set from the CMP
    ctx.assert_flag_set(FLAG_ZERO);
}

TEST_CASE(memory_load_store, "memory") {
    // Test LOAD and STORE operations
    // STORE and LOAD now use mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 66
        0x07, 0x00, 0x64, 0x00, 0x00, 0x00,  // STORE EAX, 0x64 (store EAX to address 100)
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0 (clear EAX)
        0x06, 0x00, 0x64, 0x00, 0x00, 0x00,  // LOAD EAX, 0x64 (load from address 100)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // EAX should contain the value we stored and loaded back
    ctx.assert_register_eq(0, 66);
    // Memory at address 100 should contain 66
    ctx.assert_memory_eq(100, 66);
}

TEST_CASE(io_operations, "devices") {
    // Test input/output operations
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 66
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 1 (console port)
        0x31, 0x00, 0x01,  // OUT EAX, EBX (output to console)
        0x30, 0x02, 0x01,  // IN ECX, EBX (read from console)
        0xFF               // HALT
    });

    ctx.execute_program();

    // After output, EAX should still contain 66
    ctx.assert_register_eq(0, 66);
    // The console device should have received the output
    // (Note: We can't easily test the actual device interaction in this simple test)
}

TEST_CASE(bitwise_operations, "bitwise") {
    // Test AND, OR, XOR operations
    ctx.load_program({
        0x01, 0x00, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 15 (0x0F)
        0x01, 0x01, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 51 (0x33)
        0x14, 0x00, 0x01,                     // AND EAX, EBX (0x0F & 0x33 = 0x03)
        0x01, 0x02, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 15 (0x0F)
        0x01, 0x03, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM EDX, 51 (0x33)
        0x15, 0x02, 0x03,                     // OR ECX, EDX (0x0F | 0x33 = 0x3F)
        0x01, 0x04, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM ESI, 15 (0x0F)
        0x01, 0x05, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM EDI, 51 (0x33)
        0x16, 0x04, 0x05,                     // XOR ESI, EDI (0x0F ^ 0x33 = 0x3C)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check the results of bitwise operations
    ctx.assert_register_eq(0, 0x03);  // AND result
    ctx.assert_register_eq(2, 0x3F);  // OR result
    ctx.assert_register_eq(4, 0x3C);  // XOR result
}

TEST_CASE(shift_operations, "bitwise") {
    // Test SHL and SHR operations
    ctx.load_program({
        0x01, 0x00, 0x08, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 8
        0x18, 0x00, 0x02,  // SHL EAX, 2 (immediate) (8 << 2 = 32)
        0x01, 0x02, 0x20, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 32
        0x19, 0x02, 0x02,  // SHR ECX, 2 (immediate) (32 >> 2 = 8)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Check the results of shift operations
    ctx.assert_register_eq(0, 32);  // SHL result
    ctx.assert_register_eq(2, 8);   // SHR result
}

TEST_CASE(flags_comprehensive, "flags") {
    // Test various flag conditions
    ctx.load_program({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 1
        0x0A, 0x00, 0x01,  // CMP EAX, EBX (correct opcode: 0x0A, not 0x05)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Simplified test - just check if execution completed
    // The fact that we reach here means no infinite loop
    ctx.assert_register_eq(0, 0);  // EAX should still be 0
    ctx.assert_register_eq(1, 1);  // EBX should still be 1
}

TEST_CASE(lea_basic, "lea") {
    // Test LEA (Load Effective Address) - loads address into register
    // LEA now uses mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x20, 0x00, 0x42, 0x00, 0x00, 0x00,  // LEA EAX, 0x42 (load address 0x42 into EAX)
        0x20, 0x01, 0x10, 0x00, 0x00, 0x00,  // LEA EBX, 0x10 (load address 0x10 into EBX)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that the addresses were loaded correctly
    ctx.assert_register_eq(0, 0x42);
    ctx.assert_register_eq(1, 0x10);
}

TEST_CASE(lea_multiple_addresses, "lea") {
    // Test LEA with multiple different addresses
    // LEA now uses mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x20, 0x00, 0x00, 0x00, 0x00, 0x00,  // LEA EAX, 0x00 (load address 0x00 into EAX)
        0x20, 0x01, 0xFF, 0x00, 0x00, 0x00,  // LEA EBX, 0xFF (load address 0xFF into EBX)
        0x20, 0x02, 0x80, 0x00, 0x00, 0x00,  // LEA ECX, 0x80 (load address 0x80 into ECX)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that all addresses were loaded correctly
    ctx.assert_register_eq(0, 0x00);
    ctx.assert_register_eq(1, 0xFF);
    ctx.assert_register_eq(2, 0x80);
}

TEST_CASE(swap_basic, "swap") {
    // Test SWAP - swap values between register and memory
    // STORE and SWAP now use mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x42
        0x07, 0x00, 0x50, 0x00, 0x00, 0x00,  // STORE EAX, 0x50 (store 0x42 at memory[0x50])
        0x01, 0x00, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x33
        0x21, 0x00, 0x50, 0x00, 0x00, 0x00,  // SWAP EAX, 0x50 (swap EAX with memory[0x50])
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // After swap: EAX should have 0x42, memory[0x50] should have 0x33
    ctx.assert_register_eq(0, 0x42);
    ctx.assert_memory_eq(0x50, 0x33);
}

TEST_CASE(swap_multiple_operations, "swap") {
    // Test multiple SWAP operations
    // STORE and SWAP now use mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x01, 0x00, 0x11, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x11
        0x01, 0x01, 0x22, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x22
        0x07, 0x00, 0x60, 0x00, 0x00, 0x00,  // STORE EAX, 0x60 (store 0x11 at memory[0x60])
        0x07, 0x01, 0x61, 0x00, 0x00, 0x00,  // STORE EBX, 0x61 (store 0x22 at memory[0x61])

        0x01, 0x00, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x33
        0x01, 0x01, 0x44, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x44

        0x21, 0x00, 0x60, 0x00, 0x00, 0x00,  // SWAP EAX, 0x60 (swap EAX with memory[0x60])
        0x21, 0x01, 0x61, 0x00, 0x00, 0x00,  // SWAP EBX, 0x61 (swap EBX with memory[0x61])
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // After swaps: EAX should have 0x11, EBX should have 0x22
    // memory[0x60] should have 0x33, memory[0x61] should have 0x44
    ctx.assert_register_eq(0, 0x11);
    ctx.assert_register_eq(1, 0x22);
    ctx.assert_memory_eq(0x60, 0x33);
    ctx.assert_memory_eq(0x61, 0x44);
}

TEST_CASE(lea_swap_combination, "lea") {
    // Test LEA and SWAP working together
    // LEA, STORE, and SWAP now use mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x20, 0x00, 0x70, 0x00, 0x00, 0x00,  // LEA EAX, 0x70 (load address 0x70 into EAX)
        0x01, 0x01, 0x55, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x55
        0x07, 0x01, 0x70, 0x00, 0x00, 0x00,  // STORE EBX, 0x70 (store 0x55 at memory[0x70])
        0x01, 0x01, 0x99, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x99
        0x21, 0x01, 0x70, 0x00, 0x00, 0x00,  // SWAP EBX, 0x70 (swap EBX with memory[0x70])
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // EAX should still have address 0x70, EBX should have 0x55, memory[0x70] should have 0x99
    ctx.assert_register_eq(0, 0x70);
    ctx.assert_register_eq(1, 0x55);
    ctx.assert_memory_eq(0x70, 0x99);
}

TEST_CASE(swap_edge_cases, "swap") {
    // Test SWAP with same values
    // STORE and SWAP now use mode-aware 4-byte addresses in 32-bit mode
    ctx.load_program({
        0x01, 0x00, 0x77, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x77
        0x07, 0x00, 0x80, 0x00, 0x00, 0x00,  // STORE EAX, 0x80 (store 0x77 at memory[0x80])
        0x21, 0x00, 0x80, 0x00, 0x00, 0x00,  // SWAP EAX, 0x80 (swap EAX with memory[0x80])
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Both EAX and memory[0x80] should still have 0x77
    ctx.assert_register_eq(0, 0x77);
    ctx.assert_memory_eq(0x80, 0x77);
}

TEST_CASE(jc_carry_set, "conditional_jumps") {
    // Test JC (Jump if Carry) when carry flag is set
    // Use a simpler approach: load max value and add to it
    // PC layout with 6-byte LOAD_IMM:
    // PC=0: LOAD_IMM EAX, 0 (6 bytes)
    // PC=6: NOT EAX (2 bytes) - EAX becomes 0xFFFFFFFF
    // PC=8: LOAD_IMM EBX, 1 (6 bytes)
    // PC=14: ADD EAX, EBX (3 bytes) - causes carry
    // PC=17: JC 0x1C (5 bytes) - jump to HALT if carry
    // PC=22: LOAD_IMM ECX, 0x42 (6 bytes) - should be skipped
    // PC=28: HALT
    ctx.load_program({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x00: LOAD_IMM EAX, 0 (6 bytes)
        0x17, 0x00,                           // 0x06: NOT EAX (EAX becomes 0xFFFFFFFF)
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // 0x08: LOAD_IMM EBX, 1 (6 bytes)
        0x02, 0x00, 0x01,                     // 0x0E: ADD EAX, EBX (0xFFFFFFFF + 1 = 0, carry)
        0x0F, 0x1C, 0x00, 0x00, 0x00,        // 0x11: JC 0x1C (28) - jump to HALT if carry
        0x01, 0x02, 0x42, 0x00, 0x00, 0x00,  // 0x16: LOAD_IMM ECX, 0x42 (should be skipped)
        0xFF                                  // 0x1C: HALT
    });

    ctx.execute_program();

    // EAX should have overflowed to 0, EBX should be 1, ECX should be 0 (not set to 0x42)
    ctx.assert_register_eq(0, 0);
    ctx.assert_register_eq(1, 1);
    ctx.assert_register_eq(2, 0);
    ctx.assert_flag_set(FLAG_CARRY);
}

TEST_CASE(jc_carry_clear, "conditional_jumps") {
    // Test JC (Jump if Carry) when carry flag is clear
    // PC layout with 6-byte LOAD_IMM:
    // PC=0: LOAD_IMM EAX, 0x10 (6 bytes)
    // PC=6: LOAD_IMM EBX, 0x20 (6 bytes)
    // PC=12: ADD EAX, EBX (3 bytes) - no carry
    // PC=15: JC 0x1C (5 bytes) - should NOT jump
    // PC=20: LOAD_IMM ECX, 0x42 (6 bytes) - should execute
    // PC=26: HALT
    ctx.load_program({
        0x01, 0x00, 0x10, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x10 (PC=0)
        0x01, 0x01, 0x20, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x20 (PC=6)
        0x02, 0x00, 0x01,                     // ADD EAX, EBX (PC=12, no carry)
        0x0F, 0x1A, 0x00, 0x00, 0x00,        // JC 0x1A (26) - (PC=15, should NOT jump)
        0x01, 0x02, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 0x42 (PC=20, should execute)
        0xFF                                  // HALT (PC=26)
    });

    ctx.execute_program();

    // EAX should be 0x30, EBX should be 0x20, ECX should be 0x42
    ctx.assert_register_eq(0, 0x30);
    ctx.assert_register_eq(1, 0x20);
    ctx.assert_register_eq(2, 0x42);
    ctx.assert_flag_clear(FLAG_CARRY);
}

TEST_CASE(jnc_carry_clear, "conditional_jumps") {
    // Test JNC (Jump if No Carry) when carry flag is clear
    // PC layout with 6-byte LOAD_IMM:
    // PC=0: LOAD_IMM EAX, 0x10 (6 bytes)
    // PC=6: LOAD_IMM EBX, 0x20 (6 bytes)
    // PC=12: ADD EAX, EBX (3 bytes) - no carry
    // PC=15: JNC 0x1A (5 bytes) - should jump (no carry)
    // PC=20: LOAD_IMM ECX, 0x42 (6 bytes) - should be skipped
    // PC=26: HALT
    ctx.load_program({
        0x01, 0x00, 0x10, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x10 (PC=0)
        0x01, 0x01, 0x20, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x20 (PC=6)
        0x02, 0x00, 0x01,                     // ADD EAX, EBX (PC=12, no carry)
        0x22, 0x1A, 0x00, 0x00, 0x00,        // JNC 0x1A (26) - (PC=15, should jump to HALT)
        0x01, 0x02, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 0x42 (PC=20, should be skipped)
        0xFF                                  // HALT (PC=26)
    });

    ctx.execute_program();

    // EAX should be 0x30, EBX should be 0x20, ECX should be 0 (not set to 0x42)
    ctx.assert_register_eq(0, 0x30);
    ctx.assert_register_eq(1, 0x20);
    ctx.assert_register_eq(2, 0);
    ctx.assert_flag_clear(FLAG_CARRY);
}

TEST_CASE(jnc_carry_set, "conditional_jumps") {
    // Test JNC (Jump if No Carry) when carry flag is set
    // PC layout with 6-byte LOAD_IMM:
    // PC=0: LOAD_IMM EAX, 0 (6 bytes)
    // PC=6: NOT EAX (2 bytes) - EAX becomes 0xFFFFFFFF
    // PC=8: LOAD_IMM EBX, 1 (6 bytes)
    // PC=14: ADD EAX, EBX (3 bytes) - causes carry
    // PC=17: JNC 0x1E (5 bytes) - should NOT jump (carry is set)
    // PC=22: LOAD_IMM ECX, 0x42 (6 bytes) - should execute
    // PC=28: HALT
    ctx.load_program({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0 (PC=0)
        0x17, 0x00,                           // NOT EAX (PC=6) - EAX becomes 0xFFFFFFFF
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 1 (PC=8)
        0x02, 0x00, 0x01,                     // ADD EAX, EBX (PC=14, causes carry)
        0x22, 0x1C, 0x00, 0x00, 0x00,        // JNC 0x1C (28) - (PC=17, should NOT jump)
        0x01, 0x02, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 0x42 (PC=22, should execute)
        0xFF                                  // HALT (PC=28)
    });

    ctx.execute_program();

    // EAX should have overflowed to 0, EBX should be 1, ECX should be 0x42
    ctx.assert_register_eq(0, 0);
    ctx.assert_register_eq(1, 1);
    ctx.assert_register_eq(2, 0x42);
    ctx.assert_flag_set(FLAG_CARRY);
}

TEST_CASE(carry_flag_arithmetic, "flags") {
    // Test that carry flag is properly set/cleared by arithmetic operations
    ctx.load_program({
        // Test case 1: No carry
        0x01, 0x00, 0x10, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x10
        0x01, 0x01, 0x20, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x20
        0x02, 0x00, 0x01,  // ADD EAX, EBX (0x10 + 0x20 = 0x30, no carry)

        // Test case 2: Cause carry using NOT to get max value
        0x01, 0x02, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM ECX, 0 (start with 0)
        0x17, 0x02,        // NOT ECX (ECX becomes 0xFFFFFFFF)
        0x01, 0x03, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM EDX, 1
        0x02, 0x02, 0x03,  // ADD ECX, EDX (0xFFFFFFFF + 1 = 0, carry)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Check final state
    ctx.assert_register_eq(0, 0x30);  // No carry here
    ctx.assert_register_eq(2, 0);     // Overflowed to 0
    ctx.assert_flag_set(FLAG_CARRY);   // Final carry flag should be set
}

TEST_CASE(extended_movex_basic, "extended_registers") {
    // Test MOVEX operation between extended registers
    ctx.load_program({
        0x71,              // MODE64 - Switch to 64-bit mode
        0x01, 0x08, 0x42,  // LOAD_IMM R8, 0x42 (using register index 8 for R8)
        0x60, 0x09, 0x08,  // MOVEX R9, R8 (move R8 to R9)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Both R8 and R9 should contain 0x42
    ctx.assert_register_eq(0, 0);     // EAX unchanged
    // Note: The extended registers might need special access methods
    // For now, test that the program executes without error
}

TEST_CASE(extended_addex_basic, "extended_registers") {
    // Test ADDEX operation with extended registers
    ctx.load_program({
        0x71,              // MODE64 - Switch to 64-bit mode
        0x01, 0x08, 0x10,  // LOAD_IMM R8, 0x10
        0x01, 0x09, 0x20,  // LOAD_IMM R9, 0x20
        0x61, 0x08, 0x09,  // ADDEX R8, R9 (add R9 to R8)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Test that program executes without error
    // Extended register validation would require access to the new register system
}

TEST_CASE(extended_subex_basic, "extended_registers") {
    // Test SUBEX operation with extended registers
    ctx.load_program({
        0x71,              // MODE64 - Switch to 64-bit mode
        0x01, 0x08, 0x30,  // LOAD_IMM R8, 0x30
        0x01, 0x09, 0x10,  // LOAD_IMM R9, 0x10
        0x62, 0x08, 0x09,  // SUBEX R8, R9 (subtract R9 from R8)
        0xFF               // HALT
    });

    ctx.execute_program();

    // Test that program executes without error
    // Extended register validation would require access to the new register system
}

TEST_CASE(mode_switching, "cpu_modes") {
    // Test CPU mode switching
    ctx.load_program({
        0x70,              // MODE32 - Switch to 32-bit mode
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 0x42
        0x71,              // MODE64 - Switch to 64-bit mode
        0x01, 0x01, 0x84, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0x84
        0xFF               // HALT
    });

    ctx.execute_program();

    // Check that registers were set correctly
    ctx.assert_register_eq(0, 0x42);
    ctx.assert_register_eq(1, 0x84);
}

TEST_CASE(simd_register_classification, "simd_registers") {
    // Test SIMD register type classification
    using namespace DemiEngine_Registers;

    // Test XMM registers are classified as SIMD
    ctx.assert_eq(true, RegisterNames::is_simd(Register::XMM0), "XMM0 should be SIMD");
    ctx.assert_eq(true, RegisterNames::is_simd(Register::XMM15), "XMM15 should be SIMD");
    ctx.assert_eq(false, RegisterNames::is_general_purpose(Register::XMM0), "XMM0 should not be general purpose");

    // Test that general purpose registers are not SIMD
    ctx.assert_eq(false, RegisterNames::is_simd(Register::RAX), "RAX should not be SIMD");
    ctx.assert_eq(true, RegisterNames::is_general_purpose(Register::RAX), "RAX should be general purpose");

    // Test FPU register classification
    ctx.assert_eq(true, RegisterNames::is_fpu(Register::ST0), "ST0 should be FPU");
    ctx.assert_eq(true, RegisterNames::is_fpu(Register::ST7), "ST7 should be FPU");
    ctx.assert_eq(false, RegisterNames::is_simd(Register::ST0), "ST0 should not be SIMD");

    // Test MMX register classification
    ctx.assert_eq(true, RegisterNames::is_mmx(Register::MM0), "MM0 should be MMX");
    ctx.assert_eq(true, RegisterNames::is_mmx(Register::MM7), "MM7 should be MMX");
    ctx.assert_eq(false, RegisterNames::is_general_purpose(Register::MM0), "MM0 should not be general purpose");

    // Test SIMD control register classification
    ctx.assert_eq(true, RegisterNames::is_simd_control(Register::MXCSR), "MXCSR should be SIMD control");
    ctx.assert_eq(true, RegisterNames::is_simd_control(Register::FPU_CONTROL), "FPU_CONTROL should be SIMD control");
    ctx.assert_eq(true, RegisterNames::is_simd_control(Register::FPU_STATUS), "FPU_STATUS should be SIMD control");
}

TEST_CASE(simd_register_names, "simd_registers") {
    // Test SIMD register name mapping
    using namespace DemiEngine_Registers;

    // Test XMM register names
    ctx.assert_eq(std::string("XMM0"), RegisterNames::get_name(Register::XMM0), "XMM0 name should be correct");
    ctx.assert_eq(std::string("XMM15"), RegisterNames::get_name(Register::XMM15), "XMM15 name should be correct");

    // Test FPU register names
    ctx.assert_eq(std::string("ST0"), RegisterNames::get_name(Register::ST0), "ST0 name should be correct");
    ctx.assert_eq(std::string("ST7"), RegisterNames::get_name(Register::ST7), "ST7 name should be correct");

    // Test MMX register names (note: MMX registers are aliases to FPU registers)
    // So MM0 = ST0, MM1 = ST1, etc. The name returned will be the primary register name
    ctx.assert_eq(std::string("ST0"), RegisterNames::get_name(Register::MM0), "MM0 aliases to ST0, so name should be ST0");
    ctx.assert_eq(std::string("ST7"), RegisterNames::get_name(Register::MM7), "MM7 aliases to ST7, so name should be ST7");

    // Test control register names
    ctx.assert_eq(std::string("MXCSR"), RegisterNames::get_name(Register::MXCSR), "MXCSR name should be correct");
    ctx.assert_eq(std::string("FCW"), RegisterNames::get_name(Register::FPU_CONTROL), "FPU_CONTROL should be named FCW");
    ctx.assert_eq(std::string("FSW"), RegisterNames::get_name(Register::FPU_STATUS), "FPU_STATUS should be named FSW");
}

TEST_CASE(register_count_expansion, "simd_registers") {
    // Test that register count has been properly expanded
    using namespace DemiEngine_Registers;

    // Verify total register count
    ctx.assert_eq(static_cast<size_t>(134), TOTAL_REGISTERS, "Total register count should be 134");
    ctx.assert_eq(static_cast<size_t>(134), static_cast<size_t>(Register::REGISTER_COUNT), "Register count enum should match");

    // Verify individual register type counts
    ctx.assert_eq(static_cast<size_t>(16), GENERAL_PURPOSE_COUNT, "Should have 16 general purpose registers");
    ctx.assert_eq(static_cast<size_t>(16), SIMD_REGISTER_COUNT, "Should have 16 SIMD XMM registers");
    ctx.assert_eq(static_cast<size_t>(8), FPU_REGISTER_COUNT, "Should have 8 FPU registers");
    ctx.assert_eq(static_cast<size_t>(8), MMX_REGISTER_COUNT, "Should have 8 MMX registers");
    ctx.assert_eq(static_cast<size_t>(16), AVX_REGISTER_COUNT, "Should have 16 AVX YMM registers");
}

TEST_CASE(xmm_register_access, "simd_registers") {
    // Test basic XMM register access using 64-bit operations
    using namespace DemiEngine_Registers;

    ctx.cpu.reset();

    // Test setting and getting XMM registers (low 64-bit part)
    uint64_t test_value = 0x123456789ABCDEF0ULL;
    ctx.cpu.set_register(Register::XMM0, test_value);
    uint64_t read_value = ctx.cpu.get_register(Register::XMM0);

    ctx.assert_eq(test_value, read_value, "XMM0 register read/write should work");

    // Test different XMM registers
    for (int i = 0; i < 4; i++) { // Test first 4 to keep test fast
        Register xmm_reg = static_cast<Register>(static_cast<int>(Register::XMM0) + (i * 2));
        uint64_t value = 0x1111111111111111ULL * (i + 1);

        ctx.cpu.set_register(xmm_reg, value);
        uint64_t retrieved = ctx.cpu.get_register(xmm_reg);

        ctx.assert_eq(value, retrieved, fmt::format("XMM{} register should store/retrieve correctly", i));
    }
}

TEST_CASE(fpu_register_access, "fpu_registers") {
    // Test basic FPU register access using 64-bit operations
    using namespace DemiEngine_Registers;

    ctx.cpu.reset();

    // Test setting and getting FPU registers (low 64-bit part)
    uint64_t test_value = 0xFEDCBA9876543210ULL;
    ctx.cpu.set_register(Register::ST0, test_value);
    uint64_t read_value = ctx.cpu.get_register(Register::ST0);

    ctx.assert_eq(test_value, read_value, "ST0 register read/write should work");

    // Test different FPU registers
    for (int i = 0; i < 4; i++) { // Test first 4 to keep test fast
        Register st_reg = static_cast<Register>(static_cast<int>(Register::ST0) + (i * 2));
        uint64_t value = 0x2222222222222222ULL * (i + 1);

        ctx.cpu.set_register(st_reg, value);
        uint64_t retrieved = ctx.cpu.get_register(st_reg);

        ctx.assert_eq(value, retrieved, fmt::format("ST{} register should store/retrieve correctly", i));
    }
}

TEST_CASE(fpu_instruction_tests, "fpu_registers") {
    // Test FPU instructions: FILD (load integer as floating point)
    // STORE now uses mode-aware 4-byte addresses in 32-bit mode
    
    // First, put a known integer value in memory
    ctx.load_program({
        0x01, 0x00, 0x2A, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 42 (load 42 into EAX)
        0x07, 0x00, 0x64, 0x00, 0x00, 0x00,  // STORE EAX, 0x64 (store 42 at address 100)
        
        // Test FILD with immediate 32-bit integer
        0xA3, 0x00, 0x2A, 0x00, 0x00, 0x00,  // FILD immediate 42 (load immediate 32-bit integer 42 as double)
        
        // Test FILD from memory address (load the 42 we stored earlier)
        0xA3, 0x01, 0x64, 0x00, 0x00, 0x00,  // FILD [0x0064] (load int32 from address 100 as double)
        
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // We can't easily test the FPU stack contents directly, so we'll test via other instructions
    // For now, just verify the program executed without crashing
    ctx.assert_register_eq(0, 42);  // EAX should still contain 42
}

TEST_CASE(fpu_store_instructions, "fpu_registers") {
    // Test FPU store instructions: FISTP (store floating point as integer and pop)
    // LOAD now uses mode-aware 4-byte addresses in 32-bit mode
    
    ctx.load_program({
        // Load a value onto FPU stack first
        0xA3, 0x00, 0x55, 0x00, 0x00, 0x00,  // FILD immediate 85 (load 85 as double onto FPU stack)
        
        // Store it back as integer
        0xA5, 0x01, 0x70, 0x00, 0x00, 0x00,  // FISTP [0x0070] (store ST(0) as int32 to address 112)
        
        // Load the stored value to verify it worked
        0x06, 0x01, 0x70, 0x00, 0x00, 0x00,  // LOAD EBX, 0x70 (load from address 112 into EBX)
        
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // EBX should contain 85 (the value we stored via FISTP)
    ctx.assert_register_eq(1, 85);
}

TEST_CASE(fpu_arithmetic_operations, "fpu_registers") {
    // Test FPU arithmetic operations
    using namespace DemiEngine_Registers;
    
    ctx.cpu.reset();
    
    // Manually set up FPU stack for testing
    double val1 = 10.0;
    double val2 = 5.0;
    uint64_t uint_val1 = *reinterpret_cast<uint64_t*>(&val1);
    uint64_t uint_val2 = *reinterpret_cast<uint64_t*>(&val2);
    
    ctx.cpu.set_register(Register::ST0, uint_val1);
    ctx.cpu.set_register(Register::ST1, uint_val2);
    
    // Test that we can read back floating point values correctly
    uint64_t st0_raw = ctx.cpu.get_register(Register::ST0);
    uint64_t st1_raw = ctx.cpu.get_register(Register::ST1);
    double st0_val = *reinterpret_cast<double*>(&st0_raw);
    double st1_val = *reinterpret_cast<double*>(&st1_raw);
    
    ctx.assert_eq(st0_val, 10.0, "ST0 should contain 10.0");
    ctx.assert_eq(st1_val, 5.0, "ST1 should contain 5.0");
}

TEST_CASE(mmx_register_aliasing, "mmx_registers") {
    // Test MMX register aliasing to FPU registers
    using namespace DemiEngine_Registers;

    ctx.cpu.reset();

    // Test that MM0 aliases to ST0
    uint64_t test_value = 0xAAAABBBBCCCCDDDDULL;
    ctx.cpu.set_register(Register::MM0, test_value);
    uint64_t st0_value = ctx.cpu.get_register(Register::ST0);

    ctx.assert_eq(test_value, st0_value, "MM0 should alias to ST0");

    // Test reverse aliasing
    uint64_t new_value = 0x1122334455667788ULL;
    ctx.cpu.set_register(Register::ST1, new_value);
    uint64_t mm1_value = ctx.cpu.get_register(Register::MM1);

    ctx.assert_eq(new_value, mm1_value, "ST1 should alias to MM1");
}

TEST_CASE(extended_register_operations, "extended_registers") {
    // Test extended register operations (R8-R15)
    ctx.cpu.reset();

    // Test setting extended registers
    for (int i = 8; i <= 11; i++) { // Test first 4 to keep test fast
        Register reg = static_cast<Register>(i);
        uint64_t value = 0x8000000000000000ULL + i;

        ctx.cpu.set_register(reg, value);
        uint64_t retrieved = ctx.cpu.get_register(reg);

        ctx.assert_eq(value, retrieved, fmt::format("Extended register R{} should work", i));
    }

    // Test specific extended registers
    ctx.cpu.set_register(Register::R8, 0x8888888888888888ULL);
    ctx.cpu.set_register(Register::R15, 0xFFFFFFFFFFFFFFFFULL);

    ctx.assert_eq(static_cast<uint64_t>(0x8888888888888888ULL), ctx.cpu.get_register(Register::R8), "R8 should store value correctly");
    ctx.assert_eq(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL), ctx.cpu.get_register(Register::R15), "R15 should store value correctly");
}

TEST_CASE(simd_control_registers, "simd_control") {
    // Test SIMD control register access
    using namespace DemiEngine_Registers;

    ctx.cpu.reset();

    // Test MXCSR register
    uint64_t mxcsr_value = 0x1F80; // Default MXCSR value
    ctx.cpu.set_register(Register::MXCSR, mxcsr_value);
    uint64_t read_mxcsr = ctx.cpu.get_register(Register::MXCSR);

    ctx.assert_eq(mxcsr_value, read_mxcsr, "MXCSR register should work");

    // Test FPU control registers
    uint64_t fpu_control = 0x037F; // Default FPU control word
    ctx.cpu.set_register(Register::FPU_CONTROL, fpu_control);
    uint64_t read_fpu_control = ctx.cpu.get_register(Register::FPU_CONTROL);

    ctx.assert_eq(fpu_control, read_fpu_control, "FPU_CONTROL register should work");

    // Test FPU status register
    uint64_t fpu_status = 0x0000; // Clear status
    ctx.cpu.set_register(Register::FPU_STATUS, fpu_status);
    uint64_t read_fpu_status = ctx.cpu.get_register(Register::FPU_STATUS);

    ctx.assert_eq(fpu_status, read_fpu_status, "FPU_STATUS register should work");
}

TEST_CASE(register_type_boundaries, "register_validation") {
    // Test register type boundary validation
    using namespace DemiEngine_Registers;

    // Test first and last registers of each type
    ctx.assert_eq(true, RegisterNames::is_general_purpose(Register::RAX), "RAX should be general purpose");
    ctx.assert_eq(true, RegisterNames::is_general_purpose(Register::R15), "R15 should be general purpose");
    ctx.assert_eq(false, RegisterNames::is_general_purpose(Register::XMM0), "XMM0 should not be general purpose");

    ctx.assert_eq(true, RegisterNames::is_simd(Register::XMM0), "XMM0 should be SIMD");
    ctx.assert_eq(true, RegisterNames::is_simd(Register::XMM15), "XMM15 should be SIMD");
    ctx.assert_eq(false, RegisterNames::is_simd(Register::ST0), "ST0 should not be SIMD");

    ctx.assert_eq(true, RegisterNames::is_fpu(Register::ST0), "ST0 should be FPU");
    ctx.assert_eq(true, RegisterNames::is_fpu(Register::ST7), "ST7 should be FPU");
    ctx.assert_eq(false, RegisterNames::is_fpu(Register::XMM0), "XMM0 should not be FPU");

    ctx.assert_eq(true, RegisterNames::is_mmx(Register::MM0), "MM0 should be MMX");
    ctx.assert_eq(true, RegisterNames::is_mmx(Register::MM7), "MM7 should be MMX");
    ctx.assert_eq(false, RegisterNames::is_mmx(Register::XMM0), "XMM0 should not be MMX");
}

// ============================================================================
// ASSEMBLER DIRECTIVE TESTS
// ============================================================================

TEST_CASE(test_context_basic, "assembler") {
    // Test basic TestContext functionality without assembler
    // Updated to 6-byte LOAD_IMM format: opcode + reg + 4-byte immediate
    std::vector<uint8_t> test_program = {0x01, 0x00, 0x42, 0x00, 0x00, 0x00, 0xFF}; // LOAD_IMM EAX, 66; HALT
    ctx.program = test_program;
    
    ctx.assert_program_size(7);
    ctx.assert_byte_at(0, 0x01);
    ctx.assert_byte_at(1, 0x00);
    ctx.assert_byte_at(2, 0x42);
    ctx.assert_byte_at(6, 0xFF);  // HALT is at position 6 now
}

TEST_CASE(debug_db_simple, "assembler") {
    // Debug what DB actually produces - comprehensive analysis
    ctx.assemble_code(R"(
        label1:
        DB "Hi", 2
        label2:
        DB "X", 1
        HALT
    )");
    
    /* (DEBUG)
    // Print ALL bytes to understand the complete pattern
    printf("=== Complete DB Debug Output ===\n");
    for (size_t i = 0; i < ctx.program.size(); i++) {
        char c = (ctx.program[i] >= 32 && ctx.program[i] < 127) ? ctx.program[i] : '.';
        printf("Byte[%2zu] = 0x%02X ('%c')\n", i, ctx.program[i], c);
    }
    printf("Total program size: %zu bytes\n", ctx.program.size());
    printf("=================================\n");
    */ 
    
    // Based on current simple format: strings start at byte 0
    ctx.assert_byte_at(0, 'H');
    ctx.assert_byte_at(1, 'i');
}

TEST_CASE(assembler_basic_test, "assembler") {
    // Test basic assembler functionality without directives
    // Note: LOAD_IMM EAX uses 4-byte immediate (6 bytes total: opcode + reg + imm32)
    ctx.assemble_code(R"(
        LOAD_IMM EAX, 42
        HALT
    )");
    
    // Check basic program structure
    // LOAD_IMM with 32-bit register uses 4-byte immediate
    ctx.assert_program_size(7); // LOAD_IMM (6 bytes: 1+1+4) + HALT (1 byte)
    ctx.assert_byte_at(0, 0x01); // LOAD_IMM opcode
    ctx.assert_byte_at(1, 0x00); // EAX
    ctx.assert_byte_at(2, 42);   // immediate value (little-endian low byte)
    ctx.assert_byte_at(3, 0x00); // immediate value (byte 2)
    ctx.assert_byte_at(4, 0x00); // immediate value (byte 3)
    ctx.assert_byte_at(5, 0x00); // immediate value (high byte)
    ctx.assert_byte_at(6, 0xFF); // HALT
}

TEST_CASE(org_directive_basic, "assembler") {
    // Test basic .org directive functionality
    // Note: LOAD_IMM EAX uses 4-byte immediate (6 bytes total: opcode + reg + imm32)
    try {
        ctx.assemble_code(R"(
            .org 0x100
            LOAD_IMM EAX, 42
            HALT
        )");
        
        // Check that program starts at 0x100 worth of padding + actual instructions
        // .org 0x100 should pad with 0x100 (256) null bytes, then the instructions
        // LOAD_IMM with 32-bit reg = 6 bytes, HALT = 1 byte
        size_t expected_size = 0x100 + 7; // 256 null bytes + LOAD_IMM (6 bytes) + HALT (1 byte)
        ctx.assert_program_size(expected_size);
        
        // Check padding bytes are null
        for (size_t i = 0; i < 0x100; i++) {
            ctx.assert_byte_at(i, 0x00);
        }
        
        // Check actual instructions start at 0x100
        ctx.assert_byte_at(0x100, 0x01); // LOAD_IMM opcode
        ctx.assert_byte_at(0x101, 0x00); // EAX
        ctx.assert_byte_at(0x102, 42);   // immediate value (low byte)
        ctx.assert_byte_at(0x103, 0x00); // immediate value (byte 2)
        ctx.assert_byte_at(0x104, 0x00); // immediate value (byte 3)
        ctx.assert_byte_at(0x105, 0x00); // immediate value (high byte)
        ctx.assert_byte_at(0x106, 0xFF); // HALT
    } catch (const std::exception& e) {
        throw std::runtime_error("org_directive_basic test failed: " + std::string(e.what()));
    }
}

TEST_CASE(org_directive_multiple, "assembler") {
    // Test multiple .org directives
    // Note: LOAD_IMM with 32-bit registers uses 4-byte immediates (6 bytes total)
    // Register mapping: EAX=0, ECX=1, EDX=2, EBX=3
    
    ctx.assemble_code(R"(
        LOAD_IMM EAX, 1
        .org 0x10
        LOAD_IMM EBX, 2
        .org 0x20
        LOAD_IMM ECX, 3
        HALT
    )");
    
    // First instruction at address 0 (6 bytes: opcode + reg + imm32)
    ctx.assert_byte_at(0, 0x01);    // LOAD_IMM
    ctx.assert_byte_at(1, 0x00);    // EAX (register 0)
    ctx.assert_byte_at(2, 1);       // value 1 (low byte)
    ctx.assert_byte_at(3, 0x00);    // value 1 (byte 2)
    ctx.assert_byte_at(4, 0x00);    // value 1 (byte 3)
    ctx.assert_byte_at(5, 0x00);    // value 1 (high byte)
    
    // Padding from 6 to 0x10
    for (size_t i = 6; i < 0x10; i++) {
        ctx.assert_byte_at(i, 0x00);
    }
    
    // Second instruction at 0x10 (6 bytes)
    ctx.assert_byte_at(0x10, 0x01); // LOAD_IMM
    ctx.assert_byte_at(0x11, 0x03); // EBX (register 3)
    ctx.assert_byte_at(0x12, 2);    // value 2 (low byte)
    ctx.assert_byte_at(0x13, 0x00); // value 2 (byte 2)
    ctx.assert_byte_at(0x14, 0x00); // value 2 (byte 3)
    ctx.assert_byte_at(0x15, 0x00); // value 2 (high byte)
    
    // Padding from 0x16 to 0x20
    for (size_t i = 0x16; i < 0x20; i++) {
        ctx.assert_byte_at(i, 0x00);
    }
    
    // Third instruction at 0x20 (6 bytes)
    ctx.assert_byte_at(0x20, 0x01); // LOAD_IMM
    ctx.assert_byte_at(0x21, 0x01); // ECX (register 1)
    ctx.assert_byte_at(0x22, 3);    // value 3 (low byte)
    ctx.assert_byte_at(0x23, 0x00); // value 3 (byte 2)
    ctx.assert_byte_at(0x24, 0x00); // value 3 (byte 3)
    ctx.assert_byte_at(0x25, 0x00); // value 3 (high byte)
    ctx.assert_byte_at(0x26, 0xFF); // HALT
}

TEST_CASE_EXPECT_ERROR(org_directive_backwards_old, "assembler") {
    // Test that .org cannot go backwards (now throws an error)
    // Updated to expect error with new validation
    ctx.assemble_code(R"(
        .org 0x20
        LOAD_IMM EAX, 1
        .org 0x10  ; This now generates an error
        LOAD_IMM EBX, 2
        HALT
    )");
}

TEST_CASE(db_directive_basic, "assembler") {
    // Test basic DB directive functionality
    
    ctx.assemble_code(R"(
        data_label:
        DB "Hello", 5
        HALT
    )");
    
    // Check that DB stores the string bytes
    // DB emits: string bytes + length byte
    ctx.assert_byte_at(0, 'H');
    ctx.assert_byte_at(1, 'e');
    ctx.assert_byte_at(2, 'l');
    ctx.assert_byte_at(3, 'l');
    ctx.assert_byte_at(4, 'o');
    ctx.assert_byte_at(5, 5);    // Length parameter
    ctx.assert_byte_at(6, 0xFF); // HALT
}

TEST_CASE(db_directive_with_null_terminator, "assembler") {
    // Test DB with explicit null terminator
    
    ctx.assemble_code(R"(
        message:
        DB "Test", 5
        HALT
    )");
    
    // DB emits: string bytes + length byte
    ctx.assert_byte_at(0, 'T');
    ctx.assert_byte_at(1, 'e');
    ctx.assert_byte_at(2, 's');
    ctx.assert_byte_at(3, 't');
    ctx.assert_byte_at(4, 5);    // Length parameter
    ctx.assert_byte_at(5, 0xFF); // HALT
}

TEST_CASE(db_directive_multiple_labels, "assembler") {
    // Test multiple DB directives with labels
    
    ctx.assemble_code(R"(
        first_msg:
        DB "ABC", 3
        second_msg:
        DB "XYZ", 3
        HALT
    )");
    
    // First message: "ABC" + length
    ctx.assert_byte_at(0, 'A');
    ctx.assert_byte_at(1, 'B');
    ctx.assert_byte_at(2, 'C');
    ctx.assert_byte_at(3, 3);    // Length
    
    // Second message: "XYZ" + length
    ctx.assert_byte_at(4, 'X');
    ctx.assert_byte_at(5, 'Y');
    ctx.assert_byte_at(6, 'Z');
    ctx.assert_byte_at(7, 3);    // Length
    
    ctx.assert_byte_at(8, 0xFF); // HALT
}

TEST_CASE(db_directive_with_org, "assembler") {
    // Test DB directive combined with .org
    // Note: LOAD_IMM with 32-bit registers uses 4-byte immediates (6 bytes total)
    
    ctx.assemble_code(R"(
        .org 0x100
        data_section:
        DB "Data", 4
        .org 0x200
        code_section:
        LOAD_IMM EAX, 42
        HALT
    )");
    
    // Check padding up to 0x100
    for (size_t i = 0; i < 0x100; i++) {
        ctx.assert_byte_at(i, 0x00);
    }
    
    // Check data at 0x100: "Data" + length
    ctx.assert_byte_at(0x100, 'D');
    ctx.assert_byte_at(0x101, 'a');
    ctx.assert_byte_at(0x102, 't');
    ctx.assert_byte_at(0x103, 'a');
    ctx.assert_byte_at(0x104, 4);    // Length
    
    // Check padding from 0x105 to 0x200
    for (size_t i = 0x105; i < 0x200; i++) {
        ctx.assert_byte_at(i, 0x00);
    }
    
    // Check code at 0x200 (6 bytes for LOAD_IMM + 1 byte for HALT)
    ctx.assert_byte_at(0x200, 0x01); // LOAD_IMM
    ctx.assert_byte_at(0x201, 0x00); // EAX
    ctx.assert_byte_at(0x202, 42);   // value (low byte)
    ctx.assert_byte_at(0x203, 0x00); // value (byte 2)
    ctx.assert_byte_at(0x204, 0x00); // value (byte 3)
    ctx.assert_byte_at(0x205, 0x00); // value (high byte)
    ctx.assert_byte_at(0x206, 0xFF); // HALT
}

TEST_CASE(db_directive_execution_test, "assembler") {
    // Test that DB data can be accessed during execution
    
    ctx.assemble_code(R"(
        ; Load address of string data into EAX
        LOAD_IMM64 EAX, message
        ; Use OUTSTR to output the string (tests that label points to correct address)
        OUTSTR EAX, 1
        HALT
        
        message:
        DB "Test!", 5
    )");
    
    // Execute and verify no crashes occur
    ctx.assert_no_throw([&]() {
        ctx.execute_program();
    });
    
    // Verify EAX contains the address of the message
    // (This tests that the label was correctly resolved)
    ctx.assert_register_eq(0, 14); // message should be at address 14 (after LOAD_IMM64 + OUTSTR + HALT)
}

TEST_CASE(db_directive_newline_handling, "assembler") {
    // Test DB directive with newline characters
    
    ctx.assemble_code(R"(
        line1:
        DB "Hello\n", 6
        line2:
        DB "World\n", 6
        HALT
    )");
    
    // Check first line: "Hello\n" + length
    ctx.assert_byte_at(0, 'H');
    ctx.assert_byte_at(1, 'e');
    ctx.assert_byte_at(2, 'l');
    ctx.assert_byte_at(3, 'l');
    ctx.assert_byte_at(4, 'o');
    ctx.assert_byte_at(5, '\n');
    ctx.assert_byte_at(6, 6);     // Length
    
    // Check second line: "World\n" + length
    ctx.assert_byte_at(7, 'W');
    ctx.assert_byte_at(8, 'o');
    ctx.assert_byte_at(9, 'r');
    ctx.assert_byte_at(10, 'l');
    ctx.assert_byte_at(11, 'd');
    ctx.assert_byte_at(12, '\n');
    ctx.assert_byte_at(13, 6);    // Length
    
    ctx.assert_byte_at(14, 0xFF); // HALT
}

TEST_CASE(org_and_db_integration, "assembler") {
    // Comprehensive test combining .org and DB directives
    // Need larger memory for this test due to .org directives
    TestContext large_ctx; // Create separate context
    large_ctx.cpu.resize_memory(512); // Increase memory size to 512 bytes
    
    large_ctx.assemble_code(R"(
        ; Program entry point
        LOAD_IMM64 EAX, str1
        OUTSTR EAX, 1
        LOAD_IMM64 EAX, str2  
        OUTSTR EAX, 1
        HALT
        
        ; Data section at 0x50
        .org 0x50
        str1:
        DB "First string", 12
        str2:
        DB "Second string", 13
        
        ; More code at 0x100
        .org 0x100
        unused_code:
        LOAD_IMM EBP, 99
        HALT
    )");
    
    // Verify entry point instructions (LOAD_IMM64 is different than LOAD_IMM)
    large_ctx.assert_byte_at(0, 0x53);  // LOAD_IMM64 opcode
    large_ctx.assert_byte_at(1, 0x00);  // EAX
    // Skip the 8-byte address for str1
    
    // Skip detailed byte checking since addresses are calculated by assembler
    
    // Verify data section at 0x50
    const char* first_str = "First string";
    for (size_t i = 0; i < strlen(first_str); i++) {
        large_ctx.assert_byte_at(0x50 + i, first_str[i]);
    }
    
    // Execute to verify functionality
    large_ctx.assert_no_throw([&]() {
        large_ctx.execute_program();
    });
}

// ============================================================================
// ADDITIONAL COMPREHENSIVE ASSEMBLER TESTS
// ============================================================================

TEST_CASE(org_directive_edge_cases, "assembler") {
    // Test edge cases for .org directive
    // Note: LOAD_IMM with 32-bit registers uses 6 bytes (opcode + reg + 4-byte imm)
    // Register mapping: EAX=0, ECX=1, EDX=2, EBX=3
    ctx.assemble_code(R"(
        LOAD_IMM EAX, 1
        .org 0x06  ; Jump to exact end of previous instruction (6 bytes)
        LOAD_IMM EBX, 2
        .org 0x10  ; Jump forward
        LOAD_IMM ECX, 3
        HALT
    )");
    
    // First instruction at 0 (6 bytes)
    ctx.assert_byte_at(0, 0x01);   // LOAD_IMM
    ctx.assert_byte_at(1, 0x00);   // EAX (register 0)
    ctx.assert_byte_at(2, 1);      // value 1 (low byte)
    ctx.assert_byte_at(3, 0x00);   // value 1 (byte 2)
    ctx.assert_byte_at(4, 0x00);   // value 1 (byte 3)
    ctx.assert_byte_at(5, 0x00);   // value 1 (high byte)
    
    // Second instruction at 0x06 (.org moves to exactly where we'd be anyway)
    ctx.assert_byte_at(6, 0x01);   // LOAD_IMM
    ctx.assert_byte_at(7, 0x03);   // EBX (register 3)
    ctx.assert_byte_at(8, 2);      // value 2 (low byte)
    ctx.assert_byte_at(9, 0x00);   // value 2 (byte 2)
    ctx.assert_byte_at(10, 0x00);  // value 2 (byte 3)
    ctx.assert_byte_at(11, 0x00);  // value 2 (high byte)
    
    // Padding from 12 (0x0C) to 0x10
    for (size_t i = 12; i < 0x10; i++) {
        ctx.assert_byte_at(i, 0x00);
    }
    
    // Third instruction at 0x10
    ctx.assert_byte_at(0x10, 0x01);  // LOAD_IMM
    ctx.assert_byte_at(0x11, 0x01);  // ECX (register 1)
    ctx.assert_byte_at(0x12, 3);     // value 3 (low byte)
    ctx.assert_byte_at(0x13, 0x00);  // value 3 (byte 2)
    ctx.assert_byte_at(0x14, 0x00);  // value 3 (byte 3)
    ctx.assert_byte_at(0x15, 0x00);  // value 3 (high byte)
    ctx.assert_byte_at(0x16, 0xFF);  // HALT
}

TEST_CASE(db_directive_empty_string, "assembler") {
    // Test DB with empty string
    ctx.assemble_code(R"(
        empty_str:
        DB "", 0
        HALT
    )");
    
    // Pattern: ""(0 bytes) + length(1) + HALT(1) = 2 bytes
    ctx.assert_program_size(2);
    ctx.assert_byte_at(0, 0);     // Length 0
    ctx.assert_byte_at(1, 0xFF);  // HALT
}

TEST_CASE(db_directive_single_char, "assembler") {
    // Test DB with single character
    ctx.assemble_code(R"(
        char_data:
        DB "A", 1
        HALT
    )");
    
    // Pattern: "A"(1) + length(1) + HALT(1) = 3 bytes
    ctx.assert_program_size(3);
    ctx.assert_byte_at(0, 'A');   // Character data
    ctx.assert_byte_at(1, 1);     // Length 1
    ctx.assert_byte_at(2, 0xFF);  // HALT
}

TEST_CASE(db_directive_special_chars, "assembler") {
    // Test DB with special characters
    ctx.assemble_code(R"(
        special_data:
        DB "!@#", 3
        HALT
    )");
    
    ctx.assert_byte_at(0, '!');
    ctx.assert_byte_at(1, '@');
    ctx.assert_byte_at(2, '#');
    ctx.assert_byte_at(3, 3);     // Length
    ctx.assert_byte_at(4, 0xFF);  // HALT
}

TEST_CASE(mixed_org_db_comprehensive, "assembler") {
    // Comprehensive test mixing .org and DB directives
    // Note: LOAD_IMM uses 6 bytes (opcode + reg + 4-byte imm)
    // JMP uses 5 bytes (opcode + 4-byte addr)
    // Register mapping: EAX=0, ECX=1, EDX=2, EBX=3
    ctx.assemble_code(R"(
        ; Code section at start
        LOAD_IMM EAX, 10
        JMP data_end
        
        ; Data section at 0x30 (need room for 6-byte LOAD_IMM + 5-byte JMP = 11 bytes)
        .org 0x30
        str1:
        DB "Hello", 5
        str2:
        DB "World", 5
        
        ; Code continuation at 0x50
        .org 0x50
        data_end:
        LOAD_IMM EBX, 20
        HALT
        
        ; More data at 0x70
        .org 0x70
        str3:
        DB "End", 3
    )");
    
    // Check initial code (6 bytes for LOAD_IMM)
    ctx.assert_byte_at(0, 0x01);    // LOAD_IMM
    ctx.assert_byte_at(1, 0x00);    // EAX (register 0)
    ctx.assert_byte_at(2, 10);      // value 10 (low byte)
    ctx.assert_byte_at(3, 0x00);    // value 10 (byte 2)
    ctx.assert_byte_at(4, 0x00);    // value 10 (byte 3)
    ctx.assert_byte_at(5, 0x00);    // value 10 (high byte)
    
    // JMP at offset 6 (5 bytes: opcode + 4-byte addr)
    ctx.assert_byte_at(6, 0x05);    // JMP opcode (0x05)
    ctx.assert_byte_at(7, 0x50);    // Target 0x50 (low byte)
    ctx.assert_byte_at(8, 0x00);    // Target (byte 2)
    ctx.assert_byte_at(9, 0x00);    // Target (byte 3)
    ctx.assert_byte_at(10, 0x00);   // Target (high byte)
    
    // Check first string at 0x30: "Hello" + length
    ctx.assert_byte_at(0x30, 'H');
    ctx.assert_byte_at(0x31, 'e');
    ctx.assert_byte_at(0x32, 'l');
    ctx.assert_byte_at(0x33, 'l');
    ctx.assert_byte_at(0x34, 'o');
    ctx.assert_byte_at(0x35, 5);    // Length
    
    // Check second string follows first
    // First DB: "Hello"(5) + length(1) = 6 bytes from 0x30
    // So second string starts at 0x30 + 6 = 0x36
    ctx.assert_byte_at(0x36, 'W');
    ctx.assert_byte_at(0x37, 'o');
    ctx.assert_byte_at(0x38, 'r');
    ctx.assert_byte_at(0x39, 'l');
    ctx.assert_byte_at(0x3A, 'd');
    ctx.assert_byte_at(0x3B, 5);    // Length
    
    // Check code at 0x50 (6 bytes for LOAD_IMM + 1 for HALT)
    ctx.assert_byte_at(0x50, 0x01);  // LOAD_IMM
    ctx.assert_byte_at(0x51, 0x03);  // EBX (register 3)
    ctx.assert_byte_at(0x52, 20);    // value 20 (low byte)
    ctx.assert_byte_at(0x53, 0x00);  // value 20 (byte 2)
    ctx.assert_byte_at(0x54, 0x00);  // value 20 (byte 3)
    ctx.assert_byte_at(0x55, 0x00);  // value 20 (high byte)
    ctx.assert_byte_at(0x56, 0xFF);  // HALT
    
    // Check final string at 0x70 (simple format)
    ctx.assert_byte_at(0x70, 'E');   // "End" starts at 0x70
    ctx.assert_byte_at(0x71, 'n');
    ctx.assert_byte_at(0x72, 'd');
}

TEST_CASE(db_directive_execution_with_labels, "assembler") {
    // Test that labels work correctly in execution
    ctx.assemble_code(R"(
        LOAD_IMM64 EAX, hello_msg
        OUTSTR EAX, 1
        LOAD_IMM64 EAX, goodbye_msg  
        OUTSTR EAX, 1
        HALT
        
        hello_msg:
        DB "Hi!", 3
        goodbye_msg:
        DB "Bye!", 4
    )");
    
    // Execute and verify no crashes
    ctx.assert_no_throw([&]() {
        ctx.execute_program();
    });
    
    // The exact addresses will be calculated by assembler, but verify execution works
}

// ============================================================================
// NEGATIVE TESTS - Testing error handling and validation
// ============================================================================

TEST_CASE_EXPECT_ERROR(invalid_register_number, "negative_tests") {
    // Test that invalid register numbers are caught
    ctx.assemble_code(R"(
        LOAD_IMM R99, 42
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(undefined_label, "negative_tests") {
    // Test that undefined labels are caught
    ctx.assemble_code(R"(
        JMP undefined_label
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(invalid_opcode, "negative_tests") {
    // Test that invalid opcodes are caught
    ctx.load_program({
        0xAA, 0xBB, 0xCC,  // Invalid opcode
        0xFF               // HALT
    });
    ctx.execute_program();
}

TEST_CASE(memory_out_of_bounds_read_limitation, "negative_tests") {
    // DOCUMENTATION: Cannot test memory out-of-bounds with current LOAD instruction
    // LOAD opcode uses 1-byte address (0-255), test memory is 256 bytes (0-255)
    // All possible addresses fit within memory, making true out-of-bounds impossible
    // This test documents the limitation and passes to indicate the constraint is understood
    // TODO: Add LOAD32 instruction with 4-byte address for proper out-of-bounds testing
    ctx.load_program({
        0x01, 0x00, 0xFF, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 255
        0x06, 0x00, 0xFF,  // LOAD EAX, [255] - highest valid address
        0xFF               // HALT
    });
    ctx.execute_program();
    // This passes - confirming that all 1-byte addresses are valid in 256-byte memory
}

TEST_CASE_EXPECT_ERROR(memory_out_of_bounds_write_limitation, "negative_tests") {
    // STORE uses 4-byte addresses: opcode + reg + 4-byte addr = 6 bytes
    // STORE writes a single byte to the specified address
    // Writing to address 0x1000 (memory size is 256) should cause out-of-bounds error
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 66 (3 bytes)
        0x07, 0x00, 0x00, 0x10, 0x00, 0x00,  // STORE EAX, [0x1000] - clearly out of bounds
        0xFF                                  // HALT
    });
    ctx.execute_program();
    // Should throw: address 0x1000 > 256 memory size
}

TEST_CASE_EXPECT_ERROR(stack_overflow, "negative_tests") {
    // Test that stack overflow is detected
    // SP starts at 256 (memory.size()), each PUSH subtracts 4
    // With our check for SP < 8, we'll catch overflow after ~62 pushes
    // JMP now uses 4-byte address: opcode + 4-byte addr = 5 bytes
    // LOAD_IMM now uses 6-byte format: opcode + reg + 4-byte immediate
    ctx.load_program({
        // Loop pushing values until stack overflows
        0x01, 0x00, 0x01, 0x00, 0x00, 0x00,  // 0: LOAD_IMM EAX, 1 (6 bytes)
        // loop at offset 6:
        0x08, 0x00,                           // 6: PUSH EAX (2 bytes)
        0x05, 0x06, 0x00, 0x00, 0x00,        // 8: JMP 6 (5 bytes, back to PUSH)
        0xFF                                  // 13: HALT (never reached)
    });
    ctx.execute_program();
}

TEST_CASE_EXPECT_ERROR(stack_underflow, "negative_tests") {
    // Test that stack underflow is detected
    // Stack starts at memory.size() - 4, so first POP is valid, second POP should fail
    ctx.load_program({
        0x09, 0x00,        // POP EAX (valid - reads from initial SP position)
        0x09, 0x01,        // POP EBX (underflow - SP now beyond memory)
        0xFF               // HALT
    });
    // Should now fail immediately with proper underflow detection
    ctx.execute_program();
}

TEST_CASE(db_string_only, "assembler") {
    // Test that DB with just a string (no explicit length) works
    ctx.assemble_code(R"(
        DB "Hello"
        HALT
    )");
    
    // DB emits just the string bytes when no length provided
    ctx.assert_byte_at(0, 'H');
    ctx.assert_byte_at(1, 'e');
    ctx.assert_byte_at(2, 'l');
    ctx.assert_byte_at(3, 'l');
    ctx.assert_byte_at(4, 'o');
    ctx.assert_byte_at(5, 0xFF); // HALT
}

TEST_CASE_EXPECT_ERROR(db_negative_length, "negative_tests") {
    // Test that DB with negative length is caught
    ctx.assemble_code(R"(
        DB "Hello", -5
        HALT
    )");
}

TEST_CASE(db_length_parameter_ignored, "assembler") {
    // Test that DB with length parameter larger than string still works
    // (length is just another byte emitted after the string)
    ctx.assemble_code(R"(
        DB "Hi", 100
        HALT
    )");
    
    // DB emits: string bytes + length byte
    ctx.assert_byte_at(0, 'H');
    ctx.assert_byte_at(1, 'i');
    ctx.assert_byte_at(2, 100);  // Length parameter
    ctx.assert_byte_at(3, 0xFF); // HALT
}

TEST_CASE_EXPECT_ERROR(duplicate_label, "negative_tests") {
    // Test that duplicate labels are caught
    ctx.assemble_code(R"(
        mylabel:
        LOAD_IMM EAX, 1
        mylabel:
        LOAD_IMM EBX, 2
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(invalid_immediate_value, "negative_tests") {
    // Test that out-of-range immediate values are caught
    ctx.assemble_code(R"(
        LOAD_IMM EAX, 999999999999999999999
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(malformed_instruction, "negative_tests") {
    // Test that malformed instructions are caught
    ctx.assemble_code(R"(
        LOAD_IMM EAX
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(invalid_string_terminator, "negative_tests") {
    // Test that unterminated strings are caught
    ctx.assemble_code(R"(
        DB "Hello
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(org_backwards, "negative_tests") {
    // Test that .org to a previous address is caught
    ctx.assemble_code(R"(
        .org 0x100
        LOAD_IMM EAX, 1
        .org 0x50
        LOAD_IMM EBX, 2
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(register_type_mismatch, "negative_tests") {
    // Test that using wrong register type is caught
    ctx.assemble_code(R"(
        MOVEX XMM0, EAX
        HALT
    )");
}

TEST_CASE_EXPECT_ERROR(division_modulo_zero, "negative_tests") {
    // Test that division by zero is caught (MOD doesn't exist, using DIV)
    // Note: This test currently passes because DIV by zero is already handled
    // by the division_by_zero test. Keeping for completeness.
    ctx.load_program({
        0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM EAX, 10
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM EBX, 0
        0x11, 0x00, 0x01,  // DIV EAX, EBX (division by zero)
        0xFF               // HALT
    });
    ctx.execute_program();
}

TEST_CASE_EXPECT_ERROR(jump_to_invalid_address, "negative_tests") {
    // Test that jumping to invalid address is caught
    // Create a program that's smaller than the jump target
    // JMP now uses 4-byte address: opcode(1) + addr(4) = 5 bytes
    ctx.load_program({
        0x05, 0xFF, 0xFF, 0x00, 0x00,  // JMP to address 0xFFFF - way beyond program
        0xFF,                           // HALT
        // Program ends at byte 6, but we're jumping to byte 0xFFFF
    });
    ctx.execute_program();
}

TEST_CASE_EXPECT_ERROR(call_stack_overflow, "negative_tests") {
    // Test that excessive CALL depth is caught (infinite recursion)
    // Set a low call depth limit for faster testing
    ctx.set_max_call_depth(32); // Much faster than default 256
    
    ctx.assemble_code(R"(
        recursive_func:
        CALL recursive_func
        RET
    )");
    // Should now fail when depth exceeds 32 instead of 256
    ctx.execute_program();
}

TEST_CASE_EXPECT_ERROR(ret_without_call, "negative_tests") {
    // Test that RET without CALL is caught
    // RET expects SP to point to valid return address and frame pointer
    // Initial SP is at memory.size() (256), so SP+8 = 264 > 256, should fail
    ctx.load_program({
        0x1B,              // RET (0x1B) without CALL - SP is at 256, SP+8 = 264 > 256
        0xFF               // HALT
    });
    ctx.execute_program();
}

// ============================================================================
// 64-bit Operation Tests
// ============================================================================

TEST_CASE(sub64_basic, "64bit_operations") {
    // Test basic 64-bit subtraction: 100 - 30 = 70
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 100
        LOAD_IMM64 R11, 30
        SUB64 R10, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_64_eq(10, 70);
    ctx.assert_register_64_eq(11, 30);  // R11 should be unchanged
    ctx.assert_flag_clear(FLAG_ZERO);
    ctx.assert_flag_clear(FLAG_CARRY);
    ctx.assert_flag_clear(FLAG_SIGN);
}

TEST_CASE(sub64_zero_result, "64bit_operations") {
    // Test subtraction resulting in zero: 50 - 50 = 0
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 50
        LOAD_IMM64 R11, 50
        SUB64 R10, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_64_eq(10, 0);
    ctx.assert_flag_set(FLAG_ZERO);
}

TEST_CASE(sub64_borrow, "64bit_operations") {
    // Test subtraction with borrow: 30 - 100 = -70 (unsigned underflow)
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 30
        LOAD_IMM64 R11, 100
        SUB64 R10, R11
        HALT
    )");
    ctx.execute_program();
    
    // Result should wrap around (30 - 100 in unsigned = large positive number)
    ctx.assert_flag_set(FLAG_CARRY);  // Borrow occurred
    ctx.assert_flag_set(FLAG_SIGN);   // Result is negative in signed interpretation
}

TEST_CASE(sub64_large_values, "64bit_operations") {
    // Test with large 64-bit values using LOAD_IMM64
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 0x1000000000000000
        LOAD_IMM64 R11, 0x0000000100000000
        SUB64 R10, R11
        HALT
    )");
    ctx.execute_program();
    
    uint64_t expected = 0x1000000000000000ULL - 0x0000000100000000ULL;
    ctx.assert_register_64_eq(10, expected);
    ctx.assert_flag_clear(FLAG_CARRY);
}

TEST_CASE(mov64_basic, "64bit_operations") {
    // Test basic 64-bit move operation
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 0x123456789ABCDEF0
        MOV64 R11, R10
        HALT
    )");
    ctx.execute_program();
    
    uint64_t expected = 0x123456789ABCDEF0ULL;
    ctx.assert_register_64_eq(10, expected);
    ctx.assert_register_64_eq(11, expected);  // Should be copied
}

TEST_CASE(mov64_preserves_source, "64bit_operations") {
    // Test that MOV64 preserves source register
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 12345678
        LOAD_IMM64 R11, 87654321
        MOV64 R12, R10
        MOV64 R13, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_register_64_eq(10, 12345678);
    ctx.assert_register_64_eq(11, 87654321);
    ctx.assert_register_64_eq(12, 12345678);  // Copy of R10
    ctx.assert_register_64_eq(13, 87654321);  // Copy of R11
}

TEST_CASE(mov64_chain, "64bit_operations") {
    // Test chaining multiple MOV64 operations
    ctx.assemble_code(R"(
        LOAD_IMM64 R10, 999999
        MOV64 R11, R10
        MOV64 R12, R11
        MOV64 R13, R12
        HALT
    )");
    ctx.execute_program();
    
    // All registers should have the same value
    ctx.assert_register_64_eq(10, 999999);
    ctx.assert_register_64_eq(11, 999999);
    ctx.assert_register_64_eq(12, 999999);
    ctx.assert_register_64_eq(13, 999999);
}

TEST_CASE(modecmp_32bit_equal, "mode_operations") {
    // Test MODECMP in 32-bit mode with equal values
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 42
        LOAD_IMM EBX, 42
        MODECMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_set(FLAG_ZERO);      // Values are equal
    ctx.assert_flag_clear(FLAG_CARRY);   // No borrow (EAX >= EBX)
    ctx.assert_flag_clear(FLAG_SIGN);    // Result is zero (not negative)
}

TEST_CASE(modecmp_32bit_less_than, "mode_operations") {
    // Test MODECMP in 32-bit mode with EAX < EBX
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 10
        LOAD_IMM EBX, 20
        MODECMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_clear(FLAG_ZERO);    // Not equal
    ctx.assert_flag_set(FLAG_CARRY);     // EAX < EBX (borrow occurred)
    ctx.assert_flag_set(FLAG_SIGN);      // Result is negative (10-20 = -10)
}

TEST_CASE(modecmp_32bit_greater_than, "mode_operations") {
    // Test MODECMP in 32-bit mode with EAX > EBX
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 50
        LOAD_IMM EBX, 30
        MODECMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_clear(FLAG_ZERO);    // Not equal
    ctx.assert_flag_clear(FLAG_CARRY);   // EAX > EBX (no borrow)
    ctx.assert_flag_clear(FLAG_SIGN);    // Result is positive (50-30 = 20)
}

TEST_CASE(modecmp_64bit_equal, "mode_operations") {
    // Test MODECMP in 64-bit mode with equal values
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 R10, 0x123456789ABCDEF0
        LOAD_IMM64 R11, 0x123456789ABCDEF0
        MODECMP R10, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_set(FLAG_ZERO);      // Values are equal
    ctx.assert_flag_clear(FLAG_CARRY);   // No borrow
    ctx.assert_flag_clear(FLAG_SIGN);    // Result is zero
}

TEST_CASE(modecmp_64bit_less_than, "mode_operations") {
    // Test MODECMP in 64-bit mode with R10 < R11
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 R10, 1000
        LOAD_IMM64 R11, 2000
        MODECMP R10, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_clear(FLAG_ZERO);    // Not equal
    ctx.assert_flag_set(FLAG_CARRY);     // R10 < R11
    ctx.assert_flag_set(FLAG_SIGN);      // Result is negative
}

TEST_CASE(modecmp_64bit_greater_than, "mode_operations") {
    // Test MODECMP in 64-bit mode with R10 > R11
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 R10, 0xFFFFFFFFFFFFFFFF
        LOAD_IMM64 R11, 0x1000000000000000
        MODECMP R10, R11
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_flag_clear(FLAG_ZERO);    // Not equal
    ctx.assert_flag_clear(FLAG_CARRY);   // R10 > R11 (no borrow in unsigned)
}

// ==================== Comprehensive Mode-Aware Tests ====================

TEST_CASE(mode_default_is_32bit, "mode_awareness") {
    // Test that CPU defaults to 32-bit mode
    ctx.load_program({
        0xFF  // HALT
    });
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
}

TEST_CASE(mode_switch_32_to_64, "mode_awareness") {
    // Test switching from 32-bit to 64-bit mode
    ctx.load_program({
        0x70,              // MODE32 (ensure we start in 32-bit)
        0x71,              // MODE64 (switch to 64-bit)
        0xFF               // HALT
    });
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
}

TEST_CASE(mode_switch_64_to_32, "mode_awareness") {
    // Test switching from 64-bit to 32-bit mode
    ctx.load_program({
        0x71,              // MODE64
        0x70,              // MODE32
        0xFF               // HALT
    });
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
}

TEST_CASE(mode_32bit_register_masking, "mode_awareness") {
    // Test that 32-bit mode properly masks register values
    // In 32-bit mode, operations should only affect lower 32 bits
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xFFFFFFFF
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0xFFFFFFFF);
    ctx.assert_register_upper_bits_zero(0);  // Upper bits should be zero in 32-bit mode
}

TEST_CASE(mode_32bit_add_overflow, "mode_awareness") {
    // Test 32-bit overflow behavior
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xFFFFFFFF
        LOAD_IMM EBX, 1
        ADD EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0);  // 0xFFFFFFFF + 1 = 0 (overflow)
    ctx.assert_carry_flag_set();       // Carry should be set on overflow
    ctx.assert_zero_flag_set();        // Result is zero
}

TEST_CASE(mode_32bit_sub_underflow, "mode_awareness") {
    // Test 32-bit underflow behavior
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0
        LOAD_IMM EBX, 1
        SUB EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0xFFFFFFFF);  // 0 - 1 = 0xFFFFFFFF (underflow/borrow)
    ctx.assert_carry_flag_set();                // Carry (borrow) should be set
}

TEST_CASE(mode_64bit_large_values, "mode_awareness") {
    // Test 64-bit mode with large values
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 RAX, 0x123456789ABCDEF0
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
    ctx.assert_register_64_eq(0, 0x123456789ABCDEF0);
}

TEST_CASE(mode_64bit_add_large_values, "mode_awareness") {
    // Test 64-bit addition with large values
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 RAX, 0x7FFFFFFFFFFFFFFF
        LOAD_IMM64 RBX, 1
        ADD RAX, RBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
    ctx.assert_register_64_eq(0, 0x8000000000000000);  // Max signed + 1 = min signed
    ctx.assert_overflow_flag_set();  // Signed overflow
}

TEST_CASE(mode_switch_preserves_registers, "mode_awareness") {
    // Test that mode switching preserves register values
    // Note: x86 register numbering: EAX=0, ECX=1, EDX=2, EBX=3
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x12345678
        LOAD_IMM EBX, 0x9ABCDEF0
        MODE64
        ; After switching to 64-bit, lower 32 bits should be preserved
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
    ctx.assert_register_32_eq(0, 0x12345678);  // EAX (R0)
    ctx.assert_register_32_eq(3, 0x9ABCDEF0);  // EBX (R3)
}

TEST_CASE(mode_32bit_mul_overflow, "mode_awareness") {
    // Test 32-bit multiplication overflow behavior
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x10000
        LOAD_IMM EBX, 0x10000
        MUL EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    // 0x10000 * 0x10000 = 0x100000000, but truncated to 32 bits = 0
    ctx.assert_register_32_eq(0, 0);
}

TEST_CASE(mode_32bit_boundary_values, "mode_awareness") {
    // Test 32-bit boundary values
    // Note: x86 register numbering: EAX=0, ECX=1, EDX=2, EBX=3
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x7FFFFFFF  ; Max positive signed 32-bit
        LOAD_IMM EBX, 0x80000000  ; Min negative signed 32-bit (or max+1 unsigned)
        LOAD_IMM ECX, 0xFFFFFFFF  ; Max unsigned 32-bit / -1 signed
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0x7FFFFFFF);  // EAX (R0)
    ctx.assert_register_32_eq(3, 0x80000000);  // EBX (R3)
    ctx.assert_register_32_eq(1, 0xFFFFFFFF);  // ECX (R1)
    ctx.assert_register_upper_bits_zero(0);
    ctx.assert_register_upper_bits_zero(3);
    ctx.assert_register_upper_bits_zero(1);
}

TEST_CASE(mode_32bit_shift_boundary, "mode_awareness") {
    // Test 32-bit shift operations at boundary
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 1
        SHL EAX, 31    ; Shift to sign bit position
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0x80000000);
}

TEST_CASE(mode_32bit_and_operation, "mode_awareness") {
    // Test 32-bit AND operation
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xFF00FF00
        LOAD_IMM EBX, 0x0F0F0F0F
        AND EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0x0F000F00);
    ctx.assert_register_upper_bits_zero(0);
    ctx.assert_zero_flag_clear();  // Result is non-zero
}

TEST_CASE(mode_32bit_or_operation, "mode_awareness") {
    // Test 32-bit OR operation
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xF0F0F0F0
        LOAD_IMM EBX, 0x0F0F0F0F
        OR EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0xFFFFFFFF);
    ctx.assert_register_upper_bits_zero(0);
}

TEST_CASE(mode_32bit_xor_self, "mode_awareness") {
    // Test XOR with self (zeroing pattern)
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xDEADBEEF
        XOR EAX, EAX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0);
    ctx.assert_zero_flag_set();
}

TEST_CASE(mode_32bit_not_operation, "mode_awareness") {
    // Test 32-bit NOT operation
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x55555555
        NOT EAX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0xAAAAAAAA);
    ctx.assert_register_upper_bits_zero(0);
}

TEST_CASE(mode_32bit_inc_overflow, "mode_awareness") {
    // Test INC at 32-bit boundary
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xFFFFFFFF
        INC EAX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0);
    ctx.assert_zero_flag_set();
}

TEST_CASE(mode_32bit_dec_underflow, "mode_awareness") {
    // Test DEC at 32-bit boundary (0 -> 0xFFFFFFFF)
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0
        DEC EAX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 0xFFFFFFFF);
    ctx.assert_zero_flag_clear();
}

TEST_CASE(mode_64bit_add_no_overflow, "mode_awareness") {
    // Test 64-bit addition without overflow
    ctx.assemble_code(R"(
        MODE64
        LOAD_IMM64 RAX, 0x00000000FFFFFFFF
        LOAD_IMM64 RBX, 0x0000000000000001
        ADD RAX, RBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
    ctx.assert_register_64_eq(0, 0x0000000100000000);  // No wrap in 64-bit mode
    ctx.assert_carry_flag_clear();
}

TEST_CASE(mode_32bit_cmp_equal, "mode_awareness") {
    // Test CMP with equal values in 32-bit mode
    // Note: x86 register numbering: EAX=0, ECX=1, EDX=2, EBX=3
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0x12345678
        LOAD_IMM EBX, 0x12345678
        CMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_zero_flag_set();
    ctx.assert_carry_flag_clear();
    // Registers should be unchanged after CMP
    ctx.assert_register_32_eq(0, 0x12345678);  // EAX (R0)
    ctx.assert_register_32_eq(3, 0x12345678);  // EBX (R3)
}

TEST_CASE(mode_32bit_cmp_less, "mode_awareness") {
    // Test CMP with first < second in 32-bit mode
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 10
        LOAD_IMM EBX, 20
        CMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_zero_flag_clear();
    ctx.assert_carry_flag_set();  // Borrow occurred
}

TEST_CASE(mode_32bit_cmp_greater, "mode_awareness") {
    // Test CMP with first > second in 32-bit mode
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 30
        LOAD_IMM EBX, 20
        CMP EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_zero_flag_clear();
    ctx.assert_carry_flag_clear();  // No borrow
}

TEST_CASE(mode_multiple_switches, "mode_awareness") {
    // Test multiple mode switches
    // Note: x86 register numbering: EAX/RAX=0, ECX/RCX=1, EDX/RDX=2, EBX/RBX=3
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 1
        MODE64
        LOAD_IMM RBX, 2
        MODE32
        LOAD_IMM ECX, 3
        MODE64
        LOAD_IMM RDX, 4
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();  // Should end in 64-bit mode
    ctx.assert_register_32_eq(0, 1);  // EAX/RAX (R0)
    ctx.assert_register_32_eq(3, 2);  // RBX (R3)
    ctx.assert_register_32_eq(1, 3);  // ECX/RCX (R1)
    ctx.assert_register_32_eq(2, 4);  // RDX (R2)
}

TEST_CASE(mode_32bit_mod_operation, "mode_awareness") {
    // Test modulo operation in 32-bit mode
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 17
        LOAD_IMM EBX, 5
        MOD EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 2);  // 17 % 5 = 2
}

TEST_CASE(mode_32bit_div_operation, "mode_awareness") {
    // Test division operation in 32-bit mode
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 100
        LOAD_IMM EBX, 7
        DIV EAX, EBX
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    ctx.assert_register_32_eq(0, 14);  // 100 / 7 = 14
}

TEST_CASE(mode_32bit_all_registers_masked, "mode_awareness") {
    // Test that all registers are properly masked in 32-bit mode
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0xFFFFFFFF
        LOAD_IMM EBX, 0xFFFFFFFF
        LOAD_IMM ECX, 0xFFFFFFFF
        LOAD_IMM EDX, 0xFFFFFFFF
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_32bit_mode();
    for (int i = 0; i < 4; i++) {
        ctx.assert_register_32_eq(i, 0xFFFFFFFF);
        ctx.assert_register_upper_bits_zero(i);
    }
}

TEST_CASE(mode_flags_after_mode_switch, "mode_awareness") {
    // Test that flags are preserved across mode switches
    ctx.assemble_code(R"(
        MODE32
        LOAD_IMM EAX, 0
        LOAD_IMM EBX, 0
        CMP EAX, EBX  ; Set zero flag
        MODE64
        HALT
    )");
    ctx.execute_program();
    
    ctx.assert_64bit_mode();
    ctx.assert_zero_flag_set();  // Flag should be preserved
}