# Compilers
CXX = g++
FC = gfortran

# Flags
CXXFLAGS = -Wall -Wextra -std=c++11
FFLAGS = -Wall -Wextra

# Target executable
TARGET = exec.out

# Source files
SRC_CPP = test001.cpp
SRC_F90 = test001.f90

# Object files
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_F90 = $(SRC_F90:.f90=.o)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ_CPP) $(OBJ_F90)
	$(CXX) $(CXXFLAGS) $(OBJ_CPP) $(OBJ_F90) -lgfortran -o $(TARGET)

# Compile C++ source file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Fortran source file
%.o: %.f90
	$(FC) $(FFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
