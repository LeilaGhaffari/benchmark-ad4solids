# Variables for Enzyme and ADOL-C paths
ENZYME_LIB ?=
ADOLC_INCLUDE ?=
ADOLC_LIB ?=

# Compiler and Flags
CC = clang-18
CXX = g++
CFLAGS = $(OPT) -Wall -Wextra -Wunused-variable -Wunused-function -Iinclude
CXXFLAGS = -std=c++11 -Wall -Wextra -Wunused-variable -Wunused-function -Iinclude -I$(ADOLC_INCLUDE)
LDFLAGS = -lm

# Add Enzyme-specific flags if ENZYME_LIB is defined
ifneq ($(ENZYME_LIB),)
    CFLAGS += -Xclang -load -Xclang $(ENZYME_LIB)
endif

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Source files
SRC_BENCH = $(SRCDIR)/benchmark.cpp
SRC_ADOLC = $(SRCDIR)/ad-tools/adolc.cpp
SRC_ENZYME = $(SRCDIR)/ad-tools/enzyme.c

# Object files
OBJ_BENCH = $(BUILDDIR)/benchmark.o
OBJ_ADOLC = $(BUILDDIR)/adolc.o
OBJ_ENZYME = $(BUILDDIR)/enzyme.o

# Executable name
TARGET = $(BUILDDIR)/elasticity-exec

all: $(TARGET)

# Link object files to create the single executable
$(TARGET): $(OBJ_BENCH) $(OBJ_ADOLC) $(OBJ_ENZYME)
	$(CXX) $(CXXFLAGS) -I$(ADOLC_INCLUDE) -I$(INCDIR) -L$(ADOLC_LIB) -o $@ $(OBJ_BENCH) $(OBJ_ADOLC) $(OBJ_ENZYME) -ladolc $(LDFLAGS)

# Compile the benchmark program (C++)
$(OBJ_BENCH): $(SRC_BENCH) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Compile the ADOL-C source files (C++)
$(OBJ_ADOLC): $(SRC_ADOLC) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(ADOLC_INCLUDE) -I$(INCDIR) -c $< -o $@

# Compile the Enzyme source files (C)
$(OBJ_ENZYME): $(SRC_ENZYME) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure the build directory exists
$(BUILDDIR):
	mkdir -p $@

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

.PHONY: all clean
