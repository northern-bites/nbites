#!/bin/bash

# Man
cd $NBITES_DIR/src/man

make clean

result=`make scross`

echo "Making..."
#if result non zero
#  exit $result

result=`make`

# Tool
