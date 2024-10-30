# Variables for Enzyme, ADOL-C, and Tapenade paths
ENZYME_LIB ?=
ADOLC_INCLUDE ?=
ADOLC_LIB ?=

# Compilers
CC = clang-18
CXX = g++
FC = gfortran

# Flags
CFLAGS = $(OPT) -Wall -Wextra -Wunused-variable -Wunused-function -Iinclude
CXXFLAGS = $(OPT) -std=c++11 -Wall -Wextra -Wunused-variable -Wunused-function \
            -Wno-unused-parameter $(patsubst %,-I%,$(INCDIR) $(ADOLC_INCLUDE))
FFLAGS = $(OPT) -fPIC -cpp -Wall -Wextra -Wno-unused-parameter \
         -Wno-unused-dummy-argument -MMD -MP
LDFLAGS = -lm -lgfortran  # Link against Fortran runtime

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
SOURCES_F90 = $(wildcard $(ADTOOLSDIR)/*.f90)

# Object files
OBJ_CXX = $(SOURCES_CXX:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o) \
           $(SOURCES_CXX:$(ADTOOLSDIR)/%.cpp=$(BUILDTOOLSDIR)/%.o)
OBJ_C = $(SOURCES_C:$(ADTOOLSDIR)/%.c=$(BUILDTOOLSDIR)/%.o)
OBJ_F90 = $(SOURCES_F90:$(ADTOOLSDIR)/%.f90=$(BUILDTOOLSDIR)/%.o)

# All object files
OBJ = $(OBJ_CXX) $(OBJ_C) $(OBJ_F90)

# Executable name
TARGET = $(BUILDDIR)/elasticity-exec

# Default target
all: $(TARGET)

# Link object files to create the single executable
$(TARGET): $(OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $@

# Compile C++ source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.cpp | $(BUILDTOOLSDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C source files
$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.c | $(BUILDTOOLSDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Fortran source files
$(BUILDTOOLSDIR)/%.o: $(ADTOOLSDIR)/%.f90 | $(BUILDTOOLSDIR)
	$(FC) $(FFLAGS) -c $< -o $@

# Ensure necessary directories exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDTOOLSDIR):
	mkdir -p $(BUILDTOOLSDIR)

# Clean up build artifacts
clean:
	rm -f $(BUILDDIR)/*.o $(BUILDTOOLSDIR)/*.o $(TARGET)

.PHONY: all clean
