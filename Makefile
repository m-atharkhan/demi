# =============================================================================
# DEMI Engine - Production Makefile
# =============================================================================
# Fast, efficient, production-ready build system
# Features: parallel builds, dependency tracking, incremental compilation,
# multiple build configurations, comprehensive testing
# =============================================================================

# Compiler configuration
CXX := g++
SHELL := /bin/bash

# CPU core detection for parallel builds
NPROCS := $(shell nproc 2>/dev/null || echo 1)
MAKEFLAGS += -j$(NPROCS)

# Directory structure
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
EXTERN_DIR := extern
DOC_DIR := docs
TEST_DIR := tests

# Build configuration
BUILD_TYPE ?= debug
CONFIG_FILE := .build_config

# =============================================================================
# Build Configuration Profiles
# =============================================================================

# Debug build (default)
ifeq ($(BUILD_TYPE),debug)
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -g3 -O0 -MMD -MP
CXXFLAGS += -DDEMI_DEBUG_BUILD -D_GLIBCXX_DEBUG -DDEMI_DEBUG_MODE
LDFLAGS := -g
BUILD_SUFFIX := -debug
endif

# Release build
ifeq ($(BUILD_TYPE),release)
CXXFLAGS := -Wall -Wextra -std=c++17 -O3 -DNDEBUG -MMD -MP -flto
CXXFLAGS += -march=native -mtune=native -fomit-frame-pointer
LDFLAGS := -O3 -flto -s
BUILD_SUFFIX := -release
endif

# Profile build (optimized with debug symbols)
ifeq ($(BUILD_TYPE),profile)
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -g -MMD -MP
CXXFLAGS += -DNDEBUG -fno-omit-frame-pointer -DDEMI_PROFILE_MODE
LDFLAGS := -O2 -g
BUILD_SUFFIX := -profile
endif

# Sanitizer build (for testing)
ifeq ($(BUILD_TYPE),sanitize)
CXXFLAGS := -Wall -Wextra -std=c++17 -O1 -g -MMD -MP
CXXFLAGS += -fsanitize=address,undefined,leak -fno-omit-frame-pointer
LDFLAGS := -fsanitize=address,undefined,leak
BUILD_SUFFIX := -sanitize
endif

# Common linker flags
LDFLAGS += -lstdc++fs -pthread

# =============================================================================
# External Dependencies
# =============================================================================

# FMT library
FMT_DIR := $(EXTERN_DIR)/fmt
FMT_SRCS := $(FMT_DIR)/src/format.cc
FMT_OBJS := $(patsubst $(FMT_DIR)/src/%.cc,$(BUILD_DIR)/fmt/%.o,$(FMT_SRCS))
FMT_DEPS := $(FMT_OBJS:.o=.d)
CXXFLAGS += -I$(FMT_DIR)/include

