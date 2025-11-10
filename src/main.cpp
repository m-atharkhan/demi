#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <memory>
#include <algorithm>

#if __cplusplus >= 201703L
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "config.hpp"
#include "engine/cpu.hpp"
#include "engine/device_factory.hpp"

// Include the debug framework
#include "debug/logger.hpp"

// Include the test framework
#include "test/test.hpp"
#include "test/test_framework.hpp"
#include "test/assembly_test_executor.hpp"
#include <unistd.h>  // For _exit()

// Include the assembler framework
#include "assembler/demi_assembler.hpp"
#include "assembler/lexer.hpp"
#include "assembler/parser.hpp"
#include "assembler/assembler.hpp"

// For POSIX process execution instead of system()
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


using Logging::Logger;

class ArgParser;

void initialize_devices() {
    using namespace vhw;

    DeviceManager::instance().reset();  // Reset device manager to clear any previous state
    auto console = DeviceFactory::createConsoleDevice(0x01);  // Console on port 0x01

    auto counter = DeviceFactory::createCounterDevice(0x02);  // Counter on port 0x02

    // Set up initial counter value (optional)
    counter->setCounter(42);

    // Create a file device for virtual file I/O
    auto file = DeviceFactory::createFileDevice("virtual_storage/vhd.dat", 0x04);    // Create a RAM disk device for block storage
    Logger::instance().debug() << "About to create RAMDisk..." << std::endl;
    auto ramdisk = DeviceFactory::createRamDiskDevice(8192, 0x05, 0x06);
    Logger::instance().debug() << "RAMDisk created successfully" << std::endl;

    // Optionally, create a real serial port device if available
    // Uncomment and modify the port name as needed for your system
    // auto serial = DeviceFactory::createSerialPortDevice("/dev/ttyUSB0", 0x03);

    Logger::instance().info() << "Device system initialized with standard and storage devices" << std::endl;
}

enum class ArgType { Value, Action };

struct ArgDef {
    std::string name;
    std::string arg;
    std::string alias;
    std::string help;
    ArgType type;
    std::function<void(const std::string&)> value_action; // For value args
    std::function<void()> action;                         // For action args
};

struct ParsedArg {
    std::string name;
    std::string value;
    ArgType type;
};

class ArgParser {
public:
    void add_value_arg(const std::string& name, const std::string& arg, const std::string& alias,
                       const std::string& help, std::function<void(const std::string&)> value_action) {
        args_.push_back({name, arg, alias, help, ArgType::Value, value_action, nullptr});
    }
    void add_action_arg(const std::string& name, const std::string& arg, const std::string& alias,
                        const std::string& help, std::function<void()> action) {
        args_.push_back({name, arg, alias, help, ArgType::Action, nullptr, action});
    }
    void add_bool_arg(const std::string& name, const std::string& arg, const std::string& alias,
                      const std::string& help, std::function<void(bool)> action) {
        args_.push_back({name, arg, alias, help, ArgType::Value,
            [action](const std::string& value) {
                // If value is empty, treat as true (flag style)
                if (value.empty()) action(true);
                else action(value == "true" || value == "1");
            }, nullptr});
    }

    void parse(int argc, char* argv[]) {
        // Phase 1: Collect all parsed arguments and positional arguments
        std::vector<ParsedArg> parsed_args;
        positional_args_.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string token = argv[i];
            bool matched = false;
            
            for (auto& def : args_) {
                // Check for exact match or --arg=value / -a=value format
                bool is_match = false;
                std::string value;

                if (token == def.arg || token == def.alias) {
                    is_match = true;
                } else {
                    // Check for --arg=value format
                    auto eq = token.find('=');
                    if (eq != std::string::npos) {
                        std::string arg_part = token.substr(0, eq);
                        if (arg_part == def.arg || arg_part == def.alias) {
                            is_match = true;
                            value = token.substr(eq + 1);
                        }
                    }
                }

                if (is_match) {
                    matched = true;
                    
                    if (def.type == ArgType::Value) {
                        // If we didn't get value from =, try next argument
                        if (value.empty() && i + 1 < argc && argv[i + 1][0] != '-') {
                            value = argv[++i];
                        }
                        // Store the parsed argument
                        parsed_args.push_back({def.name, value, def.type});
                    } else if (def.type == ArgType::Action) {
                        // Store the parsed action
                        parsed_args.push_back({def.name, "", def.type});
                    }
                    break;
                }
            }
            
            if (!matched) {
                if (token.rfind("-", 0) == 0) {
                    std::cerr << "Unknown argument: " << token << std::endl;
                } else {
                    // This is a positional argument (doesn't start with -)
                    positional_args_.push_back(token);
                }
            }
        }
        
