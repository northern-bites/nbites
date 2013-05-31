# The makefile!

PROJECT_DIR  = $(PWD)/../..
BUILD_DIR    = $(PROJECT_DIR)/build/tool
SRC_DIR      = $(PROJECT_DIR)/src/tool
GEN_MAKEFILE = $(BIN_DIR)/Makefile

TOOLCHAIN    = ../share/cmake/generic.cmake
FLAGS        = -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN)

EXECUTABLE = tool
LOC_EXECUTABLE = loc_tool
VISION_EXECUTABLE = vision_tool



.PHONY: build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(FLAGS) $(SRC_DIR); \
	$(MAKE) --no-print-directory

.PHONY: tool
tool: build
	@echo "Running tool executable."
	@cd $(BUILD_DIR); \
	./$(EXECUTABLE)

.PHONY: config
config:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(FLAGS) $(SRC_DIR); \
	ccmake $(SRC_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: loc
loc: build
	@echo "Running LocTool executable."
	@cd $(BUILD_DIR); \
	./$(LOC_EXECUTABLE)

.PHONY: vision
vision: build
	@echo "Running VisionTool executable."
	@cd $(BUILD_DIR); \
	./$(VISION_EXECUTABLE)

