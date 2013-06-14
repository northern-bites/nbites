#!/bin/bash
set -e

#### Man ####
cd $NBITES_DIR/src/man

make clean

# Can we make cross?
make scross
make

make clean

# Can we make straight?
make sstraight
make

# Can we run the tests and do they all pass?
make test


#### Tool ####
cd $NBITES_DIR/src/tool

make clean

make
