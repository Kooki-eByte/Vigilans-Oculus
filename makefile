# ========================
# Vigilans Oculus Makefile
# ========================

APP_NAME 	:= vigilans-oculus
GAME 			:= src

SRC_CORE := $(wildcard ./src/*.cpp) $(wildcard ./src/**/*.cpp)
BIN := bin

LINUX_EXE := $(BIN)/testbed

# Vulkan SDK Setup
VULKAN_SDK_ENV := $(VULKAN_SDK)
VULKAN_SDK_INCLUDE := $(VULKAN_SDK_ENV)/x86_64/include
VULKAN_SDK_LIB := $(VULKAN_SDK_ENV)/x86_64/lib

# Compiler Settings
CC := g++
CXXFLAGS := -Wall -Werror -Wformat -fPIC
CXXFLAGS += -g # DEBUG field
CXXSTD := -std=c++11

all:
	$(CC) $(CXXSTD) $(CXXFLAGS) $(SRC_CORE) -o $(LINUX_EXE)

clean:
	rm -rf $(BIN)/*
