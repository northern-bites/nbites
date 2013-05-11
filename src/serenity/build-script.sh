#!/bin/bash

# Man
cd $NBITES_DIR/src/man

make clean

make scross

echo "Making..."
if [ $? -ne 0 ]
then exit $?
fi

make


# Tool
