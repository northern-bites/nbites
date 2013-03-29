#!/bin/bash

# Build Types for serenity.
PULL_REQUEST=0
MOCK=1
COMPILE=2

BUILD_TYPE=$PULL_REQUEST
BRANCH=
NAME=
URL=
while getopts “cmb:n:u:” OPTION
do
     case $OPTION in
         c)
             BUILD_TYPE=$COMPILE
             ;;
         m)
             BUILD_TYPE=$MOCK
             ;;
         b)
             BRANCH=$OPTARG
             ;;
         n)
             NAME=$OPTARG
             ;;
         u)
             URL=$OPTARG
             ;;
     esac
done

mkdir $NAME
cd $NAME

git clone $URL

cd nbites

# Setup environment. Assumes lib and ext of local nbites dir are up to date.

cp -r ../../nbites/lib .
cp -r ../../nbites/ext .

nbites_bash=util/scripts/nbites.bash

echo "export NBITES_DIR=$pwd" >> $nbites_bash
# TODO make this robust.
echo "export AL_DIR=$pwd/lib/naoqi-sdk-1.12.3-linux32" >> $nbites_bash
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib" >> $nbites_bash
echo "export PATH=$nbites_dir/ext/bin:$nbites_dir/src/serenity:$PATH" >> $nbites_bash

source $nbites_bash

# Start testing.

git co $BRANCH

cd src

# Man
cd man

result = make scross

cd ..

# Tool
cd tool

cd ..

cd .. # to nbites
cd ..
rm -r nbites

cd ..
rm -r $NAME
