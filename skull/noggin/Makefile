
.PHONY: all config clean install noconfig
.PHONY: visionall visionconfig visionclean visioninstall

CUR_DIR = $(shell pwd)
BUILD_DIR = build
CMAKE_DIR = cmake.noggin
CMAKE_CACHE = $(BUILD_DIR)/CMakeCache.txt
CMAKE_SRCS = \
	CMakeLists.txt \
	buildconfig.cmake \
	nogginconfig.h
CMAKE_SRC := $(addprefix $(CMAKE_DIR),$(CMAKE_SRC))
CONFIG_FILE = nogginconfig.h
SWITCH_FILE = noggin/players/Switch.py
VISION_DIR = $(abspath $(CUR_DIR)/../vision)

TRUNK_REVISION = r0 #$(shell svn info | grep Revision | awk 'FS=" " {print $$2}')
export TRUNK_REVISION

CD=cd
CMAKE=cmake
CCMAKE=ccmake
MKDIR=mkdir

.SILENT:

all: $(CMAKE_CACHE) visionall
	@$(MAKE) -C $(BUILD_DIR)

visionall:
	@$(MAKE) -C $(VISION_DIR) all

config: $(CMAKE_CACHE) visionconfig
	@set -e; \
		$(CD) $(BUILD_DIR); \
		$(CCMAKE) .; \

visionconfig: $(VISION_DIR)/$(CMAKE_CACHE)
	@$(MAKE) -C $(VISION_DIR) config

install: $(CMAKE_CACHE) visioninstall
	@$(MAKE) -C $(BUILD_DIR) install

visioninstall:
	@$(MAKE) -C $(VISION_DIR) install

clean: visionclean
	$(RM) -r $(BUILD_DIR) $(CONFIG_FILE) $(SWITCH_FILE)

visionclean:
	@$(MAKE) -C $(VISION_DIR) clean

$(CMAKE_CACHE):
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(CD) $(BUILD_DIR); \
		$(CMAKE) $(CUR_DIR)/$(CMAKE_DIR)

$(VISION_DIR)/$(CMAKE_CACHE):
	@$(MAKE) -C $(VISION_DIR) $(CMAKE_CACHE)
