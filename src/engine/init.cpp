#include "../config.hpp"
#include "device_factory.hpp"
#include "device_manager.hpp"
#include "../debug/debug_handler.hpp"

void initialize_devices() {
    using namespace vhw;

    DeviceManager::instance().reset();  // Reset device manager to clear any previous state
    
    // Console on port 0x01
    auto console = DeviceFactory::createConsoleDevice(0x01);  
    
    // Counter on port 0x02
    auto counter = DeviceFactory::createCounterDevice(0x02);
    // Set up initial counter value
    counter->setCounter(42);

    // Create a file device for virtual file I/O
    auto file = DeviceFactory::createFileDevice("virtual_storage/vhd.dat", 0x04);    
    
    // Create a RAM disk device for block storage
    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_RAMDISK, "About to create RAMDisk...", Logging::DebugLevel::DETAIL);
    auto ramdisk = DeviceFactory::createRamDiskDevice(8192, 0x05, 0x06);
    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_RAMDISK, "RAMDisk created successfully", Logging::DebugLevel::DETAIL);

    // Optionally, create a real serial port device if available
    // auto serial = DeviceFactory::createSerialPortDevice("/dev/ttyUSB0", 0x03);

    Logging::DebugHandler::instance().report(Logging::DebugCategory::IO_DEVICE, "Device system initialized with standard and storage devices", Logging::DebugLevel::INFO);
}
