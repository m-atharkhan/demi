CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g -MMD -MP
LDFLAGS += -lstdc++fs
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Find all .cpp files in src and its subdirectories, excluding test files
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'test_runner.cpp' -not -name 'test_*.cpp')
# Add the new register system source files explicitly
REGISTER_SRCS := $(SRC_DIR)/engine/cpu_registers.cpp
SRCS += $(REGISTER_SRCS)
# Replace src/ with build/ and .cpp with .o for object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
# Generate dependency files
DEPS := $(OBJS:.o=.d)

TARGET := $(BIN_DIR)/demi-engine

# Assembler test target (minimal dependencies, no CPU execution)
ASSEMBLER_TEST_TARGET := $(BIN_DIR)/test_assembler
ASSEMBLER_TEST_SRCS := $(SRC_DIR)/test/test_assembler.cpp $(filter $(SRC_DIR)/assembler/%.cpp,$(SRCS))
ASSEMBLER_TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(ASSEMBLER_TEST_SRCS))) $(BUILD_DIR)/test/test_assembler.o

# Test framework
TEST_TARGET := $(BIN_DIR)/test_runner
TEST_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'test_runner.cpp' -not -name 'test_*.cpp'))
TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_DEPS := $(TEST_OBJS:.o=.d)
TEST_RUNNER_SRC := $(SRC_DIR)/test/test_runner.cpp
TEST_RUNNER_OBJ := $(BUILD_DIR)/test/test_runner.o
TEST_RUNNER_DEP := $(TEST_RUNNER_OBJ:.o=.d)

# Include dependency files if they exist
-include $(DEPS)
-include $(TEST_DEPS)
-include $(TEST_RUNNER_DEP)

all: $(TARGET)

# Test framework target
$(TEST_TARGET): $(TEST_OBJS) $(TEST_RUNNER_OBJ) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Fmt library
FMT_DIR := extern/fmt
FMT_SRCS = $(FMT_DIR)/src/format.cc
FMT_OBJS = $(patsubst $(FMT_DIR)/src/%.cc,$(BUILD_DIR)/fmt/%.o,$(FMT_SRCS))
FMT_DEPS = $(FMT_OBJS:.o=.d)

# Include FMT dependency files
-include $(FMT_DEPS)

# Add fmt include path to all compile rules
CXXFLAGS += -Iextern/fmt/include

# Pattern rule to build .o files in build/ mirroring src/ structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for fmt sources
$(BUILD_DIR)/fmt/%.o: $(FMT_DIR)/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build assembler test
$(ASSEMBLER_TEST_TARGET): $(ASSEMBLER_TEST_OBJS) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule for building test_assembler.o in test build directory
$(BUILD_DIR)/test/test_assembler.o: $(SRC_DIR)/test/test_assembler.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Force rebuild of specific files when headers change
.PHONY: force-rebuild-opcodes
force-rebuild-opcodes:
	rm -f $(BUILD_DIR)/engine/opcodes/*.o $(BUILD_DIR)/engine/opcodes/*.d
	$(MAKE)

test: $(TARGET)
	./$(TARGET) -t -d

# Run unit tests
unit-test: $(TARGET)
	./$(TARGET) -t

# Build and run unit tests
test-all: unit-test test

# Test assembler
test-assembler: $(ASSEMBLER_TEST_TARGET)
	./$(ASSEMBLER_TEST_TARGET)

prereqs:
	@if ! dpkg -s libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev >/dev/null 2>&1; then \
		echo "Installing required system libraries..."; \
		echo "This may take a while..."; \
		sudo apt-get -qq update; \
		sudo apt-get -qq install -y libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev; \
	else \
		echo "All required system libraries are already installed."; \
	fi
	@if [ ! -d extern/fmt ]; then \
		echo "Cloning fmt..."; \
		git clone https://github.com/fmtlib/fmt.git extern/fmt; \
	else \
		echo "fmt already present."; \
	fi

build: prereqs $(TARGET)
	@echo "Build complete. Run './$(TARGET)' to start the application."
	@echo "Run 'make clean' to remove build artifacts."

.PHONY: clean build prereqs test unit-test test-all test-assembler