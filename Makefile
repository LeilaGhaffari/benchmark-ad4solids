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
ADTOOLSDIR = $(SRCDIR)/ad-tools
INCDIR = include
BUILDDIR = build
BUILDTOOLSDIR = $(BUILDDIR)/ad-tools

# Source files
SOURCES_CXX = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(ADTOOLSDIR)/*.cpp)
SOURCES_C = $(wildcard $(ADTOOLSDIR)/*.c)

# Object files
OBJ = $(SOURCES_CXX:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o) $(SOURCES_C:$(ADTOOLSDIR)/%.c=$(BUILDTOOLSDIR)/%.o)

# Executable name
TARGET = $(BUILDDIR)/elasticity-exec

all: $(TARGET)

# Link object files to create the single executable
$(TARGET): $(OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(ADOLC_INCLUDE) -I$(INCDIR) -L$(ADOLC_LIB) -o $@ $(OBJ) -ladolc $(LDFLAGS)

# Compile C++ source files from both src/ and src/ad-tools/
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.cpp | $(BUILDTOOLSDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Compile C source files from src/ad-tools/
$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.c | $(BUILDTOOLSDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Ensure the build and build/ad-tools directories exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDTOOLSDIR):
	mkdir -p $(BUILDTOOLSDIR)

clean:
	rm -f $(BUILDDIR)/*.o $(BUILDTOOLSDIR)/*.o $(TARGET)

.PHONY: all clean
