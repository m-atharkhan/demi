#pragma once

#include <cstdint>
#include <queue>
#include <array>
#include <functional>

namespace DemiEngine_Interrupts {

/**
 * @brief CPU Exception types (vectors 0x00-0x1F)
 */
enum class CPUException : uint8_t {
    DIVIDE_BY_ZERO = 0x00,
    DEBUG = 0x01,
    NMI = 0x02,                     // Non-maskable interrupt
    BREAKPOINT = 0x03,
    OVERFLOW = 0x04,
    BOUND_RANGE_EXCEEDED = 0x05,
    INVALID_OPCODE = 0x06,
    DEVICE_NOT_AVAILABLE = 0x07,
    DOUBLE_FAULT = 0x08,
    COPROCESSOR_SEGMENT_OVERRUN = 0x09,
    INVALID_TSS = 0x0A,
    SEGMENT_NOT_PRESENT = 0x0B,
    STACK_OVERFLOW = 0x0C,
    GENERAL_PROTECTION = 0x0D,
    PAGE_FAULT = 0x0E,
    RESERVED_0F = 0x0F,
    FPU_ERROR = 0x10,
    ALIGNMENT_CHECK = 0x11,
    MACHINE_CHECK = 0x12,
    SIMD_EXCEPTION = 0x13
};

/**
 * @brief Hardware interrupt types (vectors 0x20-0x2F)
 */
enum class HardwareInterrupt : uint8_t {
    TIMER = 0x20,                   // System timer
    KEYBOARD = 0x21,
    CASCADE = 0x22,                 // Cascade for slave PIC
    COM2 = 0x23,
    COM1 = 0x24,
    LPT2 = 0x25,
    FLOPPY = 0x26,
    LPT1 = 0x27,
    RTC = 0x28,                     // Real-time clock
    ACPI = 0x29,
    AVAILABLE_1 = 0x2A,
    AVAILABLE_2 = 0x2B,
    MOUSE = 0x2C,
    COPROCESSOR = 0x2D,
    PRIMARY_ATA = 0x2E,
    SECONDARY_ATA = 0x2F
};

/**
 * @brief Special interrupt vectors
 */
constexpr uint8_t INT_SYSCALL = 0x80;       // Linux-style system calls
constexpr uint8_t INT_USER_MIN = 0x30;      // User-defined interrupt range start
constexpr uint8_t INT_USER_MAX = 0xFF;      // User-defined interrupt range end

/**
 * @brief Interrupt Controller - Manages interrupt delivery and handling
 */
class InterruptController {
public:
    InterruptController();
    ~InterruptController() = default;

    /**
     * @brief Set the Interrupt Vector Table base address
     * @param address Physical memory address where IVT is located
     */
    void set_ivt_base(uint32_t address);

    /**
     * @brief Get the Interrupt Vector Table base address
     * @return IVT base address
     */
    uint32_t get_ivt_base() const { return ivt_base_; }

    /**
     * @brief Enable interrupts
     */
    void enable_interrupts();

    /**
     * @brief Disable interrupts
     */
    void disable_interrupts();

    /**
     * @brief Check if interrupts are enabled
     * @return true if interrupts are enabled
     */
    bool are_interrupts_enabled() const { return interrupts_enabled_; }

    /**
     * @brief Queue an interrupt for delivery
     * @param vector Interrupt vector number (0x00-0xFF)
     */
    void queue_interrupt(uint8_t vector);

    /**
     * @brief Check if any interrupts are pending
     * @return true if interrupts are queued
     */
    bool has_pending_interrupts() const;

    /**
     * @brief Get the next pending interrupt
     * @return Interrupt vector number, or 0xFF if none pending
     */
    uint8_t get_next_interrupt();

    /**
     * @brief Clear all pending interrupts
     */
    void clear_interrupts();

    /**
     * @brief Trigger a CPU exception
     * @param exception Exception type
     */
    void trigger_exception(CPUException exception);

    /**
     * @brief Trigger a hardware interrupt
     * @param interrupt Hardware interrupt type
     */
    void trigger_hardware_interrupt(HardwareInterrupt interrupt);

    /**
     * @brief Set handler address for a specific interrupt vector
     * @param vector Interrupt vector (0x00-0xFF)
     * @param handler_address Address of interrupt handler code
     */
    void set_handler(uint8_t vector, uint32_t handler_address);

    /**
     * @brief Get handler address for a specific interrupt vector
     * @param vector Interrupt vector (0x00-0xFF)
     * @return Handler address, or 0 if not set
     */
    uint32_t get_handler(uint8_t vector) const;

    /**
     * @brief Check if we're currently in an interrupt handler
     * @return true if in interrupt context
     */
    bool in_interrupt_context() const { return interrupt_nesting_level_ > 0; }

    /**
     * @brief Get current interrupt nesting level
     * @return Nesting level (0 = not in interrupt, >0 = nested)
     */
    uint8_t get_nesting_level() const { return interrupt_nesting_level_; }

    /**
     * @brief Enter interrupt context (increment nesting level)
     */
    void enter_interrupt();

    /**
     * @brief Exit interrupt context (decrement nesting level)
     */
    void exit_interrupt();

    /**
     * @brief Reset the interrupt controller to initial state
     */
    void reset();

    static constexpr uint8_t MAX_NESTING_LEVEL = 8;  // Maximum nested interrupts
    static constexpr size_t MAX_QUEUE_SIZE = 256;    // Maximum pending interrupts

private:
    bool interrupts_enabled_;                       // Global interrupt enable flag
    uint32_t ivt_base_;                            // Interrupt Vector Table base address
    std::queue<uint8_t> interrupt_queue_;          // Pending interrupts
    std::array<uint32_t, 256> interrupt_handlers_; // Handler addresses for each vector
    uint8_t interrupt_nesting_level_;              // Current nesting depth

    /**
     * @brief Check if a vector is a non-maskable interrupt
     * @param vector Interrupt vector
     * @return true if NMI
     */
    bool is_nmi(uint8_t vector) const;
};

} // namespace DemiEngine_Interrupts
