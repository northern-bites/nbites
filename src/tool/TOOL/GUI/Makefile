
META_DIR := ../..
DIR := TOOL/GUI

.PHONY: all clean run

all:
	$(MAKE) -C $(META_DIR) $(DIR)

clean:
	$(RM) *.class

run:
	$(MAKE) -C $(META_DIR) run

test: all
	cd $(META_DIR) && java $(DIR)/test

%.class: %.java
	$(MAKE) -C $(META_DIR) $(DIR)/$@
