#!/bin/bash

cd $NBITES_DIR/src/serenity

git remote update

git checkout master

while [ 1 ]
do
    ./serenity.py
    echo ""
    echo "Serenity crashed... Restarting..."
    echo ""
done
