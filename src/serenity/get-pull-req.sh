#!/bin/bash

BRANCH=
NAME=
URL=
while getopts “b:n:u:” OPTION
do
     case $OPTION in
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

if [[ -z $BRANCH ]] || [[ -z $NAME ]] || [[ -z $URL ]]
then
     exit 1
fi

cd $NBITES_DIR

# Get the Pull Request
git remote add $NAME $URL
git remote update

BRANCH="$NAME/$BRANCH"

git checkout $BRANCH
