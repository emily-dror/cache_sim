#
#cacheSim: cacheSim.cpp
#	g++ -o cacheSim cacheSim.cpp
#
#.PHONY: clean
#clean:
#	rm -f *.o
#	rm -f cacheSim


# Makefile for cache simulator

# Compiler to use
CXX := g++
export CXXFLAGS := -Wall -std=c++11

# Source files
SRCS := cacheSim.cpp cache.cpp

# Output binary
TARGET := cacheSim

# Compiler flags
#CXXFLAGS := -Wall -g # -Wall enables all warnings; -g adds debug info

# Rule to build the target
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(TARGET)
