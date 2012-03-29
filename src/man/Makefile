#Directory setup
CUR_DIR = $(shell pwd)
NBITES_DIR = $(CUR_DIR)/../..
BUILD_DIR = $(NBITES_DIR)/build/man
MAN_DIR = $(CUR_DIR)

ATOM_DIR = $(BUILD_DIR)/atom
CROSS_DIR = $(BUILD_DIR)/cross
WEBOTS_DIR = $(BUILD_DIR)/webots
CROSS_INSTALL_DIR = $(BUILD_DIR)/cross_install
WEBOTS_INSTALL_DIR = $(BUILD_DIR)/webots_install
ATOM_INSTALL_DIR = $(BUILD_DIR)/atom_install
CMAKE_DIR = 
CMAKE_ATOM_DEPEND = $(ATOM_DIR)/Makefile
CMAKE_WEBOTS_DEPEND = $(WEBOTS_DIR)/Makefile
CMAKE_CROSS_DEPEND = $(CROSS_DIR)/Makefile

#File setup
CONFIG_FILE = manconfig.h
INSTALL_SCRIPT = upload.sh
CROSS_FILE = $(BUILD_DIR)/cross_file
ATOM_FILE = $(BUILD_DIR)/atom_file
WEBOTS_FILE = $(BUILD_DIR)/webots_file

PYC_FILES = $(shell find . -name "*.pyc")

#general definitions
#TODO: in cmake make everything relative to NBITES_DIR
CMAKE_DEFS = -DNBITES_DIR=$(NBITES_DIR)
#toolchains
CROSS_TOOLCHAIN = $(abspath $(CURDIR)/cmake/geode.cmake)
ATOM_TOOLCHAIN = $(abspath $(CURDIR)/cmake/atom.cmake)
#specialized defs
CMAKE_ATOM_DEFS = $(CMAKE_DEFS) -DCMAKE_TOOLCHAIN_FILE=$(ATOM_TOOLCHAIN) -DMAN_INSTALL_PREFIX=$(ATOM_INSTALL_DIR)
CMAKE_CROSS_DEFS = $(CMAKE_DEFS) -DCMAKE_TOOLCHAIN_FILE=$(CROSS_TOOLCHAIN) -DMAN_INSTALL_PREFIX=$(CROSS_INSTALL_DIR)
CMAKE_WEBOTS_DEFS = $(CMAKE_DEFS) -DOE_CROSS_BUILD=OFF -DMAN_IS_REMOTE_=ON -DWEBOTS_BACKEND=ON -DCOLOR_TABLE=../../tables/uvy/webots/webots_6_2.mtb -DMAN_INSTALL_PREFIX=$(WEBOTS_INSTALL_DIR)
#Commands setup
CD=cd
RM=rm
CMAKE=cmake
CCMAKE=ccmake
CCMAKE_GUI=cmake-gui
MKDIR=mkdir
TOUCH=touch
MAKE_OPTIONS=--no-print-directory
MAN_TARGET=man manpreloader

.PHONY: default_target all atom cross cclean clean install noconfig
.PHONY: clean_install


default_target: all