        // Phase 2: Process arguments in a deterministic order
        // Process boolean flags first (debug, verbose, etc.)
        for (const auto& parsed : parsed_args) {
            if (parsed.name == "debug" || parsed.name == "verbose" || parsed.name == "extended_registers" || 
                parsed.name == "memdump" || parsed.name == "gui" || parsed.name == "interactive") {
                auto def = find_arg_def(parsed.name);
                if (def && def->value_action) {
                    def->value_action(parsed.value);
                }
            }
        }
        
        // Process value arguments next (files, paths, etc.)
        for (const auto& parsed : parsed_args) {
            if (parsed.name != "debug" && parsed.name != "verbose" && parsed.name != "extended_registers" && 
                parsed.name != "memdump" && parsed.name != "gui" && parsed.name != "interactive" &&
                parsed.name != "help" && parsed.name != "test" && parsed.name != "unit_test" && 
                parsed.name != "assembly_test" && parsed.name != "assembly_test_quiet") {
                auto def = find_arg_def(parsed.name);
                if (def && def->value_action) {
                    def->value_action(parsed.value);
                }
            }
        }
        
        // Process action arguments last (test modes, help, etc.)
        for (const auto& parsed : parsed_args) {
            if (parsed.name == "help" || parsed.name == "test" || parsed.name == "unit_test" || 
                parsed.name == "assembly_test" || parsed.name == "assembly_test_quiet") {
                auto def = find_arg_def(parsed.name);
                if (def) {
                    if (def->type == ArgType::Action && def->action) {
                        def->action();
                    } else if (def->type == ArgType::Value && def->value_action) {
                        def->value_action(parsed.value);
                    }
                }
            }
        }
    }

    void print_help() const {
        std::cout << "demi-engine Usage: demi-engine [options]" << std::endl;
        for (const auto& def : args_) {
            // Use printf to align arguments and help text
            std::cout << fmt::format("  {:<25} {:<6}  {}\n", def.arg, def.alias, def.help);
        }
    }

private:
    std::vector<ArgDef> args_;
    std::vector<std::string> positional_args_;
    
public:
    const std::vector<std::string>& get_positional_args() const {
        return positional_args_;
    }
    
private:
    // Helper function to find argument definition by name
    ArgDef* find_arg_def(const std::string& name) {
        for (auto& def : args_) {
            if (def.name == name) {
                return &def;
            }
        }
        return nullptr;
    }
};

void run_in_assembly_tests() {
    // Scan tests/ directory for .asm files
    std::vector<std::string> test_files;
    
    // Use filesystem to find all .asm files in tests/
    std::string test_dir = "tests";
    if (fs::exists(test_dir) && fs::is_directory(test_dir)) {
        for (const auto& entry : fs::directory_iterator(test_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".asm") {
                test_files.push_back(entry.path().string());
            }
        }
    }
    
    // Note: Removed examples files from default -at execution
    // To include examples, run with specific file path: ./bin/demi-engine -at examples/test_example.asm
    
    if (test_files.empty()) {
        fmt::print("\nNo in-assembly test files found in tests/.\n");
        return;
    }
    
    // Sort files for consistent ordering
    std::sort(test_files.begin(), test_files.end());
    
    // Create test executor
    Testing::TestExecutor executor;
    std::vector<Testing::TestResult> all_results;
    
    for (const auto& file : test_files) {
        // Check if file exists
        std::ifstream check(file);
        if (!check.good()) {
            continue; // Skip if file doesn't exist
        }
        
        // Execute tests from this file
        auto results = executor.execute_tests_from_file(file);
        all_results.insert(all_results.end(), results.begin(), results.end());
    }
    
    // Print consolidated results
    if (all_results.empty()) {
        fmt::print("\nNo in-assembly tests found in tests/.\n");
    } else {
        executor.print_results(all_results);
    }
}

