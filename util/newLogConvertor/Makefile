SRC_DIR=../../src
MAN_DIR=$(SRC_DIR)/man

all: convertor

convertor: convertor.cpp
	g++ -o convertor convertor.cpp $(MAN_DIR)/memory/log/ImageLogger.cpp -lrt -I$(SRC_DIR) -I$(MAN_DIR) -I$(MAN_DIR)/include -I../../ext/include -I/usr/include -I../../build/man/cross/cfg
