

.PHONY: all config clean install noconfig

CUR_DIR = $(shell pwd)
BUILD_DIR = build
CMAKE_DIR = cmake.vision
CMAKE_CACHE = $(BUILD_DIR)/CMakeCache.txt
CMAKE_SRCS = \
	CMakeLists.txt \
	buildconfig.cmake \
	profileconfig.h
CMAKE_SRC := $(addprefix $(CMAKE_DIR),$(CMAKE_SRC))
CONFIG_FILES = visionconfig.h profileconfig.h

TRUNK_REVISION = r0 #$(shell svn info | grep Revision | awk 'FS=" " {print $$2}')
export TRUNK_REVISION

CD=cd
CMAKE=cmake
CCMAKE=ccmake
MKDIR=mkdir

.SILENT:

all: $(CMAKE_CACHE)
	@$(MAKE) -C $(BUILD_DIR)

config: $(CMAKE_CACHE)
	@set -e; \
		$(CD) $(BUILD_DIR); \
		$(CCMAKE) .; \

install: $(CMAKE_CACHE)
	@$(MAKE) -C $(BUILD_DIR) install

clean:
	$(RM) -r $(BUILD_DIR) $(CONFIG_FILES)

$(CMAKE_CACHE):
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(CD) $(BUILD_DIR); \
		$(CMAKE) $(CUR_DIR)/$(CMAKE_DIR)

