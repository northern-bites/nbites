#!/bin/bash

INSTALL_TO_PATH="/usr/local/bin/"
echo "NBITES_DIR= " $NBITES_DIR
if [[ $NBITES_DIR == "" ]]
    then
        echo "cannot use nbtool scripts without NBITES_DIR variable.";
        exit 1
fi

echo "using NBITES_DIR=" $NBITES_DIR

cd $NBITES_DIR/src/nbtool/scripts
echo "in" $(pwd)

sudo cp -v ./nbcross $INSTALL_TO_PATH
sudo cp -v ./nbtool $INSTALL_TO_PATH
sudo cp -v ./nbtool-version $INSTALL_TO_PATH
sudo cp -v ./nbtool-class $INSTALL_TO_PATH

echo
echo "done"
