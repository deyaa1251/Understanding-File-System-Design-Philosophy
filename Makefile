CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O2
INCLUDES = -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
RESULTS_DIR = results
SCRIPTS_DIR = scripts

# Source files
BTREE_SRC = $(SRC_DIR)/b_tree.cpp
BST_SRC = $(SRC_DIR)/bst.cpp
MAIN_SRC = $(SRC_DIR)/main.cpp
EXPORT_SRC = $(SRC_DIR)/csv_exporter.cpp

# Object files
BTREE_OBJ = b_tree.o
BST_OBJ = bst.o
MAIN_OBJ = main.o
EXPORT_OBJ = csv_exporter.o

# Executables
MAIN_EXEC = benchmark
EXPORT_EXEC = csv_export

.PHONY: all clean run export graphs results help

all: $(MAIN_EXEC) $(EXPORT_EXEC)

# Main benchmark executable
$(MAIN_EXEC): $(MAIN_OBJ) $(BTREE_OBJ) $(BST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# CSV export executable
$(EXPORT_EXEC): $(EXPORT_OBJ) $(BTREE_OBJ) $(BST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Object file rules
$(MAIN_OBJ): $(MAIN_SRC) $(INC_DIR)/b_tree.h $(INC_DIR)/bst.h $(INC_DIR)/benchmark.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(MAIN_SRC)

$(EXPORT_OBJ): $(EXPORT_SRC) $(INC_DIR)/b_tree.h $(INC_DIR)/bst.h $(INC_DIR)/benchmark.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(EXPORT_SRC)

$(BTREE_OBJ): $(BTREE_SRC) $(INC_DIR)/b_tree.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(BTREE_SRC)

$(BST_OBJ): $(BST_SRC) $(INC_DIR)/bst.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(BST_SRC)

# Run main benchmark
run: $(MAIN_EXEC)
	@echo "Running comprehensive benchmark..."
	@mkdir -p $(RESULTS_DIR)
	./$(MAIN_EXEC) | tee $(RESULTS_DIR)/benchmark_output.txt

# Export CSV data
export: $(EXPORT_EXEC)
	@echo "Exporting benchmark data to CSV..."
	@mkdir -p $(RESULTS_DIR)
	./$(EXPORT_EXEC)

# Generate graphs (requires Python with matplotlib, pandas, seaborn)
graphs: export
	@echo "Generating graphs..."
	@mkdir -p $(RESULTS_DIR)/graphs
	@if command -v python3 >/dev/null 2>&1; then \
		python3 $(SCRIPTS_DIR)/generate_graphs.py; \
	else \
		echo "Python3 not found. Install matplotlib, pandas, seaborn to generate graphs."; \
	fi

# Complete results generation
results: run export graphs
	@echo ""
	@echo "=========================================="
	@echo "All benchmarks and graphs completed!"
	@echo "Check the $(RESULTS_DIR)/ directory"
	@echo "=========================================="

# Clean build artifacts
clean:
	rm -f *.o $(MAIN_EXEC) $(EXPORT_EXEC)
	rm -rf $(RESULTS_DIR)

# Help menu
help:
	@echo "Available targets:"
	@echo "  make all      - Build all executables"
	@echo "  make run      - Run main benchmark"
	@echo "  make export   - Export data to CSV"
	@echo "  make graphs   - Generate PNG graphs"
	@echo "  make results  - Run everything (benchmark + export + graphs)"
	@echo "  make clean    - Remove build artifacts and results"
	@echo "  make help     - Show this help message"
