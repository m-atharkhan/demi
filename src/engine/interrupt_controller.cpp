#include "interrupt_controller.hpp"
#include "../debug/debug_handler.hpp"
#include "../debug/error_handler.hpp"
#include <fmt/format.h>

namespace DemiEngine_Interrupts {

InterruptController::InterruptController()
    : interrupts_enabled_(true)
    , ivt_base_(0x0000)
    , interrupt_nesting_level_(0)
{
    // Initialize all handler addresses to 0 (unhandled)
    interrupt_handlers_.fill(0);
}

void InterruptController::set_ivt_base(uint32_t address) {
    ivt_base_ = address;
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        fmt::format("[InterruptController] IVT base set to 0x{:08X}", address));
}

void InterruptController::enable_interrupts() {
    interrupts_enabled_ = true;
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        "[InterruptController] Interrupts enabled (STI)");
}

void InterruptController::disable_interrupts() {
    interrupts_enabled_ = false;
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        "[InterruptController] Interrupts disabled (CLI)");
}

void InterruptController::queue_interrupt(uint8_t vector) {
    if (interrupt_queue_.size() >= MAX_QUEUE_SIZE) {
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::IO_INTERRUPT,
            fmt::format("[InterruptController] Interrupt queue full ({} entries), dropping vector 0x{:02X}",
                MAX_QUEUE_SIZE, vector),
            Logging::DebugLevel::IMPORTANT);
        return;
    }

    // Non-maskable interrupts always queue
    if (is_nmi(vector) || interrupts_enabled_) {
        interrupt_queue_.push(vector);
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::IO_INTERRUPT,
            fmt::format("[InterruptController] Queued interrupt vector 0x{:02X}", vector));
    } else {
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::IO_INTERRUPT,
            fmt::format("[InterruptController] Interrupt 0x{:02X} masked (CLI active)", vector));
    }
}

bool InterruptController::has_pending_interrupts() const {
    return !interrupt_queue_.empty();
}

uint8_t InterruptController::get_next_interrupt() {
    if (interrupt_queue_.empty()) {
        return 0xFF; // No interrupt pending
    }
    
    uint8_t vector = interrupt_queue_.front();
    interrupt_queue_.pop();
    
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        fmt::format("[InterruptController] Delivering interrupt vector 0x{:02X}", vector));
    
    return vector;
}

void InterruptController::clear_interrupts() {
    while (!interrupt_queue_.empty()) {
        interrupt_queue_.pop();
    }
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        "[InterruptController] All pending interrupts cleared");
}

void InterruptController::trigger_exception(CPUException exception) {
    uint8_t vector = static_cast<uint8_t>(exception);
    
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        fmt::format("[InterruptController] CPU Exception triggered: 0x{:02X}", vector),
        Logging::DebugLevel::IMPORTANT);
    
    queue_interrupt(vector);
}

void InterruptController::trigger_hardware_interrupt(HardwareInterrupt interrupt) {
    uint8_t vector = static_cast<uint8_t>(interrupt);
    
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        fmt::format("[InterruptController] Hardware interrupt triggered: 0x{:02X}", vector));
    
    queue_interrupt(vector);
}

void InterruptController::set_handler(uint8_t vector, uint32_t handler_address) {
    interrupt_handlers_[vector] = handler_address;
    
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        fmt::format("[InterruptController] Handler for vector 0x{:02X} set to 0x{:08X}",
            vector, handler_address));
}

uint32_t InterruptController::get_handler(uint8_t vector) const {
    return interrupt_handlers_[vector];
}

void InterruptController::enter_interrupt() {
    if (interrupt_nesting_level_ < MAX_NESTING_LEVEL) {
        interrupt_nesting_level_++;
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::IO_INTERRUPT,
            fmt::format("[InterruptController] Enter interrupt context (nesting level: {})",
                interrupt_nesting_level_));
    } else {
        Logging::ErrorHandler::instance().report_runtime(
            Logging::ErrorCode::CPU_GENERIC,
            fmt::format("[InterruptController] Maximum nesting level reached ({})!",
                MAX_NESTING_LEVEL),
            0,
            "Interrupt nesting overflow");
    }
}

void InterruptController::exit_interrupt() {
    if (interrupt_nesting_level_ > 0) {
        interrupt_nesting_level_--;
        Logging::DebugHandler::instance().report(
            Logging::DebugCategory::IO_INTERRUPT,
            fmt::format("[InterruptController] Exit interrupt context (nesting level: {})",
                interrupt_nesting_level_));
    } else {
        Logging::ErrorHandler::instance().report_runtime(
            Logging::ErrorCode::CPU_GENERIC,
            "[InterruptController] IRET without matching interrupt entry!",
            0,
            "Interrupt exit underflow");
    }
}

void InterruptController::reset() {
    interrupts_enabled_ = true;
    ivt_base_ = 0x0000;
    interrupt_nesting_level_ = 0;
    clear_interrupts();
    interrupt_handlers_.fill(0);
    
    Logging::DebugHandler::instance().report(
        Logging::DebugCategory::IO_INTERRUPT,
        "[InterruptController] Reset complete");
}

bool InterruptController::is_nmi(uint8_t vector) const {
    return vector == static_cast<uint8_t>(CPUException::NMI);
}

} // namespace DemiEngine_Interrupts
