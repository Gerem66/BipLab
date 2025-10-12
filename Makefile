# BipLab Build System
.DEFAULT_GOAL := all
BUILD_DIR := build
EXECUTABLE := CellsEvolution

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Configure CMake for Debug build (generates build/Makefile)
$(BUILD_DIR)/Makefile: $(BUILD_DIR) CMakeLists.txt
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug ..

# Default build (Debug mode)
all: $(BUILD_DIR)/Makefile
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory

# Optimized build (Release mode)
release: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ..
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory

# Clean all generated files
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(EXECUTABLE)

# Build and run the application
run: all
	./$(EXECUTABLE)

# Full rebuild from scratch
rebuild: clean all

.PHONY: all release clean run rebuild