all: $(CMAKE_ATOM_DEPEND) $(CMAKE_WEBOTS_DEPEND) $(CMAKE_CROSS_DEPEND)
	@if [ -e $(CROSS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(CROSS_DIR) ${MAN_TARGET}; \
	elif [ -e $(ATOM_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(ATOM_DIR) ${MAN_TARGET}; \
	elif [ -e $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(WEBOTS_DIR) ${MAN_TARGET}; \
	fi

$(CMAKE_WEBOTS_DEPEND):
	@if [ -f $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) webots; \
	fi

$(CMAKE_ATOM_DEPEND):
	@if [ -f $(ATOM_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) atom; \
	fi

$(CMAKE_CROSS_DEPEND):
	@if [ -f $(CROSS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) cross; \
	fi

build_atom:
	$(MAKE) $(MAKE_OPTIONS) -C $(ATOM_DIR) ${MAN_TARGET}; \

atom: $(ATOM_FILE)
	@set -e;
		$(CD) $(ATOM_DIR); \
		$(CCMAKE) .

atom_gui: $(ATOM_FILE)
	@set -e;
		$(CD) $(ATOM_DIR); \
		$(CCMAKE_GUI) .

$(ATOM_FILE):
	@if [ -e $(CROSS_FILE) ]; then \
		$(RM) $(CROSS_FILE); \
	fi
	@if [ -e $(WEBOTS_FILE) ]; then \
		$(RM) $(WEBOTS_FILE); \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(ATOM_FILE); \
		$(MKDIR) -p $(ATOM_DIR); \
		$(CD) $(ATOM_DIR); \
		$(CMAKE) $(CMAKE_ATOM_DEFS) $(CURDIR)/$(CMAKE_DIR)

build_cross:
	$(MAKE) $(MAKE_OPTIONS) -C $(CROSS_DIR) ${MAN_TARGET}; \


cross: $(CROSS_FILE)
	@set -e ; \
		$(CD) $(CROSS_DIR); \
		$(CCMAKE) $(CCMAKE_CROSS_OPT) .

cross_gui: $(CROSS_FILE)
	@set -e ; \
		$(CD) $(CROSS_DIR); \
		$(CCMAKE_GUI) $(CCMAKE_CROSS_OPT) .

$(CROSS_FILE):
	@if [ -e $(ATOM_FILE) ]; then \
		$(RM) $(ATOM_FILE); \
	fi
	@if [ -e $(WEBOTS_FILE) ]; then \
		$(RM) $(WEBOTS_FILE); \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(CROSS_FILE); \
		$(MKDIR) -p $(CROSS_DIR); \
		$(CD) $(CROSS_DIR); \
		$(CMAKE) $(CMAKE_CROSS_DEFS) $(CURDIR)/$(CMAKE_DIR)

build_webots:
	$(MAKE) $(MAKE_OPTIONS) -C $(WEBOTS_DIR) ${MAN_TARGET}; \

webots: $(WEBOTS_FILE)
	@set -e;
		$(CD) $(WEBOTS_DIR); \
		$(CCMAKE) .

webots_gui: $(WEBOTS_FILE)
	@set -e;
		$(CD) $(WEBOTS_DIR); \
		$(CCMAKE_GUI) .

$(WEBOTS_FILE):
	@if [ -e $(ATOM_FILE) ]; then \
		$(RM) $(ATOM_FILE); \
	fi
	@if [ -e $(CROSS_FILE) ]; then \
		$(RM) $(CROSS_FILE); \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(WEBOTS_FILE); \
		$(MKDIR) -p $(WEBOTS_DIR); \
		$(CD) $(WEBOTS_DIR); \
		$(CMAKE) $(CMAKE_WEBOTS_DEFS) $(CURDIR)/$(CMAKE_DIR)


install: $(CMAKE_DEPEND)
	@if [ -e $(CROSS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(CROSS_DIR) install; \
		$(CROSS_DIR)/$(INSTALL_SCRIPT); \
	elif [ -e $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(WEBOTS_DIR) install; \
		$(WEBOTS_DIR)/$(INSTALL_SCRIPT); \
	elif [ -e $(ATOM_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(ATOM_DIR) install; \
		$(ATOM_DIR)/$(INSTALL_SCRIPT); \
	else \
		echo "Must have CROSS or WEBOTS build configured to 'make install'"; \
	fi


clean_install:
	$(RM) -r $(CROSS_INSTALL_DIR)/*
	$(RM) -r $(WEBOTS_INSTALL_DIR)/*

cclean: $(CMAKE_DEPEND)
	@$(MAKE) $(MAKE_OPTIONS) -C $(BUILD_DIR) clean

clean:
	$(RM) -r $(BUILD_DIR)

check:
	@if [ -e $(CROSS_FILE) ]; then \
		echo "Making for CROSS"; \
	elif [ -e $(ATOM_FILE) ]; then \
		echo "Making for ATOM"; \
	elif [ -e $(WEBOTS_FILE) ]; then \
		echo "Making for WEBOTS"; \
	fi
