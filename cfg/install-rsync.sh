#!/bin/sh

###############################################################################
# Script to install rsync onto a Nao. Run this before running install-python.sh
# because install-python.sh uses rsync.
###############################################################################

USER=""
BIN_DEST=/usr/bin
LIB_DEST=/lib

SOURCE=rsync
BIN_SRCS=$SOURCE/rsync
LIB_SRCS=$SOURCE/lib*.so.*
SCRIPT_NAME=install-rsync
OPTIONS="<robot-ip>"

set -e

if [ ! -n "$USER" ]; then
  USER=$(id -nu)
fi

if [ "$1"  == "" ]; then
    echo "Usage: ./$SCRIPT_NAME $OPTIONS"
    exit 1
fi

if [ ! -e "$SOURCE" ]; then
    if [ ! -e "$SOURCE.tar.bz2" ]; then
        curl -Ofku $USER https://robocup.bowdoin.edu/software/nao/$SOURCE.tar.bz2
    fi
    echo Unpacking...
    tar xjf $SOURCE.tar.bz2
fi

echo Transferring...
scp $BIN_SRCS root@$1:$BIN_DEST
scp $LIB_SRCS root@$1:$LIB_DEST
