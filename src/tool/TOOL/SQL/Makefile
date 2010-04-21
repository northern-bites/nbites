
META_DIR := ../..
DIR := TOOL/SQL
TEST_CLASS := SQLModule
JAVA := /usr/bin/java
CLASSPATH := mysql-connector-java-5.1.6-bin.jar:.

.PHONY: all clean run

all:
	$(MAKE) -C $(META_DIR) $(DIR)

clean:
	$(RM) *.class

run:
	$(MAKE) -C $(META_DIR) run

test:	all
	cd $(META_DIR) && $(JAVA) -cp "$(CLASSPATH)" $(DIR)/$(TEST_CLASS)

%.class: %.java
	$(MAKE) -C $(META_DIR) $(DIR)/$@
