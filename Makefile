
CUR_DIR := .#$(shell pwd)
PLATFORM := $(shell uname)

TOOL_DIR := edu/bowdoin/robocup/TOOL
TOOL_SRCS := $(shell ls $(TOOL_DIR)/*.java)
TOOL_OBJS := $(TOOL_SRCS:%.java=%.class)

CALIBRATE_DIR := $(TOOL_DIR)/Calibrate
CALIBRATE_SRCS := $(shell ls $(CALIBRATE_DIR)/*.java)
CALIBRATE_OBJS := $(CALIBRATE_SRCS:%.java=%.class)

CLASSIFIER_DIR := $(TOOL_DIR)/Classifier
CLASSIFIER_SRCS := $(shell ls $(CLASSIFIER_DIR)/*.java)
CLASSIFIER_OBJS := $(CLASSIFIER_SRCS:%.java=%.class)

COLOREDIT_DIR := $(TOOL_DIR)/ColorEdit
COLOREDIT_SRCS := $(shell ls $(COLOREDIT_DIR)/*.java)
COLOREDIT_OBJS := $(COLOREDIT_SRCS:%.java=%.class)

WORLDCONTROLLER_DIR := $(TOOL_DIR)/WorldController
WORLDCONTROLLER_SRCS := $(shell ls $(WORLDCONTROLLER_DIR)/*.java)
WORLDCONTROLLER_OBJS := $(WORLDCONTROLLER_SRCS:%.java=%.class)

CONSOLE_DIR := $(TOOL_DIR)/Console
CONSOLE_SRCS := $(shell ls $(CONSOLE_DIR)/*.java)
CONSOLE_OBJS := $(CONSOLE_SRCS:%.java=%.class)

DATA_DIR := $(TOOL_DIR)/Data
DATA_SRCS := $(shell ls $(DATA_DIR)/*.java)
DATA_OBJS := $(DATA_SRCS:%.java=%.class)

PEDITOR_DIR := $(TOOL_DIR)/PlayBookEditor
PEDITOR_SRCS := $(shell ls $(PEDITOR_DIR)/*.java)
PEDITOR_OBJS := $(PEDITOR_SRCS:%.java=%.class)

FILE_DIR := $(DATA_DIR)/File
FILE_SRCS := $(shell ls $(FILE_DIR)/*.java)
FILE_OBJS := $(FILE_SRCS:%.java=%.class)

GUI_DIR := $(TOOL_DIR)/GUI
GUI_SRCS := $(shell ls $(GUI_DIR)/*.java)
GUI_OBJS := $(GUI_SRCS:%.java=%.class)

IMAGE_DIR := $(TOOL_DIR)/Image
IMAGE_SRCS := $(shell ls $(IMAGE_DIR)/*.java)
IMAGE_OBJS := $(IMAGE_SRCS:%.java=%.class)

NET_DIR := $(TOOL_DIR)/Net
NET_SRCS := $(shell ls $(NET_DIR)/*.java)
NET_OBJS := $(NET_SRCS:%.java=%.class)

SQL_DIR := $(TOOL_DIR)/SQL
SQL_SRCS := $(shell ls $(SQL_DIR)/*.java)
SQL_OBJS := $(SQL_SRCS:%.java=%.class)

VISION_DIR := $(TOOL_DIR)/Vision
VISION_SRCS := $(shell ls $(VISION_DIR)/*.java)
VISION_OBJS := $(VISION_SRCS:%.java=%.class)

#ZSPACE_DIR := $(TOOL_DIR)/ZSpace
#ZSPACE_SRCS := $(shell ls $(ZSPACE_DIR)/*.java)
#ZSPACE_OBJS := $(ZSPACE_SRCS:%.java=%.class)


SRCS = \
	$(TOOL_SRCS) \
	$(CALIBRATE_SRCS) \
	$(CLASSIFIER_SRCS) \
	$(COLOREDIT_SRCS) \
	$(CONSOLE_DIR) \
	$(DATA_SRCS) \
	$(FILE_SRCS) \
	$(GUI_SRCS) \
	$(IMAGE_SRCS) \
	$(NET_SRCS) \
	$(PEDITOR_SRCS) \
	$(SQL_SRCS) \
	$(VISION_SRCS) \
	$(WORLDCONTROLLER_SRCS)
	#$(ZSPACE_SRCS) \

OBJS = $(SRCS:.java=.class)

INSTALL_LIST = installed

SETUP_FILE := setup.py
SETUP_OPTS := --quiet
RECORD_FILE := .installed_files
INSTALL_DIR := ..
INSTALL_CMD := install
INSTALL_OPTS := --install-lib=$(INSTALL_DIR) --record=$(RECORD_FILE)
CLEAN_CMD := clean

MAN_DIR := $(abspath ../man)

LOGGING_DIR := robolog
LOGGING_INSTALL := install_logging
LOGGING_CLEAN := clean_logging
LOGGING_RECORD := $(LOGGING_DIR)/.installed_files
LOGGING_BUILD_DIR := $(LOGGING_DIR)/build

JAR_FILE = TOOL.jar
MAIN = edu.bowdoin.robocup.TOOL.TOOL
MANIFEST = MANIFEST.MF
PYTHON_TOOL = TOOL.py
PYTHON_OBJS = TOOL.pyc

AWK_STRIP_NEWLINE := awk '//,/\n/{printf $$0 ($$0~""?" ":"");next}{print}'
ifeq "$(PLATFORM)" "CYGWIN_NT-5.1"
SED_FIX_BACKSLASH := sed -e "s/..\\\\\([^ ]*\)/..\/\1/g"
else
SED_FIX_BACKSLASH := cat
endif
JAVA = java
JAVAC = javac
JAR = jar
CP = cp
ifeq "$(PLATFORM)" "CYGWIN_NT-5.1"
PYTHON = 'C:/Program Files/Python25/python.exe'
else
PYTHON = python2.5
endif

TRUST_STORE := trustStore
JAVA_OPTS := -Djavax.net.ssl.trustStore=$(TRUST_STORE) -Djava.library.path=./
ifeq ($(PLATFORM),"CYGWIN_NT-5.1")
JAVA_OPTS += -classpath "mysql-connector-java-5.1.6-bin.jar;."
else
JAVA_OPTS += -classpath "mysql-connector-java-5.1.6-bin.jar:."
endif
JAVACFLAGS = -Xlint:unchecked -Xlint:deprecation

JAVA_BUILD := java_build

OBJS = $(SRCS:.java=.class)

INSTALL_LIST = installed
TARGETS = \
	$(TOOL_DIR) \
	$(CALIBRATE_DIR) \
	$(CLASSIFIER_DIR) \
	$(COLOREDIT_DIR) \
	$(DATA_DIR) \
	$(FILE_DIR) \
	$(GUI_DIR) \
	$(IMAGE_DIR) \
	$(NET_DIR) \
	$(SQL_DIR) \
	$(VISION_DIR) \
	$(WORLDCONTROLLER_DIR) \
	$(LOGGING_INSTALL) \
	$(LOGGING_CLEAN) \
	$(JAVA_BUILD)
	#$(PEDITOR_DIR) \
	#$(ZSPACE_DIR) \

.PHONY: all clean clean_java clean_python clean_vision vision run prompt $(TARGETS)

all: $(JAVA_BUILD) #$(LOGGING_INSTALL)

$(JAVA_BUILD): $(OBJS)

$(JAR_FILE): $(OBJS)
	if [ ! -e $(JAR_FILE) ]; then \
		$(JAR) cfm $(JAR_FILE) $(MANIFEST); \
	fi
	find -L $(TOOL_DIR) -name "*.class" | xargs	$(JAR) uf $(JAR_FILE)

$(TOOL_DIR): $(TOOL_OBJS)
$(CALIBRATE_DIR): $(CALIBRATE_OBJS)
$(CLASSIFIER_DIR): $(CLASSIFIER_OBJS)
$(COLOREDIT_DIR): $(COLOREDIT_OBJS)
$(CONSOLE_DIR) : $(CONSOLE_OBJS)
$(DATA_DIR): $(DATA_OBJS)
$(FILE_DIR): $(FILE_OBJS)
$(GUI_DIR): $(GUI_OBJS)
$(IMAGE_DIR): $(IMAGE_OBJS)
$(NET_DIR): $(NET_OBJS)
$(SQL_DIR): $(SQL_OBJS)
$(VISION_DIR): $(VISION_OBJS)
$(WORLDCONTROLLER_DIR): $(WORLDCONTROLLER_OBJS)
#$(PEDITOR_DIR): $(PEDITOR_OBJS)
#$(ZSPACE_DIR): $(ZSPACE_OBJS)

$(LOGGING_INSTALL): robolog.py
robolog.py:
	@#echo $(PYTHON) $(SETUP_FILE) $(SETUP_OPTS) $(INSTALL_CMD) $(INSTALL_OPTS)
	@cd $(LOGGING_DIR) && \
	$(PYTHON) $(SETUP_FILE) $(SETUP_OPTS) $(INSTALL_CMD) $(INSTALL_OPTS)

$(LOGGING_CLEAN):
	$(RM) -r $(LOGGING_BUILD_DIR)
	@echo -n "$(RM) " && \
		$(AWK_STRIP_NEWLINE) $(LOGGING_RECORD) | $(SED_FIX_BACKSLASH) \
		&& echo
	@cd $(LOGGING_DIR) && \
		$(AWK_STRIP_NEWLINE) $(RECORD_FILE) | $(SED_FIX_BACKSLASH) | \
		xargs $(RM) && \
		cat /dev/null > $(RECORD_FILE)

	@echo $(PYTHON) $(SETUP_FILE) $(SETUP_OPTS) $(CLEAN_CMD)
	@cd $(LOGGING_DIR) && \
		$(PYTHON) $(SETUP_FILE) $(SETUP_OPTS) $(CLEAN_CMD)

%.class: %.java
	$(JAVAC) $(JAVACFLAGS) $<

run: all
	$(JAVA) $(JAVA_OPTS) $(MAIN) #-jar $(JAR_FILE)
	@#$(PYTHON) $(PYTHON_TOOL)

prompt: all
	@$(PYTHON) -ic "from jpype import startJVM, JArray, java; startJVM();java.net.URLClassLoader(JArray(java.net.URL)(java.net.URL('jar:file:///home/jfishman/robo/branches/TOOL/mysql-connector-java-5.1.6-bin.jar!/'))).loadClass('com.mysql.jdbc.Driver')"

clean: clean_java clean_python clean_vision

clean_java:
	find . -name "*.class" | xargs $(RM)
	$(RM) $(JAR_FILE)

clean_python: #$(LOGGING_CLEAN)
	find . -name "*.pyc" | xargs $(RM)
	$(RM) $(PYTHON_OBJS)

clean_vision:
	@$(MAKE) $(MAKE_OPTIONS) -C $(VISION_DIR) clean_vision
vision: 
	@$(MAKE) $(MAKE_OPTIONS) -C $(VISION_DIR) vision
