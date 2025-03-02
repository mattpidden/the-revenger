# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Source files
CUBE_SRC = cube.cpp
CUBE_OBJ = cube.o

# Executables and their sources
SOLVER_SRC = main.cpp
SOLVER_OBJ = main.o
SOLVER_EXEC = solver

VIS_SRC = vis.cpp
VIS_OBJ = vis.o
VIS_EXEC = vis

GEN_SRC = generate_tables.cpp
GEN_OBJ = generate_tables.o
GEN_EXEC = generate_tables

# Default target: build all executables
all: $(SOLVER_EXEC) $(VIS_EXEC) $(GEN_EXEC)

# Compile solver (main.cpp + cube.cpp)
$(SOLVER_EXEC): $(SOLVER_OBJ) $(CUBE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile vis (vis.cpp + cube.cpp)
$(VIS_EXEC): $(VIS_OBJ) $(CUBE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lGL -lGLU -lglut

# Compile generate_tables (generate_tables.cpp + cube.cpp)
$(GEN_EXEC): $(GEN_OBJ) $(CUBE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files into object files
%.o: %.cpp cube.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(SOLVER_EXEC) $(VIS_EXEC) $(GEN_EXEC) *.o
