#!/bin/bash

cd $NBITES_DIR/src/serenity

git remote update

git checkout master

./serenity.py
