
CUR_DIR = $(shell pwd)
BUILD_DIR = build
INSTALL_DIR = install
INSTALL_SCRIPT = ./upload.sh
CMAKE_DIR = cmake.man
CMAKE_DEPEND = $(BUILD_DIR)/Makefile
CMAKE_SRCS = \
	CMakeLists.txt \
	buildconfig.cmake \
	manconfig.in
CMAKE_SRCS := $(addprefix $(CMAKE_DIR),$(CMAKE_SRCS))
CONFIG_FILE = manconfig.h
CROSS_FILE = $(BUILD_DIR)/cross
STRAIGHT_FILE = $(BUILD_DIR)/straight
WEBOTS_FILE = $(BUILD_DIR)/webots

PYC_FILES = $(shell find . -name "*.pyc")

CROSS_TOOLCHAIN = $(abspath $(CURDIR)/cmake/geode.cmake)
CMAKE_DEFS = -DOE_CROSS_BUILD=OFF -DSTRAIGHT=ON
CMAKE_CROSS_DEFS = -DCMAKE_TOOLCHAIN_FILE=$(CROSS_TOOLCHAIN) -DOE_CROSS_BUILD=ON
CMAKE_WEBOTS_DEFS = -DOE_CROSS_BUILD=OFF -DMAN_IS_REMOTE_=ON -DWEBOTS_BACKEND=ON -DCOLOR_TABLE=tables/webots.mtb
CCMAKE_CROSS_OPT = -DCMAKE_SHARED_LINKER_FLAGS:STRING="" -DCMAKE_MODULE_LINKER_FLAGS:STRING=""

CD=cd
CMAKE=cmake
CCMAKE=ccmake
MKDIR=mkdir
TOUCH=touch
MAKE_OPTIONS=--no-print-directory


.PHONY: default_target all straight cross cclean clean install noconfig
.PHONY: clean_install


default_target: all

all: $(CMAKE_DEPEND)
	@#echo $(TRUNK_REVISION)
	@$(MAKE) $(MAKE_OPTIONS) -C $(BUILD_DIR)

straight: $(STRAIGHT_FILE)
	@set -e; \
		$(CD) $(BUILD_DIR); \
		$(CCMAKE) .

$(CMAKE_DEPEND):
	@if [ -f $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) webots; \
	elif [ -f $(STRAIGHT_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) straight; \
	else \
		$(MAKE) $(MAKE_OPTIONS) cross; \
	fi

$(STRAIGHT_FILE):
	@if [ -e $(CROSS_FILE) ]  || [ -f $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) clean; \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(STRAIGHT_FILE); \
		$(CD) $(BUILD_DIR); \
		$(CMAKE) $(CMAKE_DEFS) $(CURDIR)/$(CMAKE_DIR)

cross: $(CROSS_FILE)
	@set -e ; \
		$(CD) $(BUILD_DIR); \
		$(CCMAKE) $(CCMAKE_CROSS_OPT) .

$(CROSS_FILE):
	@if [ -e $(STRAIGHT_FILE) ] || [ -e $(WEBOTS_FILE) ]; then \
	       $(MAKE) clean; \
        fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(CROSS_FILE); \
		$(CD) $(BUILD_DIR); \
		$(CMAKE) $(CMAKE_CROSS_DEFS) $(CURDIR)/$(CMAKE_DIR)


webots: $(WEBOTS_FILE)
	@set -e;
		$(CD) $(BUILD_DIR); \
		$(CCMAKE) .

$(WEBOTS_FILE):
	@if [ -e $(STRAIGHT_FILE) ] || [ -e $(CROSS_FILE) ]; then \
	       $(MAKE) clean; \
        fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(WEBOTS_FILE); \
		$(CD) $(BUILD_DIR); \
		$(CMAKE) $(CMAKE_WEBOTS_DEFS) $(CURDIR)/$(CMAKE_DIR)


install: $(CMAKE_DEPEND)
	@$(MAKE) $(MAKE_OPTIONS) -C $(BUILD_DIR) install
	$(INSTALL_SCRIPT)

clean_install:
	$(RM) -r $(INSTALL_DIR)/*

cclean: $(CMAKE_DEPEND)
	@$(MAKE) $(MAKE_OPTIONS) -C $(BUILD_DIR) clean

clean:
	$(RM) -r $(BUILD_DIR) $(CONFIG_FILE) $(INSTALL_SCRIPT)
	@$(MAKE) -C corpus clean
	@$(MAKE) -C motion clean
	@$(MAKE) -C noggin clean
	@$(MAKE) -C vision clean
	@$(MAKE) -C comm   clean
	$(RM) -r install/* $(PYC_FILES)

check:
	@if [ -e $(CROSS_FILE) ]; then \
		echo "Making for CROSS"; \
	elif [ -e $(STRAIGHT_FILE) ]; then \
		echo "Making for STRAIGHT"; \
	elif [ -e $(WEBOTS_FILE) ]; then \
		echo "Making for WEBOTS"; \
	fi
