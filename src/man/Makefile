# Main man makefile

# Toolchain for cross-compiling for the robot
ATOM_TOOLCHAIN = cmake/atom.cmake
# Toolchain for building natively
STRAIGHT_TOOLCHAIN = cmake/generic.cmake

# Where the build output will be placed
BUILD_DIR = build
# Where the files will be installed
INSTALL_PREFIX = install/

# Used to determine which kind of build
STRAIGHT_FILE = $(BUILD_DIR)/straight
CROSS_FILE = $(BUILD_DIR)/cross

# Different flags depending on the type of build
CMAKE_CROSS_FLAGS = -DCMAKE_TOOLCHAIN_FILE=$(ATOM_TOOLCHAIN) -DCMAKE_INSTALL_PREFIX=install/
CMAKE_STRAIGHT_FLAGS = -DCMAKE_TOOLCHAIN_FILE=$(STRAIGHT_TOOLCHAIN) -DCMAKE_INSTALL_PREFIX=install/

# Name of the script that copies files to the robot
UPLOAD_SCRIPT = $(BUILD_DIR)/upload.sh

.PHONY: build
build: $(BUILD_DIR)/Makefile
	make -C $(BUILD_DIR) --no-print-directory

.PHONY: install
install: $(BUILD_DIR)/Makefile
	@if [ -e $(CROSS_FILE) ]; then \
		make install -C $(BUILD_DIR) --no-print-directory; \
		$(UPLOAD_SCRIPT) \
	else \
		echo "You must build cross to install to a robot."; \
	fi

.PHONY: cross
cross:  $(CROSS_FILE)
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake  $(CMAKE_CROSS_FLAGS) ..; \
	ccmake ..

$(CROSS_FILE):
	@if [ -e $(STRAIGHT_FILE) ]; then \
		rm -r $(BUILD_DIR)/*; \
	fi
	@touch $(CROSS_FILE)

.PHONY: straight
straight: $(STRAIGHT_FILE)
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); \
	cmake  $(CMAKE_STRAIGHT_FLAGS) ..; \
	ccmake ..

$(STRAIGHT_FILE):
	@if [ -e $(CROSS_FILE) ]; then \
		rm -r $(BUILD_DIR)/*; \
	fi
	@touch $(STRAIGHT_FILE)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*

.PHONY: check
check:
	@if [ -e $(CROSS_FILE) ]; then \
		echo "Making cross."; \
	elif [ -e $(STRAIGHT_FILE) ]; then \
		echo "Making straight."; \
	fi
