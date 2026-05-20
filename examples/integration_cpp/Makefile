# Makefile for Demi C++ Integration API Example

# Build profile: debug or release
BUILD_TYPE ?= debug

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -I../../src -I../../extern/fmt/include

ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += -DDEMI_DEBUG_BUILD -D_GLIBCXX_DEBUG -DDEMI_DEBUG_MODE
    LIB_SUFFIX := -debug
else
    LIB_SUFFIX := -release
endif

LIB_STATIC := ../../bin/libdemi$(LIB_SUFFIX).a
TARGET := integration_demo

.PHONY: all clean run assemble

all: $(TARGET)

$(TARGET): main.cpp $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CXX) $(CXXFLAGS) $< $(LIB_STATIC) -pthread -lstdc++fs -o $@
	@echo "✅ Built $(TARGET) successfully"

# Rule to assemble hello.asm into hello.hex using root demi-engine
assemble: hello.hex

hello.hex: hello.asm
	@echo "🔨 Assembling hello.asm to raw bytecode..."
	../../bin/demi-engine$(LIB_SUFFIX) -A $< -ao $@

# Compile the root project libraries if they don't exist yet
$(LIB_STATIC):
	@echo "📦 building Demi static library in root folder..."
	$(MAKE) -C ../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

run: all assemble
	@echo "🚀 Running integration_demo..."
	./$(TARGET) hello.hex

clean:
	rm -f $(TARGET) hello.hex
