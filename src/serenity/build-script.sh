#!/bin/bash
set -e

# Man
cd $NBITES_DIR/src/man

make clean

make scross

make

# Tool
cd $NBITES_DIR/src/tool

make clean

make
