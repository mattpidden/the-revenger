# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
PROF_FLAGS = -pg  # Profiling flag

# Directories
SRC_DIR = .
OBJ_DIR = build

# Find all .cpp files and corresponding .o files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Output executable
EXEC = cube

# Ensure build directory exists (Prevent conflict with real directory)
.PHONY: all clean profile

all: $(EXEC)

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
	rm -rf $(OBJ_DIR) $(EXEC) gmon.out profile_report.txt
