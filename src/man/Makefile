#Directory setup
CUR_DIR = $(shell pwd)
NBITES_DIR = $(CUR_DIR)/../..
BUILD_DIR = $(NBITES_DIR)/build/man
MAN_DIR = $(CUR_DIR)

ATOM_DIR = $(BUILD_DIR)/atom
GEODE_DIR = $(BUILD_DIR)/geode
WEBOTS_DIR = $(BUILD_DIR)/webots
GEODE_INSTALL_DIR = $(BUILD_DIR)/geode_install
WEBOTS_INSTALL_DIR = $(BUILD_DIR)/webots_install
ATOM_INSTALL_DIR = $(BUILD_DIR)/atom_install
CMAKE_DIR = 
CMAKE_ATOM_DEPEND = $(ATOM_DIR)/Makefile
CMAKE_WEBOTS_DEPEND = $(WEBOTS_DIR)/Makefile
CMAKE_GEDOE_DEPEND = $(GEODE_DIR)/Makefile

#File setup
CONFIG_FILE = manconfig.h
INSTALL_SCRIPT = upload.sh
GEODE_FILE = $(BUILD_DIR)/geode_file
ATOM_FILE = $(BUILD_DIR)/atom_file
WEBOTS_FILE = $(BUILD_DIR)/webots_file

PYC_FILES = $(shell find . -name "*.pyc")

#general definitions
#TODO: in cmake make everything relative to NBITES_DIR
CMAKE_DEFS = -DNBITES_DIR=$(NBITES_DIR)
#toolchains
GEODE_TOOLCHAIN = $(abspath $(CURDIR)/cmake/geode.cmake)
ATOM_TOOLCHAIN = $(abspath $(CURDIR)/cmake/atom.cmake)
#specialized defs
CMAKE_ATOM_DEFS = $(CMAKE_DEFS) -DCMAKE_TOOLCHAIN_FILE=$(ATOM_TOOLCHAIN) -DMAN_INSTALL_PREFIX=$(ATOM_INSTALL_DIR)
CMAKE_GEODE_DEFS = $(CMAKE_DEFS) -DCMAKE_TOOLCHAIN_FILE=$(GEODE_TOOLCHAIN) -DMAN_INSTALL_PREFIX=$(GEODE_INSTALL_DIR)
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

.PHONY: default_target all atom geode cclean clean install noconfig
.PHONY: clean_install


default_target: all

all: $(CMAKE_ATOM_DEPEND) $(CMAKE_WEBOTS_DEPEND) $(CMAKE_GEODE_DEPEND)
	@if [ -e $(GEODE_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(GEODE_DIR) ${MAN_TARGET}; \
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

$(CMAKE_GEODE_DEPEND):
	@if [ -f $(GEODE_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) geode; \
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
	@if [ -e $(GEODE_FILE) ]; then \
		$(RM) $(GEODE_FILE); \
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

build_geode:
	$(MAKE) $(MAKE_OPTIONS) -C $(GEODE_DIR) ${MAN_TARGET}; \


geode: $(GEODE_FILE)
	@set -e ; \
		$(CD) $(GEODE_DIR); \
		$(CCMAKE) $(CCMAKE_CROSS_OPT) .

geode_gui: $(GEODE_FILE)
	@set -e ; \
		$(CD) $(GEODE_DIR); \
		$(CCMAKE_GUI) $(CCMAKE_CROSS_OPT) .

$(GEODE_FILE):
	@if [ -e $(ATOM_FILE) ]; then \
		$(RM) $(ATOM_FILE); \
	fi
	@if [ -e $(WEBOTS_FILE) ]; then \
		$(RM) $(WEBOTS_FILE); \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(GEODE_FILE); \
		$(MKDIR) -p $(GEODE_DIR); \
		$(CD) $(GEODE_DIR); \
		$(CMAKE) $(CMAKE_GEODE_DEFS) $(CURDIR)/$(CMAKE_DIR)

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
	@if [ -e $(GEODE_FILE) ]; then \
		$(RM) $(GEODE_FILE); \
	fi
	@set -e; \
		$(MKDIR) -p $(BUILD_DIR); \
		$(TOUCH) $(WEBOTS_FILE); \
		$(MKDIR) -p $(WEBOTS_DIR); \
		$(CD) $(WEBOTS_DIR); \
		$(CMAKE) $(CMAKE_WEBOTS_DEFS) $(CURDIR)/$(CMAKE_DIR)


install: $(CMAKE_DEPEND)
	@if [ -e $(GEODE_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(GEODE_DIR) install; \
		$(GEODE_DIR)/$(INSTALL_SCRIPT); \
	elif [ -e $(WEBOTS_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(WEBOTS_DIR) install; \
		$(WEBOTS_DIR)/$(INSTALL_SCRIPT); \
	elif [ -e $(ATOM_FILE) ]; then \
		$(MAKE) $(MAKE_OPTIONS) -C $(ATOM_DIR) install; \
		$(ATOM_DIR)/$(INSTALL_SCRIPT); \
	else \
		echo "Must have ATOM/GEODE or WEBOTS build configured to 'make install'"; \
	fi


clean_install:
	$(RM) -r $(GEODE_INSTALL_DIR)/*
	$(RM) -r $(ATOM_INSTALL_DIR)/*
	$(RM) -r $(WEBOTS_INSTALL_DIR)/*

cclean: $(CMAKE_DEPEND)
	@$(MAKE) $(MAKE_OPTIONS) -C $(BUILD_DIR) clean

clean:
	$(RM) -r $(BUILD_DIR)

check:
	@if [ -e $(GEODE_FILE) ]; then \
		echo "Making for GEODE"; \
	elif [ -e $(ATOM_FILE) ]; then \
		echo "Making for ATOM"; \
	elif [ -e $(WEBOTS_FILE) ]; then \
		echo "Making for WEBOTS"; \
	fi
