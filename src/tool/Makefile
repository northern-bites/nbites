# The makefile!

# How to find the build dir from here
BUILD_DIR = ../../build/tool
# How to find the src dir from build
SRC_DIR = ../../src/tool

TOOLCHAIN = ../share/cmake/generic.cmake
EXECUTABLE = tool
FLAGS = -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) -DTOOL_EXECUTABLE=$(EXECUTABLE)

.PHONY: build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(FLAGS) $(SRC_DIR); \
	$(MAKE) --no-print-directory

.PHONY: run
run: build
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
