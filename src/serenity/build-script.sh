#!/bin/bash

# Man
cd $NBITES_DIR/src/man

make clean

make scross

if [ $? -ne 0 ]
then exit $?
fi

make

if [ $? -ne 0 ]
then exit $?
fi

# Tool
cd $NBITES_DIR/src/tool

make clean

make

if [ $? -ne 0 ]
then exit $?
fi
