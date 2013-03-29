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

ls

rm -r $NAME

ls
