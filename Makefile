# Variables for Enzyme and ADOL-C paths
ENZYME_LIB ?=
ADOLC_INCLUDE ?=
ADOLC_LIB ?=

# Compiler and Flags
CC = clang-18
CXX = g++
CFLAGS = $(OPT) -Wall -Wextra -Wunused-variable -Wunused-function -Iinclude
CXXFLAGS = $(OPT) -std=c++11 -Wall -Wextra -Wunused-variable -Wunused-function -Wno-unused-parameter $(patsubst %,-I%,$(INCDIR) $(ADOLC_INCLUDE))
LDFLAGS = -lm

# Add Enzyme-specific flags if ENZYME_LIB is defined
ifneq ($(ENZYME_LIB),)
    CFLAGS += -Xclang -load -Xclang $(ENZYME_LIB)
endif

# Directories
SRCDIR = src
ADTOOLSDIR = $(SRCDIR)/ad-tools
INCDIR = include
BUILDDIR = build
LIBDIR = $(BUILDDIR)/lib
BUILDTOOLSDIR = $(BUILDDIR)/ad-tools

# Source files
SOURCES_CXX = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(ADTOOLSDIR)/*.cpp)
SOURCES_C = $(wildcard $(ADTOOLSDIR)/*.c)

# Object files
OBJ = $(SOURCES_CXX:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o) $(SOURCES_C:$(ADTOOLSDIR)/%.c=$(BUILDTOOLSDIR)/%.o)

# Executable name
TARGET = $(BUILDDIR)/elasticity-exec

# Default target
all: $(TARGET)

# Link object files to create the single executable
$(TARGET): $(OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(patsubst %,-L%,$(ADOLC_LIB)) -o $@ $^ -ladolc

# Compile C++ source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.cpp | $(BUILDTOOLSDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C source files
$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.c | $(BUILDTOOLSDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure necessary directories exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDTOOLSDIR):
	mkdir -p $(BUILDTOOLSDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# Clean up build artifacts
clean:
	rm -f $(BUILDDIR)/*.o $(BUILDTOOLSDIR)/*.o $(TARGET)

.PHONY: all clean
