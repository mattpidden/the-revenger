# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Ofast -march=native -flto

# Source files
CUBE_SRC = cube.cpp
CUBE_OBJ = cube.o

# Executables and their sources
SOLVER_SRC = main.cpp
SOLVER_OBJ = main.o
SOLVER_EXEC = solver

GEN_SRC = generate_tables.cpp
GEN_OBJ = generate_tables.o
GEN_EXEC = generate_tables

# Default target: build all executables
all: $(SOLVER_EXEC) $(GEN_EXEC)

# Compile solver (main.cpp + cube.cpp)
$(SOLVER_EXEC): $(SOLVER_OBJ) $(CUBE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# Compile generate_tables (generate_tables.cpp + cube.cpp)
$(GEN_EXEC): $(GEN_OBJ) $(CUBE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# Compile source files into object files
%.o: %.cpp cube.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -pthread

# Clean build files
clean:
	rm -f $(SOLVER_EXEC) $(GEN_EXEC) *.o
