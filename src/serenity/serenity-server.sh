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

NBITES_DIR=$PWD
#TODO make more robust.
AL_DIR=$PWD/lib/naoqi-sdk-1.12.3-linux32
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$nbites_dir/ext/lib
PATH=$nbites_dir/ext/bin:$nbites_dir/src/serenity:$PATH

# Start testing.

git co $BRANCH

cd src

# Man
cd man

result=`make scross`

cd ..

# Tool
cd tool

cd ..

cd .. # to nbites
cd ..
rm -r nbites

cd ..
rm -r $NAME
