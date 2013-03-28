#!/bin/bash

pushd $NBITES_DIR > /dev/null

git push --all > /dev/null

git rev-parse --symbolic-full-name --abbrev-ref HEAD

popd > /dev/null