void run_unit_tests_only() {
    const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
    std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
    std::cout << color << "│     Running DemiEngine Unit Tests                    │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;
    run_unit_tests();
    exit(0);
}

void run_assembly_tests_only() {
    Config::verbose = true;  // Enable INFO logs for test output
    try {
        run_in_assembly_tests();
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception in run_in_assembly_tests(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error: Unknown exception in run_in_assembly_tests()" << std::endl;
    }
    std::flush(std::cerr);
    std::flush(std::cout);
    _exit(0); // Use _exit to completely bypass all destructors
}

void run_single_file_tests(const std::string& filepath) {
    Config::verbose = true;  // Enable INFO logs for test output
    
    // Check if file exists
    std::ifstream check(filepath);
    if (!check.good()) {
        Logger::instance().error() << fmt::format("Test file not found: {}", filepath) << std::endl;
        exit(1);
    }
    check.close();
    
    // Create test executor and run tests from the specified file
    Testing::TestExecutor executor;
    auto results = executor.execute_tests_from_file(filepath);
    
    if (results.empty()) {
        fmt::print("\nNo tests found in file: {}\n", filepath);
    } else {
        executor.print_results(results);
    }
    
    exit(0);
}

bool run_tests() {
    // Run all test suites
    const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
    std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
    std::cout << color << "│     Running DemiEngine Unit Tests                    │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;

    run_unit_tests();
    
    // Run in-assembly tests
    run_in_assembly_tests();

    // Run integration tests
    std::cout << std::endl;
    std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
    std::cout << color << "│     Running DemiEngine Integration Tests             │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;

    TestRunner runner("tests/hex");
    auto results = runner.run_all();
    int passed = 0, failed = 0;
    
    const char* result_color = Config::debug ? "\033[38;5;208m" : "\033[36m";
    std::cout << result_color << "┌──────────────────────────────────────────────────────┤\033[0m" << std::endl;
    std::cout << result_color << "│     DemiEngine Integration Test Results              │\033[0m" << std::endl;
    std::cout << result_color << "└──────────────────────────────────────────────────────┘\033[0m" << std::endl;
    
    for (const auto& result : results) {
        [[maybe_unused]] constexpr int name_width = 24;
        const char* color = result.passed ? "\033[32m" : "\033[31m";
        const char* reset = "\033[0m";
        std::cout << fmt::format("{0}[{1}]{2} {3:<28}", color, result.passed ? "/" : "X", reset, result.name);
        if ((&result - &results[0] + 1) % 4 == 0)
            std::cout << std::endl;
        else
            std::cout << "    ";
        if (result.passed) ++passed; else ++failed;
    }
    std::cout << std::endl;
    
    const char* summary_color = (failed == 0) ? "\033[32m" : "\033[33m";
    std::cout << summary_color << "Integration tests passed: " << passed << " / " << results.size() << "\033[0m" << std::endl;
    exit(0);
}

class DemiEngine {
public:
    DemiEngine(int argc, char *argv[]) {
        // Help argument
        parser.add_action_arg("help", "--help", "-h", "Shows help information",
            [this]() { parser.print_help(); show_help = true; });
        // Debug argument
        parser.add_bool_arg("debug", "--debug", "-d", "Enable debug mode",
            [this](bool value) { Config::debug = value; Config::verbose = value; });
        // Verbose argument
        parser.add_bool_arg("verbose", "--verbose", "-v", "Show informational messages (use --verbose=false to disable)",
            [this](bool value) { Config::verbose = value; });

        // Extended registers argument
        parser.add_bool_arg("extended_registers", "--extended-registers", "-er", "Show extended register output (50 registers)",
            [this](bool value) { Config::extended_registers = value; });

        // Debug File argument
        parser.add_value_arg("debug_file", "--debug-file", "-f", "Debug file path",
            [this](const std::string& value) { Config::debug_file = value; });

        // Hex file argument
        parser.add_value_arg("hex", "--hex", "-H", "Path to hex file (hex bytes, space or newline separated)",
            [this](const std::string& value) { Config::program_file = value; });

        // Run tests argument (with optional file path)
        parser.add_value_arg("test", "--test", "-t", "Run built-in unit tests, or test a specific file if path provided",
            [this](const std::string& value) {
                if (value.empty() || value == "true") {
                    // No file specified, run unit tests only
                    run_unit_tests_only();
                } else {
                    // File specified, run tests from that file only
                    run_single_file_tests(value);
                }
            });
        
        // Run unit tests only (or specific file)
        parser.add_value_arg("unit_test", "--unit-test", "-ut", "Run built-in unit tests only, or test a specific file if path provided",
            [this](const std::string& value) {
                if (value.empty()) {
                    run_unit_tests_only();
                } else {
                    run_single_file_tests(value);
                }
            });
        
        // Run assembly tests only (or specific file)
        parser.add_action_arg("assembly_test", "--assembly-test", "-at", "Run in-assembly tests only",
            [this]() {
                auto positional = parser.get_positional_args();
                if (positional.empty()) {
                    run_assembly_tests_only();
                } else {
                    // Run tests on specified files
                    for (const auto& file : positional) {
                        run_single_file_tests(file);
                    }
                }
            });
        
        // Run assembly tests in quiet mode (only show title and description)
        parser.add_action_arg("assembly_test_quiet", "--assembly-test-quiet", "-atq", "Run in-assembly tests in quiet mode (title and description only)",
            [this]() {
                Config::quiet_assembly_test = true;
                auto positional = parser.get_positional_args();
                if (positional.empty()) {
                    run_assembly_tests_only();
                } else {
                    // Run tests on specified files
                    for (const auto& file : positional) {
                        run_single_file_tests(file);
                    }
                }
            });

        // Assembly mode argument
        parser.add_value_arg("assembly", "--assembly", "-A", "Assembly mode: assemble and run .asm file",
            [this](const std::string& value) {
                Config::assembly_mode = true;
                Config::assembly_file = value;
            });

        // Compile argument
        parser.add_value_arg("compile", "--compile", "-o", "Compile program into a standalone executable (optionally specify output name)",
            [this](const std::string& value) {
                Config::compile_only = true;
                Config::output_name = value;
            });

        // Memory dump argument
        parser.add_bool_arg("memdump", "--memdump", "-m", "Print memory dump after execution", [this](bool value) { Config::memdump = value; });

        // Show filter argument for tests
        parser.add_value_arg("show", "--show", "", "Filter test output (all|fails|success)", 
            [this](const std::string& value) {
                if (value == "fails" || value == "fail") {
                    Config::test_show_mode = TestShowMode::FAILS;
                } else if (value == "success" || value == "pass") {
                    Config::test_show_mode = TestShowMode::SUCCESS;
                } else if (value == "all" || value.empty()) {
                    Config::test_show_mode = TestShowMode::ALL;
                } else {
                    std::cerr << "Error: Invalid --show value '" << value << "'. Use 'all', 'fails', or 'success'." << std::endl;
                    exit(1);
                }
            });

        parser.parse(argc, argv);
    }

    // Run in compiled mode (create a standalone executable)
    void run_compiled(std::vector<uint8_t>& program) {
        // Create a standalone executable instead of running the program
        std::string output_name;

        if (Config::output_name.empty()) {
            // Generate a name if none provided
            output_name = generate_executable_name(Config::program_file);
        } else {
            // Use provided name and sanitize it
            output_name = sanitize_filename(Config::output_name);
            if (output_name.empty()) {
                std::cerr << "Error: Invalid output filename: " << Config::output_name << std::endl;
                std::cerr << "Filename cannot contain: . at start, ../, or shell metacharacters ;|&`$()[]{}*?<>" << std::endl;
                return;
            }

            // Create directory if it doesn't exist
            fs::path output_path(output_name);
            if (output_path.has_parent_path()) {
                fs::path dir = output_path.parent_path();
                if (!fs::exists(dir)) {
                    if (!fs::create_directories(dir)) {
                        std::cerr << "Error: Failed to create directory: " << dir << std::endl;
                        return;
                    }
                }
            }
        }
        if (create_standalone_executable(program, output_name)) {
            std::string shown_name = output_name;
            if (shown_name.substr(0, 2) != "./") {
                shown_name = "./" + shown_name;
            }
            std::cout << "Successfully compiled to executable: " << shown_name << std::endl;

            // Don't add ./ prefix if output_name already starts with ./
            std::string run_command = output_name;
            if (run_command.substr(0, 2) != "./") {
                run_command = "./" + run_command;
            }
            std::cout << "You can run it with: " << run_command << std::endl;
        }
    }

    // Generate a suitable executable name from the program file path
    std::string generate_executable_name(const std::string& program_file) {
        fs::path path(program_file);
        std::string name = path.stem().string();  // Get filename without extension

        // Make sure we have the bin directory
        fs::path bin_dir("bin");
        if (!fs::exists(bin_dir)) {
            fs::create_directory(bin_dir);
        }

        return (bin_dir / name).string();
    }

    // Sanitize filename to prevent command injection
    std::string sanitize_filename(const std::string& filename) {
        // First check for completely invalid patterns
        if (filename.empty() ||
            filename.find("../") != std::string::npos ||
            filename.find_first_of(";|&`$()[]{}*?<>") != std::string::npos) {
            return "";
        }

        // Check for hidden files (starting with . but not ./ which is current directory)
        if (filename[0] == '.' && filename.length() > 1 && filename[1] != '/') {
            return "";
        }

        std::string sanitized;
        sanitized.reserve(filename.length());

        for (char c : filename) {
            // Allow alphanumeric, dots, hyphens, underscores, and forward slashes for paths
            if (std::isalnum(c) || c == '.' || c == '-' || c == '_' || c == '/') {
                sanitized += c;
            }
            // Replace other characters with underscore
            else {
                sanitized += '_';
            }
        }

        return sanitized;
    }

    // Create a standalone executable with the program embedded
    bool create_standalone_executable(const std::vector<uint8_t>& program,
                                      const std::string& output_name) {
        // Note: output_name is already sanitized by run_compiled()

        // 1. Generate program_data.hpp with the program bytes
        if (!generate_program_data_header(program)) {
            std::cerr << "Failed to generate program data header" << std::endl;
            return false;
        } else {
            std::cout << "Generated program data header with " << program.size() << " bytes" << std::endl;
        }

        // 2. Compile the standalone main
        if (!compile_standalone_main(output_name)) {
            std::cerr << "Failed to compile standalone executable" << std::endl;
            return false;
        } else {
            std::cout << "Compiled standalone main to: " << output_name << std::endl;
        }

        return true;
    }

    // Generate program_data.hpp with the program bytes
    bool generate_program_data_header(const std::vector<uint8_t>& program) {
        std::ofstream outfile("src/program_data.hpp");
        if (!outfile) {
            std::cerr << "Error: Cannot create program_data.hpp" << std::endl;
            return false;
        }

        // Get current time for timestamp
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

        // Write the header file
        outfile << "// Auto-generated from program file\n";
        outfile << "// Generated on: " << timestamp.str() << "\n\n";
        outfile << "#ifndef PROGRAM_DATA_HPP\n";
        outfile << "#define PROGRAM_DATA_HPP\n\n";
        outfile << "#include <vector>\n";
        outfile << "#include <cstdint>\n\n";
        outfile << "// Program binary data\n";
        outfile << "const std::vector<uint8_t> PROGRAM_DATA = {\n    ";

        // Write the program bytes in a nicely formatted array
        const int ITEMS_PER_LINE = 12;
        for (size_t i = 0; i < program.size(); ++i) {
            outfile << "0x" << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(program[i]);

            if (i < program.size() - 1) {
                outfile << ", ";
                if ((i + 1) % ITEMS_PER_LINE == 0) {
                    outfile << "\n    ";
                }
            }
        }

        outfile << "\n};\n\n";
        outfile << "#endif // PROGRAM_DATA_HPP\n";

        return true;
    }

    // Securely compile standalone executable using execvp (no system())
    bool compile_standalone_main(const std::string& output_name) {
        // Create a simplified standalone main that doesn't include imgui
        std::string temp_file = "build/tmp_standalone.cpp";

        // Write a simplified version of standalone_main.cpp to a temporary file
        std::ofstream outfile(temp_file);
        if (!outfile) {
            std::cerr << "Error: Cannot create temporary file" << std::endl;
            return false;
        }

        outfile << "#include <iostream>\n";
        outfile << "#include <vector>\n";
        outfile << "#include <cstdint>\n";
        outfile << "#include \"config.hpp\"\n";
        outfile << "#include \"vhardware/cpu.hpp\"\n";
        outfile << "#include \"vhardware/device_factory.hpp\"\n\n";
        outfile << "// Include the generated program data\n";
        outfile << "#include \"program_data.hpp\"\n\n";
        outfile << "// Basic device initialization without logging\n";
        outfile << "void silent_initialize_devices() {\n";
        outfile << "    using namespace vhw;\n";
        outfile << "    auto console = DeviceFactory::createConsoleDevice(0x01);\n";
        outfile << "    auto counter = DeviceFactory::createCounterDevice(0x02);\n";
        outfile << "    auto file = DeviceFactory::createFileDevice(\"virtual_storage/vhd.dat\", 0x04);\n";
        outfile << "    auto ramdisk = DeviceFactory::createRamDiskDevice(8192, 0x05, 0x06);\n";
        outfile << "}\n\n";
        outfile << "int main(int, char**) {\n";
        outfile << "    // No debug mode by default\n";
        outfile << "    Config::debug = false;\n";
        outfile << "    Config::verbose = false;\n\n";
        outfile << "    // Initialize CPU\n";
        outfile << "    CPU cpu;\n";
        outfile << "    cpu.reset();\n\n";
        outfile << "    // Initialize devices silently\n";
        outfile << "    silent_initialize_devices();\n\n";
        outfile << "    // Execute the program\n";
        outfile << "    cpu.execute(PROGRAM_DATA);\n\n";
        outfile << "    // Handle errors if any\n";
        outfile << "    if (Config::error_count > 0) {\n";
        outfile << "        std::cerr << \"Execution failed with \" << Config::error_count << \" errors.\" << std::endl;\n";
        outfile << "        return 1;\n";
        outfile << "    }\n";
        outfile << "    return 0;\n";
        outfile << "}\n";
        outfile.close();

        // Create bin directory if it doesn't exist
        fs::path bin_dir("bin");
        if (!fs::exists(bin_dir)) {
            fs::create_directory(bin_dir);
        }        // Propietary files
        // Note: fmt library is included in extern/fmt
        // Note: imgui not included
        std::vector<std::string> extra_files = {
            "src/debug/logger.cpp",
            "src/vhardware/cpu.cpp",
            "src/vhardware/device_manager.cpp",
            // Use consolidated opcodes file for faster compilation
            "src/vhardware/opcodes/opcodes_consolidated.cpp",
            // "src/vhardware/device_factory.cpp",
            // "src/vhardware/devices/console_device.cpp",
            // "src/vhardware/devices/counter_device.cpp",
            // "src/vhardware/devices/file_device.cpp",
            // "src/vhardware/devices/ramdisk_device.cpp",
            "extern/fmt/src/format.cc"
        };

        // Prepare argument vector
        std::vector<std::string> args = {
            "g++", "-std=c++17", "-I./src",
            "-Iextern/fmt/include", "-Iextern",
            "-std=c++17",
            "-o", output_name,
            temp_file
        };
        // Add all extra source files
        args.insert(args.end(), extra_files.begin(), extra_files.end());

        // Convert args to char* array
        std::vector<char*> c_args;
        for (auto& arg : args) c_args.push_back(const_cast<char*>(arg.c_str()));
        c_args.push_back(nullptr);

        std::cout << "Building standalone executable..." << std::endl;

        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Failed to fork for compile" << std::endl;
            return false;
        } else if (pid == 0) {
            // child process
            execvp("g++", c_args.data());
            // If execvp fails
            std::cerr << "Failed to execute g++" << std::endl;
            _exit(127);
        }
        // parent process
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "Failed during waitpid" << std::endl;
            return false;
        } else {
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return false;
        }

        // Make the executable file executable
        fs::permissions(output_name,
            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
            fs::perm_options::add);

        return true;
    }

    void run() {
        if (show_help) return;

        // Handle test mode first
        if (Config::running_tests) {
            // Validate conflicting flags for test mode
            if (Config::assembly_mode) {
                std::cerr << "Error: Test mode (-t/--test) cannot be used with assembly mode (-A/--assembly)" << std::endl;
                return;
            }
            if (!Config::program_file.empty()) {
                std::cerr << "Error: Test mode (-t/--test) cannot be used with hex file (-H/--hex)" << std::endl;
                return;
            }
            run_tests();
            return;
        }

        // Validate conflicting flags for assembly mode
        if (Config::assembly_mode && !Config::program_file.empty()) {
            std::cerr << "Error: Assembly mode (-A/--assembly) cannot be used with hex file (-H/--hex)" << std::endl;
            return;
        }

        // Handle assembly mode
        if (Config::assembly_mode) {
            run_assembly_mode();
            return;
        }

        CPU cpu;
        cpu.reset();

        std::vector<uint8_t> program;
        if (!Config::program_file.empty()) {
            if (!load_program_file(Config::program_file, program)) {
                std::cerr << "Failed to load program file: " << Config::program_file << std::endl;
                return;
            }
        } else {
            std::cerr << "No hex file specified. Use --hex or -H to specify a hex file." << std::endl;
            return;
        }

        // Check if we should compile instead of run
        if (Config::compile_only) {
            run_compiled(program);
            return;
        }

        // Print a simple, clean headerw
        if (!Config::compile_only) {
            const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
            std::cout << color << "\n=== Demi Engine ===" << "\033[0m" << std::endl;
            std::cout << color << "Execution started..." << "\033[0m\n" << std::endl;
        }

        // Initialize the device system
        initialize_devices();

    cpu.execute(program, 0);

        // Print CPU state
        cpu.print_state("End");
        cpu.print_registers();

        // Print extended registers if enabled
        if (Config::extended_registers) {
            cpu.print_extended_registers();
        }

        cpu.print_memory();

        if (Config::error_count > 0) {
            Logger::instance().error() << "Execution failed with " << Config::error_count << " errors." << std::endl;
        } else {
            Logger::instance().success() << "Execution completed successfully." << std::endl;
        }
    }