# ImGui library  
IMGUI_DIR := $(EXTERN_DIR)/imgui
IMGUI_BACKEND_DIR := $(IMGUI_DIR)/backends
IMGUI_SRCS := $(wildcard $(IMGUI_DIR)/*.cpp) \
              $(IMGUI_BACKEND_DIR)/imgui_impl_glfw.cpp \
              $(IMGUI_BACKEND_DIR)/imgui_impl_opengl3.cpp
IMGUI_OBJS := $(patsubst $(EXTERN_DIR)/%.cpp,$(BUILD_DIR)/extern/%.o,$(IMGUI_SRCS))
IMGUI_DEPS := $(IMGUI_OBJS:.o=.d)
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_BACKEND_DIR)

# System libraries for ImGui
LDFLAGS += -lglfw -lGL -lGLEW

# =============================================================================
# Source File Discovery and Object Generation  
# =============================================================================

# Find all source files (cached for performance)
MAIN_SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'test_assembler.cpp')
TEST_SRCS := $(shell find $(SRC_DIR)/test -name 'test_assembler.cpp' -type f 2>/dev/null)
ALL_SRCS := $(MAIN_SRCS) $(TEST_SRCS)

# Generate object file paths
MAIN_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_SRCS))
TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))
ALL_OBJS := $(MAIN_OBJS) $(TEST_OBJS)

# Dependency files
ALL_DEPS := $(ALL_OBJS:.o=.d) $(FMT_DEPS) $(IMGUI_DEPS)

# =============================================================================
# Target Definitions
# =============================================================================

# Main executable
TARGET := $(BIN_DIR)/demi-engine$(BUILD_SUFFIX)
MAIN_TARGET_OBJS := $(filter-out $(BUILD_DIR)/test/test_assembler.o,$(MAIN_OBJS)) $(BUILD_DIR)/main.o

# Test targets
TEST_TARGET := $(BIN_DIR)/test_runner$(BUILD_SUFFIX)
ASSEMBLER_TEST_TARGET := $(BIN_DIR)/test_assembler$(BUILD_SUFFIX)

# Library objects (everything except main.o and test_assembler.o)
LIB_OBJS := $(filter-out $(BUILD_DIR)/main.o $(BUILD_DIR)/test/test_assembler.o,$(MAIN_OBJS))

# =============================================================================
# Default Target and Build Goals
# =============================================================================

.DEFAULT_GOAL := all
.PHONY: all clean build install test unit-test integration-test benchmark
.PHONY: debug release profile sanitize prereqs help version info
.PHONY: test-all test-assembler force-rebuild check-deps
.PHONY: benchmark-binaries docs lint format

# =============================================================================
# Main Build Targets
# =============================================================================

# Default build (all configurations)
all: $(TARGET)

# Quick build shorthand  
build: all

# Main executable
$(TARGET): $(MAIN_TARGET_OBJS) $(FMT_OBJS) $(IMGUI_OBJS) | $(BIN_DIR)
	@echo "🔗 Linking $(notdir $@)..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Build complete: $@"

# Test runner
$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS) $(FMT_OBJS) $(IMGUI_OBJS) | $(BIN_DIR)
	@echo "🔗 Linking test runner..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Test runner built: $@"

# Assembler test (lightweight)
$(ASSEMBLER_TEST_TARGET): $(filter $(BUILD_DIR)/assembler/%,$(LIB_OBJS)) \
                          $(filter $(BUILD_DIR)/test/test_assembler%,$(TEST_OBJS)) \
                          $(FMT_OBJS) | $(BIN_DIR)
	@echo "🔗 Linking assembler test..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Assembler test built: $@"

# =============================================================================
# Compilation Rules
# =============================================================================

# Source file compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling $(notdir $<)..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# FMT library compilation
$(BUILD_DIR)/fmt/%.o: $(FMT_DIR)/src/%.cc
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling FMT $(notdir $<)..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ImGui compilation
$(BUILD_DIR)/extern/imgui/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling ImGui $(notdir $<)..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/extern/imgui/backends/%.o: $(IMGUI_BACKEND_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling ImGui backend $(notdir $<)..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# =============================================================================
# Directory Creation
# =============================================================================

$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# =============================================================================
# Build Configuration Management
# =============================================================================

# Configuration targets
debug:
	@$(MAKE) BUILD_TYPE=debug all
	@echo "BUILD_TYPE=debug" > $(CONFIG_FILE)

release:
	@$(MAKE) BUILD_TYPE=release all  
	@echo "BUILD_TYPE=release" > $(CONFIG_FILE)

profile:
	@$(MAKE) BUILD_TYPE=profile all
	@echo "BUILD_TYPE=profile" > $(CONFIG_FILE)

sanitize:
	@$(MAKE) BUILD_TYPE=sanitize all
	@echo "BUILD_TYPE=sanitize" > $(CONFIG_FILE)

# =============================================================================
# Testing Targets
# =============================================================================

# Run all tests
test-all: unit-test integration-test

# Unit tests
unit-test: $(TARGET)
	@echo "🧪 Running unit tests..."
	@./$(TARGET) -t

# Integration tests with debug output
integration-test: $(TARGET)
	@echo "🔬 Running integration tests..."
	@./$(TARGET) -t -d

# Assembler-only tests
test-assembler: $(ASSEMBLER_TEST_TARGET)
	@echo "🧪 Running assembler tests..."
	@./$(ASSEMBLER_TEST_TARGET)

# Simple test alias
test: unit-test

# =============================================================================
# Development and Maintenance Targets  
# =============================================================================

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -f $(CONFIG_FILE)
	@echo "✅ Clean complete"

# Force rebuild of specific components
force-rebuild:
	@echo "🔄 Force rebuilding opcodes..."
	@rm -f $(BUILD_DIR)/engine/opcodes/*.o $(BUILD_DIR)/engine/opcodes/*.d
	@$(MAKE) all

# Install system prerequisites
prereqs:
	@echo "📦 Checking system prerequisites..."
	@if ! dpkg -s libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev >/dev/null 2>&1; then \
		echo "🔧 Installing required system libraries..."; \
		sudo apt-get -qq update && sudo apt-get -qq install -y \
			libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev \
			build-essential pkg-config; \
	else \
		echo "✅ All required system libraries are installed"; \
	fi
	@if [ ! -d $(FMT_DIR) ]; then \
		echo "📥 Cloning FMT library..."; \
		git clone --depth=1 --branch=10.1.1 https://github.com/fmtlib/fmt.git $(FMT_DIR); \
	else \
		echo "✅ FMT library present"; \
	fi
	@if [ ! -d $(IMGUI_DIR) ]; then \
		echo "📥 Cloning ImGui library..."; \
		git clone --depth=1 --branch=v1.89.9 https://github.com/ocornut/imgui.git $(IMGUI_DIR); \
	else \
		echo "✅ ImGui library present"; \
	fi

# Dependency check
check-deps:
	@echo "🔍 Checking dependencies..."
	@echo "Source files: $(words $(ALL_SRCS))"
	@echo "Object files: $(words $(ALL_OBJS))"
	@echo "FMT objects: $(words $(FMT_OBJS))"
	@echo "ImGui objects: $(words $(IMGUI_OBJS))"
	@echo "Build type: $(BUILD_TYPE)"

# =============================================================================
# Performance and Benchmarking
# =============================================================================

# Benchmark build configurations
BENCHMARK_BUILD_DIR := build-benchmark
BENCHMARK_THREADED_TARGET := $(BIN_DIR)/demi-engine-threaded
BENCHMARK_FALLBACK_TARGET := $(BIN_DIR)/demi-engine-fallback

# Optimized benchmark objects
BENCHMARK_THREADED_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BENCHMARK_BUILD_DIR)/threaded/%.o,$(MAIN_SRCS))
BENCHMARK_FALLBACK_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BENCHMARK_BUILD_DIR)/fallback/%.o,$(MAIN_SRCS))

# Benchmark FMT objects
BENCHMARK_THREADED_FMT := $(patsubst $(FMT_DIR)/src/%.cc,$(BENCHMARK_BUILD_DIR)/threaded/fmt/%.o,$(FMT_SRCS))
BENCHMARK_FALLBACK_FMT := $(patsubst $(FMT_DIR)/src/%.cc,$(BENCHMARK_BUILD_DIR)/fallback/fmt/%.o,$(FMT_SRCS))

# Threaded benchmark build (maximum optimization)
$(BENCHMARK_BUILD_DIR)/threaded/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "⚡ Compiling threaded benchmark $(notdir $<)..."
	@$(CXX) -Wall -Wextra -std=c++17 -O3 -DNDEBUG -flto -march=native \
		-I$(FMT_DIR)/include -MMD -MP -c $< -o $@

$(BENCHMARK_BUILD_DIR)/threaded/fmt/%.o: $(FMT_DIR)/src/%.cc  
	@mkdir -p $(dir $@)
	@echo "⚡ Compiling threaded FMT $(notdir $<)..."
	@$(CXX) -Wall -Wextra -std=c++17 -O3 -DNDEBUG -flto -march=native \
		-I$(FMT_DIR)/include -MMD -MP -c $< -o $@

$(BENCHMARK_THREADED_TARGET): $(BENCHMARK_THREADED_OBJS) $(BENCHMARK_THREADED_FMT) | $(BIN_DIR)
	@echo "🚀 Linking threaded benchmark..."
	@$(CXX) -O3 -DNDEBUG -flto -march=native -o $@ $^ -lstdc++fs -pthread
	@echo "✅ Threaded benchmark built: $@"

# Fallback benchmark build (with sanitizer)
$(BENCHMARK_BUILD_DIR)/fallback/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🔍 Compiling fallback benchmark $(notdir $<)..."
	@$(CXX) -Wall -Wextra -std=c++17 -O2 -g -fsanitize=address \
		-I$(FMT_DIR)/include -MMD -MP -c $< -o $@

$(BENCHMARK_BUILD_DIR)/fallback/fmt/%.o: $(FMT_DIR)/src/%.cc
	@mkdir -p $(dir $@)  
	@echo "🔍 Compiling fallback FMT $(notdir $<)..."
	@$(CXX) -Wall -Wextra -std=c++17 -O2 -g -fsanitize=address \
		-I$(FMT_DIR)/include -MMD -MP -c $< -o $@

$(BENCHMARK_FALLBACK_TARGET): $(BENCHMARK_FALLBACK_OBJS) $(BENCHMARK_FALLBACK_FMT) | $(BIN_DIR)
	@echo "🔍 Linking fallback benchmark..."
	@$(CXX) -O2 -g -fsanitize=address -o $@ $^ -lstdc++fs -pthread -fsanitize=address
	@echo "✅ Fallback benchmark built: $@"

# Build both benchmark targets
benchmark-binaries: $(BENCHMARK_THREADED_TARGET) $(BENCHMARK_FALLBACK_TARGET)
	@echo "📊 Benchmark binaries ready:"
	@echo "  🚀 $(BENCHMARK_THREADED_TARGET) (optimized)"
	@echo "  🔍 $(BENCHMARK_FALLBACK_TARGET) (sanitized)"

# Run comprehensive benchmark suite
benchmark: benchmark-binaries
	@echo "🏁 DEMI Engine Performance Benchmark"
	@echo "═══════════════════════════════════════"
	@if [ ! -d benchmarks ]; then \
		echo "⚠️  No benchmarks/ directory found - creating basic test..."; \
		mkdir -p benchmarks; \
		echo "# Basic test case" > benchmarks/basic.asm; \
		echo "MOV R1, 42" >> benchmarks/basic.asm; \
		echo "ADD R1, 8" >> benchmarks/basic.asm; \
		echo "HALT" >> benchmarks/basic.asm; \
	fi
	@BENCHMARK_FILES=$$(find benchmarks -name "*.asm" | sort); \
	if [ -z "$$BENCHMARK_FILES" ]; then \
		echo "❌ No .asm files found in benchmarks/"; \
		exit 1; \
	fi; \
	echo "📁 Found benchmark files:"; \
	for file in $$BENCHMARK_FILES; do echo "  📄 $$file"; done; \
	echo; \
	for file in $$BENCHMARK_FILES; do \
		echo "🧪 Testing $$file"; \
		echo "────────────────────────────────────────"; \
		echo "🚀 Threaded Dispatcher:"; \
		for i in 1 2 3; do \
			printf "  Run $$i: "; \
			if timeout 30 ./$(BENCHMARK_THREADED_TARGET) -A "$$file" >/dev/null 2>&1; then \
				echo "✅ Success"; \
			else \
				echo "❌ Failed"; \
			fi; \
		done; \
		echo "🔍 Fallback Dispatcher:"; \
		for i in 1 2 3; do \
			printf "  Run $$i: "; \
			start=$$(date +%s.%N); \
			if timeout 30 ./$(BENCHMARK_FALLBACK_TARGET) -A "$$file" >/dev/null 2>&1; then \
				end=$$(date +%s.%N); \
				runtime=$$(echo "$$end - $$start" | bc -l 2>/dev/null || echo "0.0000"); \
				printf "✅ %.4fs\n" $$runtime; \
			else \
				echo "❌ Failed"; \
			fi; \
		done; \
		echo; \
	done; \
	echo "🏁 Benchmark complete!"

# =============================================================================
# Development Quality Tools
# =============================================================================

# Code formatting (if clang-format is available)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "🎨 Formatting source code..."; \
		find $(SRC_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i; \
		echo "✅ Code formatting complete"; \
	else \
		echo "⚠️  clang-format not available"; \
	fi

# Static analysis (if available)
lint:
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "🔍 Running static analysis..."; \
		cppcheck --enable=warning,style,performance,portability \
			--std=c++17 --quiet $(SRC_DIR)/; \
	else \
		echo "⚠️  cppcheck not available"; \
	fi

# Project information
info:
	@echo "📋 DEMI Engine Build Information"
	@echo "═══════════════════════════════════"
	@echo "🏗️  Build type: $(BUILD_TYPE)"
	@echo "🔧 Compiler: $(CXX)"
	@echo "⚙️  Flags: $(CXXFLAGS)"
	@echo "🔗 Linker flags: $(LDFLAGS)"  
	@echo "📁 Source files: $(words $(ALL_SRCS))"
	@echo "🎯 Main target: $(TARGET)"
	@echo "🧪 Test target: $(TEST_TARGET)"
	@echo "🚀 CPU cores: $(NPROCS)"
	@echo "📦 External libs: FMT, ImGui"

# Version information (if VERSION file exists)
version:
	@if [ -f VERSION ]; then \
		echo "📋 DEMI Engine v$$(cat VERSION)"; \
	else \
		echo "📋 DEMI Engine (development)"; \
	fi
	@if [ -f $(CONFIG_FILE) ]; then \
		echo "🏗️  Last build: $$(cat $(CONFIG_FILE))"; \
	fi

# Help target
help:
	@{ \
		echo "🛠️  DEMI Engine Makefile Commands"; \
		echo "════════════════════════════════════"; \
		echo ""; \
		echo "🏗️  Build Commands:"; \
		echo "  make, make all    - Build main target (current: $(BUILD_TYPE))"; \
		echo "  make debug        - Build debug version"; \
		echo "  make release      - Build optimized release"; \
		echo "  make profile      - Build with profiling symbols"; \
		echo "  make sanitize     - Build with sanitizers"; \
		echo ""; \
		echo "🧪 Test Commands:"; \
		echo "  make test         - Run unit tests"; \
		echo "  make test-all     - Run all tests"; \
		echo "  make unit-test    - Run unit tests only"; \
		echo "  make integration-test - Run integration tests"; \
		echo "  make test-assembler   - Run assembler tests"; \
		echo ""; \
		echo "📊 Performance:"; \
		echo "  make benchmark    - Run performance benchmarks"; \
		echo "  make benchmark-binaries - Build benchmark targets"; \
		echo ""; \
		echo "🛠️  Maintenance:"; \
		echo "  make clean        - Remove build artifacts"; \
		echo "  make prereqs      - Install system dependencies"; \
		echo "  make force-rebuild - Force rebuild opcodes"; \
		echo "  make format       - Format source code"; \
		echo "  make lint         - Static analysis"; \
		echo ""; \
		echo "ℹ️  Information:"; \
		echo "  make info         - Build configuration info"; \
		echo "  make version      - Version information"; \
		echo "  make help         - This help message"; \
	} 2>/dev/null || true

# =============================================================================
# Dependency Management
# =============================================================================

# Include dependency files (suppress errors if they don't exist)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),help)
ifneq ($(MAKECMDGOALS),prereqs)
-include $(ALL_DEPS)
endif
endif
endif

# =============================================================================
# Special Targets and Rules
# =============================================================================

# Install to system (optional)
install: $(TARGET)
	@echo "📦 Installing DEMI Engine..."
	@sudo install -D $(TARGET) /usr/local/bin/demi-engine
	@echo "✅ Installed to /usr/local/bin/demi-engine"

# Prevent make from deleting intermediate files
.PRECIOUS: $(BUILD_DIR)/%.o $(BUILD_DIR)/%.d

# Silent rules for cleaner output
ifndef VERBOSE
.SILENT:
endif