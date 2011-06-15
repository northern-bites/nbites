
# Makefile for using cmake to generate a project Makefile
# @author Octavian Neamtu

MAKE=make
CMAKE=cmake
CCMAKE=ccmake

MKDIR=mkdir
CD=cd

PROJECT_DIR=$(PWD)/../..
BIN_DIR=$(PROJECT_DIR)/build/QTool
SRC_DIR=$(PWD)
GEN_MAKEFILE=$(BIN_DIR)/Makefile

TOOLCHAIN_FILE=$(PROJECT_DIR)/src/man/cmake/generic.cmake
TOOLCHAIN_FLAG=-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE)

ECLIPSE_PROJECT_FILE=$(BIN_DIR)/.project
ECLIPSE_FLAGS=-G "Eclipse CDT4 - Unix Makefiles"

MAKE_FLAGS=--no-print-directory

.phony: build clean cfg eclipse

default: build

build: $(GEN_MAKEFILE)
	$(MAKE) $(MAKE_FLAGS) -C $(BIN_DIR)

$(GEN_MAKEFILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(TOOLCHAIN_FLAG) $(SRC_DIR)

eclipse: $(ECLIPSE_PROJECT_FILE)

$(ECLIPSE_PROJECT_FILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(TOOLCHAIN_FLAG) $(ECLIPSE_FLAGS) $(SRC_DIR)

cfg: $(GEN_MAKEFILE)
	$(CCMAKE) $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)
