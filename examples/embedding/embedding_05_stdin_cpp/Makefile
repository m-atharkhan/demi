# Makefile for Demi embedding example 05 (stdin hook, C++ API)

BUILD_TYPE ?= debug

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -I../../../include -I../../../src -I../../../extern/fmt/include

ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += -DDEMI_DEBUG_BUILD -D_GLIBCXX_DEBUG -DDEMI_DEBUG_MODE
    LIB_SUFFIX := -debug
else
    LIB_SUFFIX := -release
endif

LIB_STATIC := ../../../bin/libdemi$(LIB_SUFFIX).a
TARGET := embedding_05_stdin_cpp

.PHONY: all clean run run-debug assemble lib

all: $(TARGET)

$(TARGET): main.cpp $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CXX) $(CXXFLAGS) $< $(LIB_STATIC) -pthread -lstdc++fs -o $@
	@echo "✅ Built $(TARGET) successfully"

assemble: echo_input.hex

echo_input.hex: echo_input.asm
	@echo "🔨 Assembling echo_input.asm to raw bytecode..."
	../../../bin/demi-engine$(LIB_SUFFIX) -A $< -ao $@

$(LIB_STATIC):
	@echo "📦 building Demi static library in root folder..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

lib:
	@echo "📦 refreshing Demi static library..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

run: lib all assemble
	@echo "🚀 Running $(TARGET)..."
	./$(TARGET) echo_input.hex

run-debug: lib all assemble
	@echo "🚀 Running $(TARGET) in debug mode..."
	./$(TARGET) echo_input.hex --debug

clean:
	rm -f $(TARGET) echo_input.hex

