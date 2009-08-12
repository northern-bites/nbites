
META_DIR := ../..
DIR := TOOL/Learning

.PHONY: all clean all

all:
	$(MAKE) -C $(META_DIR) $(DIR)

clean:
	$(RM) *.class

run:
	$(MAKE) -C $(META_DIR) run

%.class: %.java
	$(MAKE) -C $(META_DIR) $(DIR)/$@
