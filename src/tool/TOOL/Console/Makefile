
META_DIR := ../..
DIR := TOOL/Console

.PHONY: all clean run

all:
	$(MAKE) -C $(META_DIR) $(DIR)

clean:
	$(RM) *.class

run:
	$(MAKE) -C $(META_DIR) run

%.class: %.java
	$(MAKE) -C $(META_DIR) $(DIR)/$@
