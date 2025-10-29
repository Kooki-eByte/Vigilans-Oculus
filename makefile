# ========================
# Vigilans Oculus Makefile
# ========================

APP_NAME := vigilans-oculus
GAME     := src

SRC_CORE := $(wildcard ./src/*.cpp) $(wildcard ./src/**/*.cpp)
BIN      := bin

# Detect OS
ifeq ($(OS),Windows_NT)
    # ----------------------
    # Windows Configuration
    # ----------------------
    EXE_EXT := .exe
    EXE := $(BIN)/testbed$(EXE_EXT)

    CC := g++
    CXXSTD := -std=c++11
    CXXFLAGS := -Wall -Werror -O2

    # Vulkan SDK setup (Windows typically has environment variable VULKAN_SDK)
    VULKAN_SDK_ENV := $(VULKAN_SDK)
    VULKAN_SDK_INCLUDE := $(VULKAN_SDK_ENV)/Include
    VULKAN_SDK_LIB := $(VULKAN_SDK_ENV)/Lib

    SRC_INCLUDE_FLAGS := -I$(VULKAN_SDK_INCLUDE) -Iinclude
    SRC_LINKER_FLAGS := -L$(VULKAN_SDK_LIB) -lvulkan-1 -lglfw3 -lgdi32 -luser32 -lkernel32
    SRC_LINKER_FLAGS += -Llib

else
    # ----------------------
    # Linux Configuration
    # ----------------------
    EXE := $(BIN)/testbed

    CC := g++
    CXXSTD := -std=c++11
    CXXFLAGS := -Wall -Werror -Wformat -fPIC -O2

    VULKAN_SDK_ENV := $(VULKAN_SDK)
    VULKAN_SDK_INCLUDE := $(VULKAN_SDK_ENV)/x86_64/include
    VULKAN_SDK_LIB := $(VULKAN_SDK_ENV)/x86_64/lib

    SRC_INCLUDE_FLAGS := -I$(VULKAN_SDK_INCLUDE)
    SRC_LINKER_FLAGS := -lglfw -lX11 -lXxf86vm -lpthread -lXrandr -lXi -ldl -lm \
                        -L$(VULKAN_SDK_LIB) -lvulkan -lwayland-client -lwayland-cursor -lwayland-egl

    RM := rm -rf
    MKDIR := mkdir -p $(BIN)
endif

.PHONY: all test clean

all:
	$(CC) $(CXXSTD) $(CXXFLAGS) $(SRC_CORE) -o $(EXE) $(SRC_INCLUDE_FLAGS) $(SRC_LINKER_FLAGS)

test: all
	@echo "Running $(EXE)..."
	$(EXE)

clean:
	$(RM) $(BIN)/*