private:
    ArgParser parser;
    std::string data;
    bool show_help = false;

    // Helper to load hex bytes from file
    bool load_program_file(const std::string& path, std::vector<uint8_t>& out) {
        std::ifstream file(path);
        if (!file) return false;
        std::string token;
        while (file >> token) {
            if (token[0] == '#') { file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                out.push_back(byte);
            } catch (...) {
                std::cerr << "Invalid hex byte in program file: " << token << std::endl;
                Config::error_count++;
                return false;
            }
        }
        return true;
    }

    // Assembly mode: assemble and run .asm file
    void run_assembly_mode() {
        if (Config::assembly_file.empty()) {
            std::cerr << "Error: No assembly file specified for assembly mode (-A/--assembly)" << std::endl;
            return;
        }

        // Check if file exists and has .asm extension
        if (!fs::exists(Config::assembly_file)) {
            std::cerr << "Error: Assembly file not found: " << Config::assembly_file << std::endl;
            return;
        }

        // Load assembly source
        std::ifstream file(Config::assembly_file);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open assembly file: " << Config::assembly_file << std::endl;
            return;
        }

        std::ostringstream oss;
        oss << file.rdbuf();
        std::string assembly_source = oss.str();

        if (Config::verbose) {
            std::cout << "Assembling: " << Config::assembly_file << std::endl;
        }

        // Step 1: Lexical analysis
        Assembler::Lexer lexer(assembly_source);
        auto tokens = lexer.tokenize();

        if (lexer.has_errors()) {
            std::cerr << "Lexer errors:" << std::endl;
            for (const auto& error : lexer.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        // Step 2: Parsing
        Assembler::Parser parser(tokens);
        auto ast = parser.parse();

        if (parser.has_errors()) {
            std::cerr << "Parser errors:" << std::endl;
            for (const auto& error : parser.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        // Step 3: Code generation
        Assembler::AssemblerEngine assembler;
        auto bytecode = assembler.assemble(*ast);

        if (assembler.has_errors()) {
            std::cerr << "Assembly errors:" << std::endl;
            for (const auto& error : assembler.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        if (Config::verbose) {
            std::cout << "Assembly successful. Generated " << bytecode.size() << " bytes of bytecode." << std::endl;

            // Show symbol table if verbose
            const auto& symbols = assembler.get_symbols();
            if (!symbols.empty()) {
                std::cout << "Symbol table:" << std::endl;
                for (const auto& [name, symbol] : symbols) {
                    std::cout << "  " << name << " = 0x" << std::hex << symbol.address << std::dec << std::endl;
                }
            }
        }

        // Initialize CPU and devices
        CPU cpu;
        cpu.reset();
        initialize_devices();

        // Set PC to entry address if available
        uint32_t entry_addr = assembler.get_entry_address();

        // Print header for assembled program
        if (Config::verbose) {
            std::cout << "\n\033[36m┌─────────────────────────────────────────────────────────────┐\033[0m" << std::endl;
            std::cout << "\033[36m│\033[0m               \033[1mRunning Assembled Program\033[0m                     \033[36m│\033[0m" << std::endl;
            std::cout << "\033[36m└─────────────────────────────────────────────────────────────┘\033[0m" << std::endl;
        }

        // Print hex dump of assembled bytecode
        Logger::instance().debug() << "Assembled bytecode hex dump:" << std::endl;
        std::string line;
        for (size_t i = 0; i < bytecode.size(); ++i) {
            if (i % 16 == 0) {
                if (!line.empty()) Logger::instance().debug() << line << std::endl;
                std::ostringstream addr_stream;
                addr_stream << "0x" << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
                line = addr_stream.str();
            }
            std::ostringstream byte_stream;
            byte_stream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytecode[i]) << " ";
            line += byte_stream.str();
        }
        if (!line.empty()) Logger::instance().debug() << line << std::endl;
        std::cout << std::dec << std::endl;

        // Print entry address and bytes at entry address
        Logger::instance().debug() << "Entry address: 0x" << std::hex << entry_addr << std::dec << std::endl;
        std::ostringstream bytes_stream;
        bytes_stream << "Bytes at entry address:";
        for (size_t i = entry_addr; i < entry_addr + 8 && i < bytecode.size(); ++i) {
            bytes_stream << " " << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytecode[i]);
        }
        Logger::instance().debug() << bytes_stream.str() << std::endl;

        try {
            // Debug: Print entry address before execution
            Logger::instance().debug() << "Entry address: 0x" << std::hex << entry_addr << std::dec << std::endl;
            // Execute the assembled bytecode, starting at entry address
            cpu.execute(bytecode, entry_addr);

            // Print CPU state and registers (same as regular program mode)
            Logger::instance().debug() << "Post-execution: printing CPU state..." << std::endl;
            cpu.print_state("End");
            Logger::instance().debug() << "Post-execution: printing registers..." << std::endl;
            cpu.print_registers();

            // Print extended registers if enabled
            if (Config::extended_registers) {
                Logger::instance().debug() << "Post-execution: printing extended registers..." << std::endl;
                cpu.print_extended_registers();
            }

            if (Config::memdump) {
                Logger::instance().debug() << "Post-execution: printing memory..." << std::endl;
                size_t mem_size = cpu.get_memory().size();
                if (mem_size > 0) {
                    cpu.print_memory(0, std::min<size_t>(mem_size, 256));
                } else {
                    Logger::instance().debug() << "Memory size is zero, skipping print_memory." << std::endl;
                }
            }

            if (Config::error_count > 0) {
                Logger::instance().error() << "Assembly program failed with " << Config::error_count << " errors." << std::endl;
            } else {
                Logger::instance().success() << "Assembly program completed successfully." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
            Config::error_count++;
        }
    }
};

int main(int argc, char *argv[]) {
    DemiEngine app(argc, argv);
    app.run();
    return 0;
}