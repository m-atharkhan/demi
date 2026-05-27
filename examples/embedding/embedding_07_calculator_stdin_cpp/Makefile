# Makefile for Demi embedding example 07 (interactive calculator, C++ API)

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
TARGET := embedding_07_calculator_stdin_cpp
GUEST_ASM := ../../x64/syscalls/line_calculator.asm

.PHONY: all clean run run-debug assemble lib

all: $(TARGET)

$(TARGET): main.cpp $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CXX) $(CXXFLAGS) $< $(LIB_STATIC) -pthread -lstdc++fs -o $@
	@echo "✅ Built $(TARGET) successfully"

assemble: line_calculator.hex

line_calculator.hex:
	@echo "🔨 Assembling line_calculator.asm to raw bytecode..."
	../../../bin/demi-engine$(LIB_SUFFIX) -A $(GUEST_ASM) -ao $@

$(LIB_STATIC):
	@echo "📦 building Demi static library in root folder..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

lib:
	@echo "📦 refreshing Demi static library..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

run: lib all assemble
	@echo "🚀 Running $(TARGET)..."
	./$(TARGET) line_calculator.hex

run-debug: lib all assemble
	@echo "🚀 Running $(TARGET) in debug mode..."
	./$(TARGET) line_calculator.hex --debug

clean:
	rm -f $(TARGET) line_calculator.hex

