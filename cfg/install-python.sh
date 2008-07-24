#!/bin/sh

###############################################################################
# Script to install the appropriate python files onto a Nao. Necessary
# because the Naos as of NaoQi .18 do not come with math or some other
# key system files
###############################################################################

USER=
DEST=/usr
SOURCE=naopython
SCRIPT_NAME=install-python
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
    tar -xjf $SOURCE.tar.bz2
fi

echo Transferring...
# trailing slashes here are NECESSARY
rsync -rclv --exclude=.svn $SOURCE/ root@$1:$DEST/

