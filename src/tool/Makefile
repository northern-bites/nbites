# The makefile!

# How to find the build dir from here
BUILD_DIR = ../../build/tool
# How to find the src dir from build
SRC_DIR = ../../src/tool

.PHONY: build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(SRC_DIR); \
	$(MAKE) --no-print-directory

.PHONY: config
config:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake $(SRC_DIR); \
	ccmake $(SRC_DIR)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*