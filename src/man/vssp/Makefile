
MAKE=make
CMAKE=cmake
CCMAKE=ccmake

MKDIR=mkdir
CD=cd

PROJECT_DIR=$(PWD)
BIN_DIR=$(PROJECT_DIR)/bin
SRC_DIR=$(PROJECT_DIR)
GEN_MAKEFILE=$(BIN_DIR)/Makefile

export NBITES_DIR=$(PROJECT_DIR)/../../..

EXECUTABLE=$(BIN_DIR)/vssp_tool

MAKE_FLAGS=--no-print-directory

VSSP_TARGET=vssp_tool

.PHONY: build clean

default: build

build: $(GEN_MAKEFILE)
	$(MAKE) $(MAKE_FLAGS) $(VSSP_TARGET) -C $(BIN_DIR)

$(GEN_MAKEFILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(SRC_DIR)

clean:
	rm -rf $(BIN_DIR)

$(EXECUTABLE): build

run: $(EXECUTABLE)
	$(EXECUTABLE)
