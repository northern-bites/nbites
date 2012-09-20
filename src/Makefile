# Makefile for cmake

MAKE=make
CMAKE=cmake

MKDIR=mkdir
CD=cd

PROJECT_DIR=$(PWD)/..
BIN_DIR=$(PROJECT_DIR)/bin
SRC_DIR=$(PROJECT_DIR)/src
GEN_MAKEFILE=$(BIN_DIR)/Makefile

ECLIPSE_PROJECT_FILE=$(BIN_DIR)/.project
ECLIPSE_FLAGS=-G "Eclipse CDT4 - Unix Makefiles"

MAKE_FLAGS=--no-print-directory

EXECUTABLE=$(BIN_DIR)/visionsim

.phony: build clean cfg eclipse

build: $(GEN_MAKEFILE)
	$(MAKE) $(MAKE_FLAGS) -C $(BIN_DIR)

$(GEN_MAKEFILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(SRC_DIR)

eclipse: $(ECLIPSE_PROJECT_FILE)

$(ECLIPSE_PROJECT_FILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(ECLIPSE_FLAGS) $(SRC_DIR)

cfg: $(GEN_MAKEFILE)
	$(CCMAKE) $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

$(EXECUTABLE): build

run: $(EXECUTABLE)
	$(EXECUTABLE)