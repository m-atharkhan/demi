# Makefile for Demi embedding example 03 (output capture)

BUILD_TYPE ?= debug

CC := gcc
CFLAGS := -Wall -Wextra -O2 -I../../../src

ifeq ($(BUILD_TYPE),debug)
    CFLAGS += -DDEMI_DEBUG_BUILD
    LIB_SUFFIX := -debug
else
    LIB_SUFFIX := -release
endif

LIB_STATIC := ../../../bin/libdemi$(LIB_SUFFIX).a
TARGET := embedding_03_output_capture

.PHONY: all clean run assemble

all: $(TARGET)

$(TARGET): main.c $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CC) $(CFLAGS) $< $(LIB_STATIC) -lstdc++fs -lstdc++ -pthread -o $@
	@echo "✅ Built $(TARGET) successfully"

assemble: hello.hex

hello.hex: hello.asm
	@echo "🔨 Assembling hello.asm to raw bytecode..."
	../../../bin/demi-engine$(LIB_SUFFIX) -A $< -ao $@

$(LIB_STATIC):
	@echo "📦 building Demi static library in root folder..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

run: all assemble
	@echo "🚀 Running $(TARGET)..."
	./$(TARGET) hello.hex

clean:
	rm -f $(TARGET) hello.hex
