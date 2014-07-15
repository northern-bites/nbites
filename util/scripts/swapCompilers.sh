#!/bin/bash

# Swaps what compiler version you are using (between 1.14 and 2.1)
# Kind of a hack until we can use 2.1 for all robots

OLD=1.14
NEW=2.1

TOOLCHAIN=atomtoolchain
LIB_DIR=$NBITES_DIR/lib
SCRIPTS_DIR=$NBITES_DIR/util/scripts

line=`sed -n 2p $SCRIPTS_DIR/nbites.bash`

if [[ $line == *1.14.5* ]]; then
    echo "current version is 1.14.5"
    echo "Is this correct? (y/n)"
    BACKUP_TOOLCHAIN_DIR=$OLD
    NEW_TOOLCHAIN_DIR=$NEW
    VERSION=2.1.0.19
elif [[ $line == *2.1.0.19* ]]; then
    echo "current version is 2.1.0.19"
    echo "Is this correct? (y/n)"
    BACKUP_TOOLCHAIN_DIR=$NEW
    NEW_TOOLCHAIN_DIR=$OLD
    VERSION=1.14.5
else
    echo "COULDN'T FIND NAOQI VERSION"
    exit 1
fi
read CORRECT

if [ $CORRECT != "y" ]; then
    echo "Exiting"
    exit 1
fi

# Swaps compilers around
mv $LIB_DIR/$TOOLCHAIN $LIB_DIR/$BACKUP_TOOLCHAIN_DIR
mv $LIB_DIR/$NEW_TOOLCHAIN_DIR $LIB_DIR/$TOOLCHAIN

# Sets nbites.bash correctly (Even though we don't use AL_DIR currently)
mv $SCRIPTS_DIR/nbites.bash bashBackup
sed "2 c\export AL_DIR=$LIB_DIR/naoqi-sdk-$VERSION-linux32" bashBackup >> nbites.bash


