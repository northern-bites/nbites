SRC_DIR=../../src
MAN_DIR=$(SRC_DIR)/man
LIBRARY_PATH=../../ext/lib

all: oldToNewConvertor newToOldConvertor

oldToNewConvertor: oldToNewConvertor.cpp
	g++ -o $@ $< $(MAN_DIR)/memory/log/ImageLogger.cpp -lrt -I$(SRC_DIR) -I$(MAN_DIR) -I$(MAN_DIR)/include -I../../ext/include -I/usr/include -I../../build/man/cross/cfg

newToOldConvertor: newToOldConvertor.cpp
	g++ -m32 -o $@ $< $(MAN_DIR)/memory/parse/ImageParser.cpp $(LIBRARY_PATH)/libprotobuf.so -lrt -I$(SRC_DIR) -I$(MAN_DIR) -I$(MAN_DIR)/include -I../../ext/include -I/usr/include -I../../build/man/cross/cfg

clean:
	rm newToOldConvertor oldToNewConvertor
