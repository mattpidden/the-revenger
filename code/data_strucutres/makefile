# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
PROF_FLAGS = #-pg  # Profiling flag

# Directories
SRC_DIR = .
OBJ_DIR = build

# Find all .cpp files except vis.cpp
SRC = $(filter-out $(SRC_DIR)/vis.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Output executable
EXEC = solver
VIS_EXEC = vis  # Output for vis.cpp

# Ensure build directory exists
.PHONY: all clean profile

all: $(EXEC) $(VIS_EXEC)

# Compile vis.cpp separately
$(VIS_EXEC): $(SRC_DIR)/vis.cpp
	$(CXX) $(SRC_DIR)/vis.cpp -o $(VIS_EXEC) -lGL -lGLU -lglut

# Compile .cpp files into .o files inside build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(PROF_FLAGS) -c $< -o $@

# Link all object files into the final executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) $(PROF_FLAGS) -o $(EXEC)

# Create the build directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Run profiling
profile: $(EXEC)
	./$(EXEC)  # Run program to generate gmon.out
	gprof $(EXEC) gmon.out > profile_report.txt
	cat profile_report.txt  # Display results

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(EXEC) $(VIS_EXEC) gmon.out profile_report.txt
