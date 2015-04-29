# The makefile!

PROJECT_DIR  = $(PWD)/../..
BUILD_DIR    = $(PROJECT_DIR)/build/nbcross
SRC_DIR      = $(PROJECT_DIR)/src/nbcross
GEN_MAKEFILE = $(BIN_DIR)/Makefile

TOOLCHAIN    = ../share/cmake/generic.cmake
FLAGS        = -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN)

EXECUTABLE = nbcross_tool
#LOC_EXECUTABLE = loc_tool
#BALL_EXECUTABLE = ball_tool
#VISION_EXECUTABLE = vision_tool
#CALIBRATE_EXECUTABLE = calibrate_tool
#PLAYBOOK_EXECUTABLE = playbook_tool
#SIM_EXECUTABLE = sim_tool
#INSTALLER_EXECUTABLE = installer_tool

.PHONY: build_nbcross
build_nbcross:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(FLAGS) $(SRC_DIR); \
	$(MAKE) --no-print-directory $(EXECUTABLE)

.PHONY: nbcross
nbcross: build_nbcross
	@echo "nbcross in da house!!"
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
