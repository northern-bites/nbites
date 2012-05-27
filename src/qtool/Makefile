
# Makefile for using cmake to generate a project Makefile
# @author Octavian Neamtu

MAKE=make
CMAKE=cmake
CCMAKE=ccmake

GDB=gdb

MKDIR=mkdir
CD=cd

PROJECT_DIR=$(PWD)/../..
BIN_DIR=$(PROJECT_DIR)/build/qtool
SRC_DIR=$(PROJECT_DIR)/src
GEN_MAKEFILE=$(BIN_DIR)/Makefile

TOOLCHAIN_FILE=$(PROJECT_DIR)/src/man/cmake/generic.cmake
TOOLCHAIN_FLAG=-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE)

ECLIPSE_PROJECT_FILE=$(BIN_DIR)/.project
ECLIPSE_FLAGS=-G "Eclipse CDT4 - Unix Makefiles"

EXECUTABLE=$(BIN_DIR)/qtool/qtool

CMAKE_DEBUG_CONFIG=-DCMAKE_BUILD_TYPE=debug
CMAKE_RELEASE_CONFIG=-DCMAKE_BUILD_TYPE=release

MAKE_FLAGS=--no-print-directory

OVERSEER_SERVER_TARGET=overseer_server
QTOOL_TARGET=qtool

OVERSEER_SERVER_LIB=$(BIN_DIR)/qtool/overseer/liboverseer_server.so
OVERSEER_DESTINATION_DIR=$(SRC_DIR)/overseer/austinvilla/ground_truth/lib/

.phony: build clean cfg eclipse

default: build

build: $(GEN_MAKEFILE)
	$(MAKE) $(MAKE_FLAGS) $(QTOOL_TARGET) -C $(BIN_DIR)

$(GEN_MAKEFILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(TOOLCHAIN_FLAG) $(CMAKE_RELEASE_CONFIG) $(SRC_DIR)

release: clean $(GEN_MAKEFILE)

eclipse: $(ECLIPSE_PROJECT_FILE)

$(ECLIPSE_PROJECT_FILE):
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(TOOLCHAIN_FLAG) $(CMAKE_RELEASE_CONFIG) $(ECLIPSE_FLAGS) $(SRC_DIR)

cfg: $(GEN_MAKEFILE)
	$(CCMAKE) $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

clean_soft:
	$(MAKE) clean -C $(BIN_DIR)

$(EXECUTABLE): build

run: $(EXECUTABLE)
	$(EXECUTABLE) # add R_HOME=/usr/lib/R to the beginning for R to work.

debug: 
	$(MKDIR) -p $(BIN_DIR); \
	$(CD) $(BIN_DIR); \
	$(CMAKE) $(TOOLCHAIN_FLAG) $(CMAKE_DEBUG_CONFIG) .

run_gdb: debug
	$(GDB) $(EXECUTABLE)

overseer_server: $(GEN_MAKEFILE)
	$(MAKE) $(OVERSEER_SERVER_TARGET) $(MAKE_FLAGS) -C $(BIN_DIR); \
	$(MKDIR) -p $(OVERSEER_DESTINATION_DIR); \
	cp $(OVERSEER_SERVER_LIB) $(OVERSEER_DESTINATION_DIR)
