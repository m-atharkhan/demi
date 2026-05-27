# Makefile for Demi embedding example 06 (stdin hook, C API)

BUILD_TYPE ?= debug

CC := gcc
CFLAGS := -Wall -Wextra -O2 -I../../../include -I../../../src

ifeq ($(BUILD_TYPE),debug)
    CFLAGS += -DDEMI_DEBUG_BUILD
    LIB_SUFFIX := -debug
else
    LIB_SUFFIX := -release
endif

LIB_STATIC := ../../../bin/libdemi$(LIB_SUFFIX).a
TARGET := embedding_06_stdin_c

.PHONY: all clean run assemble lib

all: $(TARGET)

$(TARGET): main.c $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CC) $(CFLAGS) $< $(LIB_STATIC) -lstdc++fs -lstdc++ -lm -pthread -o $@
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

clean:
	rm -f $(TARGET) echo_input.hex

