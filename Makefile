# Copyright (c) 2024, Emily Dror

### Project Paramaters
TARGET := cacheSim

################################################################################
# Compiler and Linker Options
export CXX := clang++
export CXXFLAGS := -Wall -std=c++17

export LD := $(CXX)
export LD_FLAGS :=

################################################################################
### Process command line options to the makefile

# VERBOSE:
#   When not specified, suppress normal makefile output for a more terse version
#   of the output.  But, allow a user to see the full makefile output when this
#   option is specified.
ifneq ($(VERBOSE), 1)
    brief = @echo "[$(patsubst $(OBJ_DIR)/%,%,$(@))]";
    verb = @
else
    brief = @echo "";
    verb =
endif

# CCACHE:
#   If CCACHE is specified on the command line, do the following
#   - Modify CC to include "ccache" as a prefix
#   - Setup NOCCACHE_CC to always be a version of CC without ccache
#   - Don't use CCACHE for HOST_CXX as it is used for linking, which cannot be cached
ifdef CCACHE
CCACHE_AVAILABLE := $(shell which ccache > /dev/null; echo $$?)
ifeq ($(CCACHE_AVAILABLE),0)
$(info Enabling ccache for build)
export CXX := ccache $(CXX)
else
$(info CCACHE requested, but no ccache found)
endif
endif

################################################################################
### Directory Structure
ROOT := .

SRC_DIR := $(ROOT)/src
TEST_DIR := $(ROOT)/tests
BUILD_DIR := $(ROOT)/build
INCLUDE_DIRS := -I $(ROOT)/include

# Build Directory, Don't Change!!
OBJ_DIR := $(BUILD_DIR)/obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

################################################################################
### Rules

.PHONY: all
all: $(TARGET)

$(TARGET): BUILD_DIRS $(OBJ_FILES)
	$(brief)
	$(verb) $(LD) $(OBJ_FILES) $(LD_FLAGS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(brief)
	$(verb) $(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

BUILD_DIRS:
	@echo "\nUsing Development Build Flow\n"
	$(verb) mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	$(brief)
	$(verb) rm -rf $(BUILD_DIR)/* $(TARGET)

.PHONY: test
test:
	echo "Running tests..."
	# Add your test runner commands here, e.g., GoogleTest
