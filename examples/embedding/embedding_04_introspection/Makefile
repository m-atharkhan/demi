# Makefile for Demi embedding example 04 (introspection, C++ API)

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
TARGET := embedding_04_introspection

.PHONY: all clean run lib

all: $(TARGET)

$(TARGET): main.cpp $(LIB_STATIC)
	@echo "🔗 Linking $(TARGET) (mode: $(BUILD_TYPE)) with libdemi..."
	$(CXX) $(CXXFLAGS) $< $(LIB_STATIC) -pthread -lstdc++fs -o $@
	@echo "✅ Built $(TARGET) successfully"

$(LIB_STATIC):
	@echo "📦 building Demi static library in root folder..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

lib:
	@echo "📦 refreshing Demi static library..."
	$(MAKE) -C ../../.. lib-all BUILD_TYPE=$(BUILD_TYPE)

run: lib all
	@echo "🚀 Running $(TARGET)..."
	./$(TARGET)

clean:
	rm -f $(TARGET)
