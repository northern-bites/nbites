# The makefile!

# How to find the build dir from here
BUILD_DIR = ../../build/tool
# How to find the src dir from build
SRC_DIR = ../../src/tool

EXECUTABLE = $(BUILD_DIR)/tool

.PHONY: build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(SRC_DIR) -DTOOL_EXECUTABLE=$(EXECUTABLE); \
	$(MAKE) --no-print-directory

.PHONY: run
run: build
	$(EXECUTABLE)

.PHONY: config
config:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(SRC_DIR) -DTOOL_EXECUTABLE=$(EXECUTABLE); \
	ccmake $(SRC_DIR)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*